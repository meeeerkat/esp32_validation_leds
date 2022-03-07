
#include "validation_leds.h"

#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


static xQueueHandle blink_led_command_queue = NULL;


void blink_validation_led(uint32_t led_gpio)
{
    xQueueSend(blink_led_command_queue, &led_gpio, 0);
}

void validation_leds_task(void *args)
{
    uint32_t led_gpio;
    while (1) {
        if(xQueueReceive(blink_led_command_queue, &led_gpio, portMAX_DELAY)) {
            gpio_set_level(led_gpio, 1);
            vTaskDelay(pdMS_TO_TICKS(CONFIG_LED_BLINK_DURATION));
            gpio_set_level(led_gpio, 0);
        }
    }
}

void init_validation_leds(void)
{
    gpio_set_direction(SUCCESS_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(FAILURE_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(SUCCESS_LED, 0);
    gpio_set_level(FAILURE_LED, 0);

    blink_led_command_queue = xQueueCreate(5, sizeof(uint32_t));
    xTaskCreate(validation_leds_task, "validation_leds_task", 2048, NULL, 5, NULL);
}
