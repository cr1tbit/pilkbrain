#pragma onece

#include <Arduino.h>

#include "pinDefs.h"
// #include "SD_MMC.h"

void initPins(){
    pinMode(EN_PERIPH, OUTPUT);
    digitalWrite(EN_PERIPH, LOW);

    pinMode(EN_3V3OUT, OUTPUT);
    digitalWrite(EN_3V3OUT, LOW);

    pinMode(LED_STAT, OUTPUT);

    analogReadResolution(12);
    adcAttachPin(VBAT_MEAS);
}