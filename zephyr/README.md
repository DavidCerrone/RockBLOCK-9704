# RockBLOCK 9704 Zephyr Integration

This directory contains the Zephyr RTOS integration for the RockBLOCK 9704 Iridium satellite communication library.

## Overview

The RockBLOCK 9704 is an Iridium satellite communication module that enables global, two-way communication for IoT devices. This integration allows you to use the RockBLOCK 9704 with Zephyr RTOS applications.

## Files

- `module.yml` - Zephyr module definition
- `CMakeLists.txt` - Build configuration for Zephyr
- `Kconfig` - Configuration options
- `crossplatform_zephyr.c` - Zephyr timing functions
- `gpio_zephyr.c` - GPIO control implementation (optional)
- `iridium.h` - Simplified API wrapper
- `README.md` - This file

## Serial Presets

The Zephyr serial implementation is located in:
- `../src/serial_presets/serial_zephyr/serial_zephyr.h`
- `../src/serial_presets/serial_zephyr/serial_zephyr.c`

## Usage

### Configuration

Add to your `prj.conf`:
```ini
CONFIG_IRIDIUM=y
CONFIG_IRIDIUM_UART_DEVICE="arduino-serial"
CONFIG_IRIDIUM_QUEUE_SIZE=2
CONFIG_IRIDIUM_LOG_LEVEL=3
```

### Basic Usage

```c
#include <iridium.h>

int main(void)
{
    // Initialize Iridium
    if (!iridium_init("arduino-serial")) {
        printk("Failed to initialize Iridium\n");
        return -1;
    }

    // Wait for modem to be ready
    k_msleep(150);

    // Send a message
    const char *msg = "Hello from Zephyr!";
    if (iridium_send(msg, strlen(msg))) {
        printk("Message queued\n");
    }

    // Main loop
    while (1) {
        // Must call frequently (at least every 50ms)
        iridium_poll();

        // Check for received messages
        char *received_msg;
        size_t msg_len = iridium_receive(&received_msg);
        if (msg_len > 0) {
            printk("Received: %.*s\n", msg_len, received_msg);
            rbAcknowledgeReceiveHeadAsync(); // Free up queue space
        }

        k_msleep(10);
    }

    return 0;
}
```

## Hardware Setup

### nRF5340DK Pin Connections

| RockBLOCK 9704 Pin | nRF5340DK Pin | Function |
|-------------------|---------------|----------|
| TXD               | P1.00         | UART1 RX |
| RXD               | P1.01         | UART1 TX |
| RTS               | P1.02         | UART1 RTS |
| CTS               | P1.03         | UART1 CTS |
| VCC               | 5V            | Power |
| GND               | GND           | Ground |

### Device Tree

Your device tree overlay should include:
```dts
&uart1 {
    status = "okay";
    current-speed = <230400>;
    // pin configuration...
};

/ {
    aliases {
        arduino-serial = &uart1;
    };
};
```

## Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `CONFIG_IRIDIUM` | n | Enable Iridium support |
| `CONFIG_IRIDIUM_UART_DEVICE` | "arduino-serial" | UART device name |
| `CONFIG_IRIDIUM_QUEUE_SIZE` | 1 | Message queue size |
| `CONFIG_IRIDIUM_AUTO_TELEMETRY` | y | Automatic telemetry |
| `CONFIG_IRIDIUM_TELEMETRY_INTERVAL` | 300 | Telemetry interval (seconds) |
| `CONFIG_IRIDIUM_ENABLE_GPIO` | n | GPIO power control |
| `CONFIG_IRIDIUM_LOG_LEVEL` | 3 | Log verbosity (0-4) |

## Features

- ✅ UART communication using Zephyr drivers
- ✅ Asynchronous message sending and receiving
- ✅ Signal strength monitoring
- ✅ Configurable via Kconfig
- ✅ Optional GPIO control for power management
- ✅ Optional Kermit protocol support for firmware updates
- ✅ Thread-safe operation

## Integration Notes

This integration is designed to work seamlessly with the original RockBLOCK 9704 library while providing Zephyr-specific implementations for:

1. **Serial Communication**: Uses Zephyr UART API with ring buffers
2. **Timing Functions**: Uses Zephyr kernel timing primitives
3. **GPIO Control**: Uses Zephyr GPIO API
4. **Logging**: Integrates with Zephyr logging system

The library automatically detects Zephyr (`__ZEPHYR__` macro) and uses the appropriate implementations.

## Building

The module is automatically built when `CONFIG_IRIDIUM=y` is set. No additional build steps are required.

## Troubleshooting

1. **UART Issues**: Ensure your device tree correctly configures the UART and aliases
2. **Signal Issues**: RockBLOCK requires clear view of sky for satellite communication
3. **Memory Issues**: Increase heap size if experiencing allocation failures
4. **Timing Issues**: Ensure `iridium_poll()` is called at least every 50ms

For more details, see the main library documentation and examples in the parent directory.
