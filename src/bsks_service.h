#ifndef BSKS_SERVICE_H_
#define BSKS_SERVICE_H_

#include <zephyr/bluetooth/uuid.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


#define BSKS_GROUP 0x11 ///< SET ME!

/// @brief BSKS Service UUID
#define BT_UUID_BSKS_SERVICE_VAL BT_UUID_128_ENCODE((0x00000001 | (BSKS_GROUP << 8)), 0xdead, 0xbeef, 0xcafe, 0xbabe0c00ffee)
#define BT_UUID_BSKS_SERVICE     BT_UUID_DECLARE_128(BT_UUID_BSKS_SERVICE_VAL)
/// @brief BSKS Characteristic Nonce
#define BT_UUID_BSKS_CHAR_NONCE_VAL BT_UUID_128_ENCODE((0x00000002 | (BSKS_GROUP << 8)), 0xdead, 0xbeef, 0xcafe, 0xbabe0c00ffee)
#define BT_UUID_BSKS_CHAR_NONCE     BT_UUID_DECLARE_128(BT_UUID_BSKS_CHAR_NONCE_VAL)
/// @brief BSKS Characteristic Hash
#define BT_UUID_BSKS_CHAR_HASH_VAL BT_UUID_128_ENCODE((0x00000003 | (BSKS_GROUP << 8)), 0xdead, 0xbeef, 0xcafe, 0xbabe0c00ffee)
#define BT_UUID_BSKS_CHAR_HASH     BT_UUID_DECLARE_128(BT_UUID_BSKS_CHAR_HASH_VAL)
/// @brief BSKS Characteristic Data
#define BT_UUID_BSKS_CHAR_DATA_VAL BT_UUID_128_ENCODE((0x00000004 | (BSKS_GROUP << 8)), 0xdead, 0xbeef, 0xcafe, 0xbabe0c00ffee)
#define BT_UUID_BSKS_CHAR_DATA     BT_UUID_DECLARE_128(BT_UUID_BSKS_CHAR_DATA_VAL)
/// @brief BSKS Characteristic Control
#define BT_UUID_BSKS_CONTROL_DATA_VAL BT_UUID_128_ENCODE((0x00000005 | (BSKS_GROUP << 8)), 0xdead, 0xbeef, 0xcafe, 0xbabe0c00ffee)
#define BT_UUID_BSKS_CONTROL_DATA     BT_UUID_DECLARE_128(BT_UUID_BSKS_CONTROL_DATA_VAL)


/**
 * @brief Called on server initiated data transmission activation/deactivation.
 * @param active True if indication/notification turned on, otherwise false.
 *
 */
typedef void (*bsks_on_server_initiated_cb)(bool active);

/**
 * @brief Callback called when service calls for disconnection.
 *
 * Whenever data is received, this callback is called.
 * It is called from a context of a stack.
 *
 * @param data Data pointer.
 * @param data_length Data length.
 */
typedef void (*bsks_on_data_cb)(const uint8_t* data, size_t data_length);

/**
 * @brief Called when @ref bsks_set_data finished its work successfully.
 *
 * Indicates that data has been stored in server or has been successfully
 * sent via indication/notification mechanism.
 */
typedef void (*bsks_on_data_sent_cb)(void);

/**
 * @brief Callbacks placeholder.
 */
typedef struct {
    bsks_on_server_initiated_cb on_server_initiated;
    bsks_on_data_cb             on_data;
    bsks_on_data_sent_cb        on_data_sent;
} bsks_service_cb;

/**
 * @brief Initializes BSKS Service.
 * @param cb Callbacks placeholder structure, must not be NULL.
 */
int bsks_service_init(const bsks_service_cb* cb);

/**
 * @brief Data setter/sender.
 *
 * Sets data in characteristics, sends data to peer if indication/notification is enabled.
 * @ref bsks_on_data_sent_cb is called whenever data has been successfully sent.
 */
int bsks_set_data(const uint8_t* data, size_t data_length);

#endif // BSKS_SERVICE_H_