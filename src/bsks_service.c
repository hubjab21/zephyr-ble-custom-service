#include "bsks_service.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include <errno.h>
#include <stddef.h>
#include <string.h>



#define LOG_LEVEL CONFIG_BSKS_LOG_LEVEL
LOG_MODULE_REGISTER(bsks_service, LOG_LEVEL);
static struct k_mutex my_mutex;

static uint8_t         data_buffer[255];
static size_t          data_buffer_size = sizeof(data_buffer);
static bsks_service_cb bsks_service_cb_ptr;


static ssize_t write_hash(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags) {
    LOG_DBG("write_hash cb");
    return 0;
}

static ssize_t read_nonce(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset) {
    LOG_DBG("read_nonce cb");
    return 0;
}

static ssize_t write_data(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags) {
    LOG_DBG("DATA write, h: %u, c: %p", attr->handle, (void*)conn);
    if (offset != 0) {
        LOG_DBG("Incorrect data offset on write BSKS Data");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    if (bsks_service_cb_ptr.on_data) {
        bsks_service_cb_ptr.on_data(buf, len);
    }
    return len;
}
static ssize_t write_control(struct bt_conn* conn, const struct bt_gatt_attr* attr, const void* buf, uint16_t len, uint16_t offset, uint8_t flags) {
    LOG_DBG("write_control cb");
    return 0;
}

static ssize_t read_data(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset) {
    LOG_DBG("read_data cb");
}

static ssize_t read_control(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset) {
    LOG_DBG("read_control cb");
    return 0;
}


static bool indication_enable = 0;

static void ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value) {
    indication_enable = (value == BT_GATT_CCC_INDICATE);
    if (NULL != bsks_service_cb_ptr.on_server_initiated) {
        bsks_service_cb_ptr.on_server_initiated(indication_enable);
    }
    LOG_DBG("BSKS indication %s", indication_enable ? "on" : "off");
}

int bsks_service_init(const bsks_service_cb* cb) {
    LOG_DBG("initing mutex");
    k_mutex_init(&my_mutex);
    bsks_service_cb_ptr = *cb;
    return 0;
}


BT_GATT_SERVICE_DEFINE(bsks_svc,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_BSKS_SERVICE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_BSKS_CHAR_NONCE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_nonce, NULL, NULL),
                       BT_GATT_CHARACTERISTIC(BT_UUID_BSKS_CHAR_HASH, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, write_hash, NULL),
                       BT_GATT_CHARACTERISTIC(BT_UUID_BSKS_CHAR_DATA, BT_GATT_CHRC_WRITE | BT_GATT_CHRC_READ | BT_GATT_CHRC_INDICATE, BT_GATT_PERM_WRITE | BT_GATT_PERM_READ, read_data, write_data, NULL),
                       BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_BSKS_CONTROL_DATA, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ, read_control, write_control, NULL));
