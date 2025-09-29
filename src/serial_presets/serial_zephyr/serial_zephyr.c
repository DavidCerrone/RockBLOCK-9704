#include "serial_zephyr.h"
#include "../../serial.h"
#include <zephyr/logging/log.h>
#include <zephyr/sys/ring_buffer.h>
#include <string.h>

LOG_MODULE_REGISTER(iridium_serial, CONFIG_IRIDIUM_LOG_LEVEL);

// UART device and receive buffer
static const struct device *uart_dev;
static struct uart_config uart_cfg;

// Ring buffer for incoming data
#define RX_BUFFER_SIZE 1024
static uint8_t rx_buffer_data[RX_BUFFER_SIZE];
static struct ring_buf rx_ring_buf;

// Serial variables from the original library
extern enum serialState serialState;
extern serialContext context;

// UART callback for incoming data
static void uart_rx_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
    switch (evt->type) {
    case UART_RX_RDY:
        // Add received data to ring buffer
        ring_buf_put(&rx_ring_buf, evt->data.rx.buf + evt->data.rx.offset, evt->data.rx.len);
        break;
        
    case UART_RX_DISABLED:
        // Re-enable RX if it gets disabled
        uart_rx_enable(dev, rx_buffer_data, RX_BUFFER_SIZE, 1);
        break;
        
    default:
        break;
    }
}

bool setContextZephyr(const char * port, const uint32_t baud)
{
    // Get UART device
    uart_dev = device_get_binding(port);
    if (!uart_dev) {
        LOG_ERR("Failed to get UART device: %s", port);
        return false;
    }

    // Initialize ring buffer
    ring_buf_init(&rx_ring_buf, RX_BUFFER_SIZE, rx_buffer_data);

    // Set up context
    strncpy(context.serialPort, port, SERIAL_PORT_LENGTH - 1);
    context.serialPort[SERIAL_PORT_LENGTH - 1] = '\0';
    context.serialBaud = baud;
    context.serialInit = openPortZephyr;
    context.serialDeInit = closePortZephyr;
    context.serialRead = readZephyr;
    context.serialWrite = writeZephyr;
    context.serialPeek = peekZephyr;

    // Test the connection
    if (context.serialInit()) {
        if (context.serialDeInit()) {
            return true;
        }
    }
    return false;
}

bool openPortZephyr(void)
{
    if (serialState == OPEN) {
        return true;
    }

    if (!uart_dev) {
        LOG_ERR("UART device not initialized");
        return false;
    }

    // Configure UART
    uart_cfg.baudrate = context.serialBaud;
    uart_cfg.parity = UART_CFG_PARITY_NONE;
    uart_cfg.stop_bits = UART_CFG_STOP_BITS_1;
    uart_cfg.data_bits = UART_CFG_DATA_BITS_8;
    uart_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

    int ret = uart_configure(uart_dev, &uart_cfg);
    if (ret) {
        LOG_ERR("Failed to configure UART: %d", ret);
        return false;
    }

    // Set up callback for async operation
    ret = uart_callback_set(uart_dev, uart_rx_callback, NULL);
    if (ret) {
        LOG_ERR("Failed to set UART callback: %d", ret);
        return false;
    }

    // Enable RX
    ret = uart_rx_enable(uart_dev, rx_buffer_data, RX_BUFFER_SIZE, 1);
    if (ret) {
        LOG_ERR("Failed to enable UART RX: %d", ret);
        return false;
    }

    serialState = OPEN;
    LOG_INF("UART opened successfully on %s at %d baud", context.serialPort, context.serialBaud);
    return true;
}

bool closePortZephyr(void)
{
    if (serialState == CLOSED) {
        return true;
    }

    if (uart_dev) {
        uart_rx_disable(uart_dev);
        uart_callback_set(uart_dev, NULL, NULL);
    }

    serialState = CLOSED;
    LOG_INF("UART closed");
    return true;
}

int writeZephyr(const char * data, const uint16_t length)
{
    if (serialState != OPEN || !uart_dev) {
        return -1;
    }

    // Send data using polling TX
    for (uint16_t i = 0; i < length; i++) {
        uart_poll_out(uart_dev, data[i]);
    }

    return length;
}

int readZephyr(char * bytes, const uint16_t length)
{
    if (serialState != OPEN || !bytes) {
        return -1;
    }

    // Read from ring buffer
    uint32_t read_len = ring_buf_get(&rx_ring_buf, (uint8_t *)bytes, length);
    
    return (int)read_len;
}

int peekZephyr(void)
{
    if (serialState != OPEN) {
        return -1;
    }

    // Return number of bytes available in ring buffer
    return (int)ring_buf_size_get(&rx_ring_buf);
}
