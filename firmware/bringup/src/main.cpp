#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

#include "commonFwUtils.h"
#include "alfalog.h"
#include "advancedOledLogger.h"

#include "pinDefs.h"
#include "pilkbrainPeriphs.hpp"



#define I2S_DOUT      9  // connect to DAC pin DIN
#define I2S_BCLK      6  // connect to DAC pin BCK
#define I2S_LRC       11  // connect to DAC pin LCK

const int I2S_DAT = 9;
const int I2S_LRCLK = 11;
const int I2S_CLK = 6;

Audio audio;


// #include "FS.h"
// #include "SD_MMC.h"

SerialLogger serialLogger = SerialLogger(
    [](const char* str) {Serial.println(str);},
    LOG_DEBUG, ALOG_FANCY
);

// SerialLogger debugLogger = SerialLogger(
//     [](const char* str) {Serial0.println(str);},
//     LOG_DEBUG, ALOG_FANCY
// );



#include <U8g2lib.h>

U8G2_SSD1306_72X40_ER_1_HW_I2C u8g2(U8G2_R0, 37,I2C_SCL,I2C_SDA); 

void draw (){

  char buf [15];

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_4x6_mf);

    int anal_result = 69;
    snprintf(buf,15,"%d.%02dV",anal_result/1000,(anal_result/10)%100);

    u8g2.drawBox(0,0,72,8);
    u8g2.setColorIndex(0);
    u8g2.drawStr(50,7,buf);

    snprintf(buf,15,"%ddb",WiFi.RSSI());
    u8g2.drawStr(1,7,buf);

    u8g2.setColorIndex(1);

    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(0,17,"192.168.100.144");

    snprintf(buf,15,"Uptime %d",millis());
    u8g2.drawStr(0,27,buf);

    u8g2.drawStr(0,37,"Wezwanie do zaplaty");

    // u8g2.drawXBM(0,30,8,8,x_bits);
  } while ( u8g2.nextPage() );
  delay(100);
  

}

void heartbeatTask(void * parameter)
{
  int loop_count = 0;
   while(1){
    handle_io_pattern(LED_STAT, PATTERN_HBEAT);
    if(loop_count++ % 10 == 0){
      int voltage = analogReadMilliVolts(VBAT_MEAS);
      ALOGI("status: {}", voltage);
    }
    draw();
    vTaskDelay(100 / portTICK_PERIOD_MS);
   }
}

// void init_SD(){
//   if(!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)){
//     Serial.println("Pin change failed!");
//     return;
//   }

//   if (!SD_MMC.begin()) {
//     Serial.println("Card failed, or not present");
//     return;
//   }
//   uint8_t cardType = SD_MMC.cardType();
//   if(cardType == CARD_NONE){
//       Serial.println("No SD_MMC card attached");
//       return;
//   }

//   Serial.print("SD_MMC Card Type: ");
//   if(cardType == CARD_MMC){
//       Serial.println("MMC");
//   } else if(cardType == CARD_SD){
//       Serial.println("SDSC");
//   } else if(cardType == CARD_SDHC){
//       Serial.println("SDHC");
//   } else {
//       Serial.println("UNKNOWN");
//   }

//   uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
//   Serial.printf("SD_MMC Card Size: %lluMB\n\r", cardSize);

//   Serial.printf("Using: %llu/%lluMB\n\r",
//     SD_MMC.usedBytes() / (1024 * 1024),
//     SD_MMC.totalBytes() / (1024 * 1024)
//   );
// }


// AdvancedOledLogger aOledLogger = AdvancedOledLogger(Wire, LOG_INFO, OLED_64x48);


#define PIN_ENABLE  12

void music_loop(void *pvParameters) {
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) delay(1500);


  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_ENABLE, HIGH);

  SPIFFS.begin();

  Serial.print("DUPA");


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
  initPins();
  Serial.begin(115200);

  Serial.setTxTimeoutMs(0); // prevent logger slowdown when no usb connected

  pinMode(36, OUTPUT);
  digitalWrite(36, LOW);

  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);

  pinMode(37, OUTPUT);
  digitalWrite(37, HIGH);

  Serial.println(alogGetInitString());

  // Wire.begin(I2C_SDA, I2C_SCL, 400000);


  u8g2.begin();
  u8g2.setFlipMode(1);

  AlfaLogger.addBackend(&serialLogger);
  // AlfaLogger.addBackend(&aOledLogger);
  // AlfaLogger.addBackend(&debugLogger);

  AlfaLogger.begin();
  ALOGD("logger started");
  // ALOG_I2CLS(Wire);

  // init_SD();

  digitalWrite(EN_PERIPH, HIGH);

  xTaskCreatePinnedToCore(music_loop, "loop",
     100000, NULL, 1, NULL, 1);


  xTaskCreatePinnedToCore( heartbeatTask, "heartbeatTask",
      4096, NULL, 31, NULL , 1);
}


void loop() {
  if(Serial.available()){
    char c = Serial.read();
    if(c == 'r'){
      //reset esp32
      ESP.restart();
    }
  }
  //toggle the pin
  digitalWrite(LED_STAT, !digitalRead(LED_STAT));
  vTaskDelay(100 / portTICK_PERIOD_MS);
}