#ifndef INC_IR_SENSOR_H_
#define INC_IR_SENSOR_H_

#include "main.h"

// Define which pin the IR sensor is connected to
#define IR_SENSOR_GPIO_PORT GPIOA
#define IR_SENSOR_PIN       GPIO_PIN_5

void IR_Sensor_Init(void);
uint8_t IR_Sensor_Read(void);

#endif /* INC_IR_SENSOR_H_ */
