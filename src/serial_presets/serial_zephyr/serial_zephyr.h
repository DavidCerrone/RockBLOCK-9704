#ifndef SERIAL_ZEPHYR_H
#define SERIAL_ZEPHYR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Sets the serial communication context for Zephyr.
 *
 * @param port The name of the UART device (e.g., "arduino-serial").
 * @param baud The baud rate for communication.
 * @return true if the context was set successfully, false otherwise.
 */
bool setContextZephyr(const char * port, const uint32_t baud);

/**
 * @brief Opens the previously configured UART device.
 *
 * @return true if the device was opened successfully, false otherwise.
 */
bool openPortZephyr(void);

/**
 * @brief Closes the currently open UART device.
 *
 * @return true if the device was closed successfully, false otherwise.
 */
bool closePortZephyr(void);

/**
 * @brief Writes data to the UART device.
 *
 * @param data Pointer to the data to send.
 * @param length Number of bytes to write.
 * @return Number of bytes actually written, or -1 on failure.
 */
int writeZephyr(const char * data, const uint16_t length);

/**
 * @brief Reads data from the UART device.
 *
 * @param bytes Buffer to store the received data.
 * @param length Maximum number of bytes to read.
 * @return Number of bytes actually read, or -1 on failure.
 */
int readZephyr(char * bytes, const uint16_t length);

/**
 * @brief Peeks at the number of bytes available in the receive buffer.
 *
 * @return Number of bytes available to read, or -1 on error.
 */
int peekZephyr(void);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_ZEPHYR_H
