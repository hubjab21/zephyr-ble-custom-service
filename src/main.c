#include <autoconf.h>
#include <dk_buttons_and_leds.h>
#include <soc.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>


#define LOG_LEVEL CONFIG_BSKS_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bsks_app);

#include "bsks_service.h"

#define LED_RUN_STATUS                   DK_LED1
#define LED_RUN_STATUS_BLINK_INTERVAL_MS 500

#define BUTTON_1    DK_BTN1_MSK
#define DEVICE_NAME "BSKS 11"

uint16_t gap_apperance[2] = {320, 480};

static const struct bt_data ad[] = {BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
                                    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, sizeof(DEVICE_NAME)),
                                    BT_DATA(BT_DATA_GAP_APPEARANCE, gap_apperance, sizeof(gap_apperance))};

static const struct bt_le_adv_param* adv_param = BT_LE_ADV_PARAM((BT_LE_ADV_OPT_ONE_TIME | BT_LE_ADV_OPT_CONNECTABLE), 160, 320, NULL);

struct bt_conn* glob_conn;
int             reenable_adv;

static void bt_enable_cb() {
    LOG_DBG("Bluetooth Enable");
}

static void button_changed(uint32_t state, uint32_t button) {
    LOG_DBG("Button %" PRIx32 ", has changed %" PRIx32, button, state);
}

static void bt_register_connected(struct bt_conn* conn, uint8_t err) {
    if (!err) {
        LOG_DBG("Connection error");
        bt_addr_le_t* adress = bt_conn_get_dst(conn);
        char          str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(adress, str, BT_ADDR_LE_STR_LEN);
        glob_conn = conn;
    } else {
        LOG_DBG("Connection error %d", err);
    }
}

static void bt_register_disconnected(struct bt_conn* conn, uint8_t reason) {
    LOG_DBG("Disconnected %d", reason);
    if (reason == BT_HCI_ERR_REMOTE_USER_TERM_CONN) {
        LOG_DBG("User");
    } else if (BT_HCI_ERR_CONN_TIMEOUT) {
        LOG_DBG("Timeout");
    } else {
        LOG_DBG("%x", reason);
    }
    reenable_adv = 1;
}

static void bt_register_update(struct bt_conn* conn, uint16_t interval, uint16_t latency, uint16_t timeout) {
    LOG_DBG("Parameter updated");
    LOG_DBG("interval %d", interval);
    LOG_DBG("latency %d", latency);
    LOG_DBG("timeout %d", timeout);
}

void my_bsks_on_server_initiated_cb(bool active) {
    struct bt_le_conn_param paramy = {.interval_min = 8, .interval_max = 24, .latency = 2, .timeout = 200};


    if (active) {
        LOG_DBG("indication on");
    } else {
        paramy.interval_min = CONFIG_BT_PERIPHERAL_PREF_MIN_INT;
        paramy.interval_max = CONFIG_BT_PERIPHERAL_PREF_MAX_INT;
        paramy.latency      = CONFIG_BT_PERIPHERAL_PREF_LATENCY;
        paramy.timeout      = CONFIG_BT_PERIPHERAL_PREF_TIMEOUT;
        LOG_DBG("indication off");
    }
    bt_conn_le_param_update(glob_conn, &paramy);
}

void my_bsks_on_data_sent_cb() {
    LOG_DBG("Data sent");
}

void my_bsks_on_data_cb(const uint8_t* data, size_t data_length) {
    LOG_DBG("Data: %s", (char*)data);
}

int main(void) {
    printk("Starting BSKS peripheral device\n");

    int err;
    if ((err = dk_leds_init())) {
        LOG_ERR("LEDs init failed (err %d)", err);
        k_oops();
    }

    if ((err = dk_buttons_init(button_changed))) {
        LOG_ERR("Button init failed (err %d)", err);
        k_oops();
    }

    if ((err = bt_enable(bt_enable_cb))) {
        LOG_ERR("BLuetooth enabled failed (err %d)", err);
        k_oops();
    }

    struct bt_conn_cb cb = {.connected = bt_register_connected, .disconnected = bt_register_disconnected, .le_param_updated = bt_register_update};

    bt_conn_cb_register(&cb);


    bsks_service_cb my_bsks_service = {
        .on_server_initiated = my_bsks_on_server_initiated_cb,
        .on_data_sent        = my_bsks_on_data_sent_cb,
        .on_data             = my_bsks_on_data_cb,
    };
    bsks_service_init(&my_bsks_service);

    if ((err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), NULL, 0))) {
        LOG_ERR("BLuetooth advertising failed (err %d)", err);
        k_oops();
    }

    int blink_status = 0;
    while (true) {
        dk_set_led(LED_RUN_STATUS, (++blink_status) % 2);
        k_sleep(K_MSEC(LED_RUN_STATUS_BLINK_INTERVAL_MS));
    }
}