#include "../src/crossplatform.h"
#include <zephyr/kernel.h>

unsigned long millis(void)
{
    return k_uptime_get_32();
}

void delay(uint32_t ms)
{
    k_msleep(ms);
}
