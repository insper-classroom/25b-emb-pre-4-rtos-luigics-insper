#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>


const int LED_PIN_R = 4;
const int BTN_PIN = 7;
const int LED_PIN_G = 6;
SemaphoreHandle_t xSemaphore_r;

void led_1_task(void *p) {
  gpio_init(LED_PIN_R);
  gpio_set_dir(LED_PIN_R, GPIO_OUT);

  int delay = 250;
  while (true) {
    gpio_put(LED_PIN_R, 1);
    vTaskDelay(pdMS_TO_TICKS(delay));
    gpio_put(LED_PIN_R, 0);
    vTaskDelay(pdMS_TO_TICKS(delay));
  }
}

void led_2_task(void *p) {
  gpio_init(LED_PIN_G);
  gpio_set_dir(LED_PIN_G, GPIO_OUT);

  int delay = 250;
  while (true) {
    if (xSemaphoreTake(xSemaphore_r,pdMS_TO_TICKS(500)) == pdTRUE){
      gpio_put(LED_PIN_G, 1);
      vTaskDelay(pdMS_TO_TICKS(delay));
      gpio_put(LED_PIN_G, 0);
      vTaskDelay(pdMS_TO_TICKS(delay));
    }
  }
}

void btn_2_task(void *p) {
  gpio_init(BTN_PIN);
  gpio_set_dir(BTN_PIN, GPIO_IN);
  gpio_pull_up(BTN_PIN);
  while (true) {
    if (!gpio_get(BTN_PIN)) {
      while (!gpio_get(BTN_PIN)) {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
      xSemaphoreGive(xSemaphore_r);
    }
  }
} 

int main() {
  stdio_init_all();
  xSemaphore_r = xSemaphoreCreateBinary();
  printf("Start RTOS \n");

  xTaskCreate(led_1_task, "LED_Task 1", 256, NULL, 1, NULL);
  xTaskCreate(led_2_task, "LED_Task 2", 256, NULL, 1, NULL);
  xTaskCreate(btn_2_task, "BTN_Task 2", 256, NULL, 1, NULL);
  vTaskStartScheduler();

  while (true)
    ;
}
