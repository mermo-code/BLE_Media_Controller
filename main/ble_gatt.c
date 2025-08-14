#include "ble_gatt.h"
#include "hid_vars.h"
#include "host/ble_gatt.h"

/***************GATT SERVICE DEFINITIONS***************/
const struct ble_gatt_svc_def GATT_SVR_SVCS_DEF[] = {
	{
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_SERVICE),
		.characteristics =
			(struct ble_gatt_chr_def[]){
				{
					/*** HID Information */
					.uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_INFORMATION),
					.flags = BLE_GATT_CHR_F_READ,
					.access_cb = hid_svc_chr_access,
				},
				{
					/*** HID Control Point */
					.uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_CONTROL_POINT),
					.flags = BLE_GATT_CHR_F_WRITE_NO_RSP,
					.access_cb = hid_svc_chr_access,
				},
				{
					/*** HID Report Map */
					.uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_REPORT_MAP),
					.flags = BLE_GATT_CHR_F_READ,
					.access_cb = hid_svc_chr_access,
				},
				{
					/*** HID Report */
					.uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_INPUT_REPORT),
					.flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
					.access_cb = hid_svc_chr_access,
					.val_handle = &HANDLE_HID_REPORT,
					.descriptors =
						(struct ble_gatt_dsc_def[]){
							{
								.uuid = BLE_UUID16_DECLARE(
									GATT_UUID_HID_RPT_REF_DESCR),
								.att_flags = BLE_ATT_F_READ,
								.access_cb = hid_svc_dsc_access,
							},
							{
								/*** End of Descriptors in this Characteristic
								 */
								0,
							},
						},
				},
				{
					/*** End of Characteristics*/
					0,
				},

			},
	},
	{
		/*** End of Services */
		0,
	},
};

/****************SERVICE CHARACTERISTIC ACCESS****************/
int hid_svc_chr_access(uint16_t conn_handle, uint16_t attr_handle,
					   struct ble_gatt_access_ctxt *ctxt, void *arg) {
	uint16_t uuid16 = ble_uuid_u16(ctxt->chr->uuid);
	int rc;

	ESP_LOGI("Event", "Accessing characteristic, 0x%04X, (Attr handle: %d)",
			 uuid16, attr_handle);

	switch (uuid16) {
	/************************************************************************/
	case GATT_UUID_HID_INFORMATION: /*** Access HID Information */
		if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
			ESP_LOGW("Event", "Invalid operation: %d", ctxt->op);
			break;
		}
		rc = os_mbuf_append(ctxt->om, hid_info, LEN_HID_INFO);
		if (!rc) {
			ESP_LOGI("Event", "Sending HID Information");
		} else {
			ESP_LOGE("Event", "Error sending HID Information, Error=%d", rc);
		}
		return rc;
	/************************************************************************/
	case GATT_UUID_HID_CONTROL_POINT: /*** Access HID Control Point */
		if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
			ESP_LOGW("Event", "Invalid operation: %d", ctxt->op);
			break;
		}
		uint8_t command = ctxt->om->om_data[0];
		switch (command) {
		case 0x00:
			ESP_LOGI("Event", "HID Control Point: Suspend command received");
			// Optional: Set a flag to pause HID reports
			break;
		case 0x01:
			ESP_LOGI("Event",
					 "HID Control Point: Exit Suspend command received");
			// Optional: Resume HID reports
			break;
		default:
			ESP_LOGE("Event", "HID Control Point: Unknown command 0x%02X",
					 command);
			return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
		}
		return 0;
	/************************************************************************/
	case GATT_UUID_HID_REPORT_MAP: /*** Access HID Report Map */
		if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
			ESP_LOGW("Event", "Invalid operation: %d", ctxt->op);
			break;
		}
		rc = os_mbuf_append(ctxt->om, hid_descriptor_report,
							LEN_HID_DESCRIPTOR_REPORT);
		if (!rc) {
			ESP_LOGI("Event", "Sending HID Report Map");
		} else {
			ESP_LOGE("Event", "Error sending HID Report Map, Error=%d", rc);
		}
		return rc;
	/************************************************************************/
	case GATT_UUID_HID_INPUT_REPORT:
		if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
			ESP_LOGW("Event", "Invalid operation: %d", ctxt->op);
			break;
		}
		rc = os_mbuf_append(ctxt->om, &hid_input_report,
							sizeof(hid_input_report));
		if (!rc) {
			ESP_LOGI("Event", "Sending HID Report");
		} else {
			ESP_LOGE("Event", "Error sending HID Report, Error=%d", rc);
		}
		return rc;
	/************************************************************************/
	default:
		ESP_LOGW("Event", "Invalid UUID: 0x%04X", uuid16);
		break;
	}
	return BLE_ATT_ERR_UNLIKELY;
}

/****************SERVICE DESCRIPTOR ACCESS****************/
int hid_svc_dsc_access(uint16_t conn_handle, uint16_t attr_handle,
					   struct ble_gatt_access_ctxt *ctxt, void *arg) {
	uint16_t uuid16 = ble_uuid_u16(ctxt->dsc->uuid);
	int rc;

	ESP_LOGI("Event", "Accessing descriptor, 0x%04X, (Attr handle: %d)", uuid16,
			 attr_handle);

	switch (uuid16) {
	/************************************************************************/
	case GATT_UUID_HID_RPT_REF_DESCR: // Accessing HID Report Reference
		if (ctxt->op != BLE_GATT_ACCESS_OP_READ_DSC) {
			ESP_LOGW("Event", "Invalid operation: %d", ctxt->op);
			break;
		}
		rc = os_mbuf_append(ctxt->om, report_reference,
							LEN_HID_REPORT_REFERENCE);
		if (!rc) {
			ESP_LOGI("Event", "Sending HID Report Reference");
		} else {
			ESP_LOGE("Event", "Error sending HID Report Reference, Error=%d",
					 rc);
		}
		return rc;
	/************************************************************************/
	default:
		ESP_LOGW("Event", "Invalid UUID: 0x%04X", uuid16);
		break;
	}
	return BLE_ATT_ERR_UNLIKELY;
}