#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

SemaphoreHandle_t xSemaphore_led_r;
SemaphoreHandle_t xSemaphore_led_g;

SemaphoreHandle_t xSemaphore_btn_r;
SemaphoreHandle_t xSemaphore_btn_g;

void btn_callback(uint gpio, uint32_t events){
  if(gpio == BTN_PIN_R){
    xSemaphoreGiveFromISR(xSemaphore_btn_r,0);
  } else if(gpio == BTN_PIN_G){
    xSemaphoreGiveFromISR(xSemaphore_btn_g,0);
  }
}

void btn_1_task(void *p){
  gpio_init(BTN_PIN_R);
  gpio_set_dir(BTN_PIN_R, GPIO_IN);
  gpio_pull_up(BTN_PIN_R);

  while (true){
    if(xSemaphoreTake(xSemaphore_btn_r, pdMS_TO_TICKS(100)) == pdTRUE){
      xSemaphoreGive(xSemaphore_led_r);
      vTaskDelay(pdMS_TO_TICKS(200));
    }
  }

}

void btn_2_task(void *p){
  gpio_init(BTN_PIN_G);
  gpio_set_dir(BTN_PIN_G, GPIO_IN);
  gpio_pull_up(BTN_PIN_G);
    
  while (true){
    if(xSemaphoreTake(xSemaphore_btn_g, pdMS_TO_TICKS(100)) == pdTRUE){
      xSemaphoreGive(xSemaphore_led_g);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }

}

void led_1_task(void *p){
  gpio_init(LED_PIN_R);
  gpio_set_dir(LED_PIN_R, GPIO_OUT);
  int delay = 250;

  while (true){
    if(xSemaphoreTake(xSemaphore_led_r, pdMS_TO_TICKS(100)) == pdTRUE){
        gpio_put(LED_PIN_R, 1);
        vTaskDelay(pdMS_TO_TICKS(delay));
        gpio_put(LED_PIN_R, 0);
        vTaskDelay(pdMS_TO_TICKS(delay));
    }
  }
  
}

void led_2_task(void *p){
  gpio_init(LED_PIN_G);
  gpio_set_dir(LED_PIN_G, GPIO_OUT);
  int delay = 250;

  while (true){
    if(xSemaphoreTake(xSemaphore_led_g, pdMS_TO_TICKS(100)) == pdTRUE){
      gpio_put(LED_PIN_G, 1);
      vTaskDelay(pdMS_TO_TICKS(delay));
      gpio_put(LED_PIN_G, 0);
      vTaskDelay(pdMS_TO_TICKS(delay));
    }
  }

}

int main(){
  stdio_init_all();
  xSemaphore_btn_r = xSemaphoreCreateBinary();
  xSemaphore_btn_g = xSemaphoreCreateBinary();

  xSemaphore_led_r = xSemaphoreCreateBinary();
  xSemaphore_led_g = xSemaphoreCreateBinary();

  gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
  gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

  xTaskCreate(led_1_task, "LED_Task 1", 256, NULL, 1, NULL);
  xTaskCreate(btn_1_task, "BTN_Task 1", 256, NULL, 1, NULL);
  xTaskCreate(led_2_task, "LED_Task 2", 256, NULL, 1, NULL);
  xTaskCreate(btn_2_task, "BTN_Task 2", 256, NULL, 1, NULL);
  
  vTaskStartScheduler();

  while (true)
    ;
}