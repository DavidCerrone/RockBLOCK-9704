#ifndef IRIDIUM_H
#define IRIDIUM_H

/**
 * @file iridium.h
 * @brief Zephyr wrapper for RockBLOCK 9704 Iridium satellite communication library
 * 
 * This header provides a simplified interface for using the RockBLOCK 9704 module
 * with Zephyr RTOS. Make sure to enable CONFIG_IRIDIUM in your project configuration.
 * 
 * Usage example:
 * @code
 * #include <iridium.h>
 * 
 * // Initialize Iridium with arduino-serial UART
 * if (iridium_init("arduino-serial")) {
 *     // Send a message
 *     iridium_send("Hello from space!", 17);
 *     
 *     // Check for incoming messages
 *     char *received_msg;
 *     size_t msg_len = iridium_receive(&received_msg);
 *     if (msg_len > 0) {
 *         // Process received message
 *     }
 * }
 * @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "../src/rockblock_9704.h"

/**
 * @brief Initialize the Iridium communication module
 * 
 * This function initializes the RockBLOCK 9704 module using the specified UART device.
 * The UART device should be defined in your device tree (e.g., arduino-serial).
 * 
 * @param uart_device_name Name of the UART device to use (e.g., "arduino-serial")
 * @return true if initialization successful, false otherwise
 */
static inline bool iridium_init(const char *uart_device_name)
{
    return rbBegin(uart_device_name);
}

/**
 * @brief Send a message via Iridium satellite
 * 
 * @param data Pointer to message data
 * @param length Length of message data (max 100kB for non-Arduino platforms)
 * @return true if message queued successfully, false otherwise
 */
static inline bool iridium_send(const char *data, size_t length)
{
    return rbSendMessageAsync(RAW_TOPIC, data, length);
}

/**
 * @brief Send a message on a specific topic
 * 
 * @param topic Topic ID (use cloudloopTopics_t enum or custom topic ID)
 * @param data Pointer to message data
 * @param length Length of message data
 * @return true if message queued successfully, false otherwise
 */
static inline bool iridium_send_topic(uint16_t topic, const char *data, size_t length)
{
    return rbSendMessageAsync(topic, data, length);
}

/**
 * @brief Check for and receive incoming messages
 * 
 * @param buffer Pointer to receive buffer pointer (will be set to received data)
 * @return Length of received message, 0 if no message available
 */
static inline size_t iridium_receive(char **buffer)
{
    return rbReceiveMessageAsync(buffer);
}

/**
 * @brief Poll for Iridium communication events
 * 
 * This function must be called frequently (at least every 50ms) to handle
 * communication with the Iridium modem.
 */
static inline void iridium_poll(void)
{
    rbPoll();
}

/**
 * @brief Get current signal strength
 * 
 * @return Signal strength in bars (0-5), or -1 on error
 */
static inline int8_t iridium_get_signal(void)
{
    return rbGetSignal();
}

/**
 * @brief Close Iridium communication
 * 
 * @return true if successful, false otherwise
 */
static inline bool iridium_close(void)
{
    return rbEnd();
}

#ifdef __cplusplus
}
#endif

#endif // IRIDIUM_H
