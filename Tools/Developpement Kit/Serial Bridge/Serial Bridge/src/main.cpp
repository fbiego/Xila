// Bridge

#include "Arduino.h"
#include "HardwareSerial.h"

HardwareSerial Serial_1(1);

void Serial_Handler(void *pvParameters);
void Serial_1_Handler(void *pvParameters);

void setup()
{
    Serial.begin(921600);
    Serial_1.begin(921600, SERIAL_8N1, 16, 17);

    xTaskCreatePinnedToCore(Serial_Handler, "Serial Handler", 4 * 1024, NULL, 0, NULL, 0);

    xTaskCreatePinnedToCore(Serial_1_Handler, "Serial Handler", 4 * 1024, NULL, 0, NULL, 0);
}

void Serial_Handler(void *pvParameters)
{
    while (1)
    {
        if (Serial.available())
        {
            
            Serial_1.print(Serial.read(), HEX);
        }
    }
}

void Serial_1_Handler(void *pvParameters)
{
    while (1)
    {
        if (Serial_1.available())
        {
            Serial.print(Serial_1.read(), HEX);
        }
    }
}

void loop()
{
    vTaskDelete(NULL);
}