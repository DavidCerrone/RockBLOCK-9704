#include "../src/gpio.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(iridium_gpio, CONFIG_IRIDIUM_LOG_LEVEL);

// Static GPIO device handles
static const struct device *power_enable_dev;
static const struct device *iridium_enable_dev;
static const struct device *boot_dev;

// GPIO configuration for Zephyr
static struct gpio_dt_spec power_enable_pin = {
    .port = NULL,
    .pin = CONFIG_IRIDIUM_POWER_ENABLE_PIN,
    .dt_flags = GPIO_ACTIVE_HIGH
};

static struct gpio_dt_spec iridium_enable_pin = {
    .port = NULL,
    .pin = CONFIG_IRIDIUM_IRIDIUM_ENABLE_PIN,
    .dt_flags = GPIO_ACTIVE_HIGH
};

static struct gpio_dt_spec boot_pin = {
    .port = NULL,
    .pin = CONFIG_IRIDIUM_BOOT_PIN,
    .dt_flags = GPIO_ACTIVE_HIGH
};

static bool gpio_init_done = false;

static bool init_gpio_devices(void)
{
    if (gpio_init_done) {
        return true;
    }

    // Get GPIO devices
    power_enable_dev = device_get_binding(CONFIG_IRIDIUM_POWER_ENABLE_GPIO);
    iridium_enable_dev = device_get_binding(CONFIG_IRIDIUM_IRIDIUM_ENABLE_GPIO);
    boot_dev = device_get_binding(CONFIG_IRIDIUM_BOOT_GPIO);

    if (!power_enable_dev || !iridium_enable_dev || !boot_dev) {
        LOG_ERR("Failed to get GPIO devices");
        return false;
    }

    // Set device pointers
    power_enable_pin.port = power_enable_dev;
    iridium_enable_pin.port = iridium_enable_dev;
    boot_pin.port = boot_dev;

    // Configure pins
    if (gpio_pin_configure_dt(&power_enable_pin, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Failed to configure power enable pin");
        return false;
    }

    if (gpio_pin_configure_dt(&iridium_enable_pin, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Failed to configure iridium enable pin");
        return false;
    }

    if (gpio_pin_configure_dt(&boot_pin, GPIO_INPUT) < 0) {
        LOG_ERR("Failed to configure boot pin");
        return false;
    }

    gpio_init_done = true;
    return true;
}

bool gpioToggle(const char * selectedChip, int selectedPin, int value)
{
    if (!init_gpio_devices()) {
        return false;
    }

    struct gpio_dt_spec *pin = NULL;
    
    // Map chip and pin to our configured pins
    if (selectedPin == CONFIG_IRIDIUM_POWER_ENABLE_PIN) {
        pin = &power_enable_pin;
    } else if (selectedPin == CONFIG_IRIDIUM_IRIDIUM_ENABLE_PIN) {
        pin = &iridium_enable_pin;
    } else if (selectedPin == CONFIG_IRIDIUM_BOOT_PIN) {
        pin = &boot_pin;
    }

    if (!pin) {
        LOG_ERR("Unknown GPIO pin: %d", selectedPin);
        return false;
    }

    return gpio_pin_set_dt(pin, value) == 0;
}

bool gpioDriveHigh(const char * selectedChip, int selectedPin)
{
    return gpioToggle(selectedChip, selectedPin, 1);
}

bool gpioDriveLow(const char * selectedChip, int selectedPin)
{
    return gpioToggle(selectedChip, selectedPin, 0);
}

int gpioReceive(const char * selectedChip, int selectedPin)
{
    if (!init_gpio_devices()) {
        return -1;
    }

    struct gpio_dt_spec *pin = NULL;
    
    if (selectedPin == CONFIG_IRIDIUM_BOOT_PIN) {
        pin = &boot_pin;
    } else {
        LOG_ERR("GPIO read not supported for pin: %d", selectedPin);
        return -1;
    }

    return gpio_pin_get_dt(pin);
}

bool gpioListenIridBooted(const char * selectedChip, int selectedPin, const int timeout)
{
    if (!init_gpio_devices()) {
        return false;
    }

    int64_t start_time = k_uptime_get();
    int64_t timeout_ms = timeout * 1000LL;

    while ((k_uptime_get() - start_time) < timeout_ms) {
        if (gpioReceive(selectedChip, selectedPin) == 1) {
            return true;
        }
        k_msleep(10); // Small delay to avoid busy waiting
    }

    return false;
}

// Default GPIO table - this can be overridden by the user
const rbGpioTable_t gpioTable = {
    .powerEnable = { .chip = CONFIG_IRIDIUM_POWER_ENABLE_GPIO, .pin = CONFIG_IRIDIUM_POWER_ENABLE_PIN },
    .iridiumEnable = { .chip = CONFIG_IRIDIUM_IRIDIUM_ENABLE_GPIO, .pin = CONFIG_IRIDIUM_IRIDIUM_ENABLE_PIN },
    .booted = { .chip = CONFIG_IRIDIUM_BOOT_GPIO, .pin = CONFIG_IRIDIUM_BOOT_PIN }
};
