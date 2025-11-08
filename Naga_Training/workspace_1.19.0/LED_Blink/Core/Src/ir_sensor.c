#include "ir_sensor.h"

void IR_Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clock
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PA5 as input for IR sensor
    GPIO_InitStruct.Pin = IR_SENSOR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;  // Use pull-up for stable logic
    HAL_GPIO_Init(IR_SENSOR_GPIO_PORT, &GPIO_InitStruct);
}

// Return 1 = Object Detected, 0 = No Object
uint8_t IR_Sensor_Read(void)
{
    GPIO_PinState state = HAL_GPIO_ReadPin(IR_SENSOR_GPIO_PORT, IR_SENSOR_PIN);

    if (state == GPIO_PIN_RESET)
        return 1;  // Object detected (active LOW)
    else
        return 0;  // No object
}
