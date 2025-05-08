#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

#include "FreeRTOS.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2S_DOUT      9  // connect to DAC pin DIN
#define I2S_BCLK      6  // connect to DAC pin BCK
#define I2S_LRC       11  // connect to DAC pin LCK

#define PIN_ENABLE  12

Audio audio;
Adafruit_SSD1306 display;

// const char* ssid =     "eduram";
// const char* password = "zarazcipodam";

const char* ssid =     "UPCEF2134F";
const char* password = "5rrhKnxKcdrv";


void loop2(void *pvParameters) {
    pinMode(PIN_ENABLE, OUTPUT);
    digitalWrite(PIN_ENABLE, HIGH);

    pinMode(0, INPUT);


    Serial.begin(115200);
    // WiFi.begin(ssid, password);
    // while (WiFi.status() != WL_CONNECTED) delay(1500);

    SPIFFS.begin();


    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(21);
//  audio.connecttohost("https://github.com/schreibfaul1/ESP32-audioI2S/raw/master/additional_info/Testfiles/Olsen-Banden.mp3");
    //  audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3d"); // 64 kbp/s aac+
//  audio.connecttospeech("Wenn die Hunde schlafen, kann der Wolf gut Schafe stehlen.", "de");
    audio.connecttoFS(SPIFFS, "/miau.mp3"); // SPIFFS
    while (true) {
        audio.loop();
        vTaskDelay(1);
    }
}

void setup() {
    xTaskCreatePinnedToCore(loop2, "loop", 100000, NULL, 1, NULL, 1);
}


void loop() {
    vTaskDelay(50);
    if(digitalRead(0) == LOW){
        ESP.restart();
    }
}

void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}