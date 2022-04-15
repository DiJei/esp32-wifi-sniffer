#include <WiFi.h>
#include "EEPROM.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"

#include "utils/utils.h"
#include "wifi_sniffer/wifi_sniffer_task.h"
#include "bt_sniffer/bt_sniffer_task.h"
#include "ble_sniffer/ble_sniffer_task.h"

#define LED_GPIO_PIN                     2
#define WIFI_CHANNEL_SWITCH_INTERVAL  (25)
#define WIFI_CHANNEL_MIN               (1)

#define DT_MODULE_Pin 33
#define CK_MODULE_Pin 32
#define SETUP_MODE_Pin 34

uint8_t  channel = WIFI_CHANNEL_MIN;
bool await_clock_set = true;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
String hardware_id = "-1";

void reset_mac_table(void*z) {
  while(1) {
    delay(300000);
    Serial.println("----GLOBAL RESET---");
    clean_wifi_table();
    clean_bt_table();
    clean_ble_table();
  }
}

void read_serial(void*z) {
  String line,temp;
  int aux, aux_1;
  aux = 0;
  aux_1 = 0;
  int year,month,day,hour,min,sec;
  
  while (1) {
    aux = 0;
    aux_1 = 0;
    line = "";
    temp = "";
    char id[6];

    if (Serial.available() > 0) {
      line = Serial.readString();
    }
  
    aux = line.indexOf("update_time");
    if (aux >= 0) {
      // init var
      year = 0;
      month = 0;
      day = 0;
      hour = 0;
      min = 0;
      sec = 0;
      // get datetime data
      temp = line.substring(aux + 12);
    
      aux_1 = temp.indexOf(" ");
      year = temp.substring(0, aux_1).toInt();
      temp = temp.substring(aux_1 + 1);
    
      aux_1 = temp.indexOf(" ");
      month =  temp.substring(0, aux_1).toInt();
      temp  = temp.substring(aux_1 + 1);

      aux_1 = temp.indexOf(" ");
      day =  temp.substring(0, aux_1).toInt();
      temp  = temp.substring(aux_1 + 1);
  
      aux_1 = temp.indexOf(" ");
      hour =  temp.substring(0, aux_1).toInt();
      temp  = temp.substring(aux_1 + 1);
  
      aux_1 = temp.indexOf(" ");
      min =  temp.substring(0, aux_1).toInt();
      temp  = temp.substring(aux_1 + 1);

      sec =  temp.toInt();
      
      // Set RTC time
      set_rtc_time(year, month, day, hour, min, sec) ;
      Serial.println("rtc setting received");
      await_clock_set = false;
  }

  aux = line.indexOf("set_id");
  if (aux >= 0) {
    temp = line.substring(aux + 7);
    temp.toCharArray(id, 7);
    if (set_hardware_id(id)) {
      Serial.println("hardware id set");
    }
    else {
      Serial.println("ERROR");
    }
  }

  aux = line.indexOf("get_id");
  if (aux >= 0) {
    Serial.println(get_hardware_id());
  }

  aux = line.indexOf("get_time");
  if (aux >= 0) {
    update_time();
    delay(50);
    Serial.print(get_date());
    Serial.print(" ");
    Serial.println(get_hour());
  }
    
  delay(250);
 }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // Turn ON BLE and BT Modules
  pinMode(LED_GPIO_PIN, OUTPUT);
  pinMode(DT_MODULE_Pin, OUTPUT);
  pinMode(CK_MODULE_Pin, OUTPUT);
  pinMode(SETUP_MODE_Pin, INPUT);

  delay(100);
  digitalWrite(LED_GPIO_PIN, LOW);
  digitalWrite(DT_MODULE_Pin, HIGH);
  digitalWrite(CK_MODULE_Pin, HIGH);
  delay(500);
  digitalWrite(LED_GPIO_PIN, HIGH);
  delay(500);
  
  Serial.begin(115200);

  // Clock
  init_rtc();
  update_time();

  // Serial Reader
  delay(100);
  xTaskCreatePinnedToCore(read_serial, "read_serial", 8192, NULL, 1, NULL, 0);
  delay(200);

  // Check debug mode
  if (digitalRead(SETUP_MODE_Pin) == LOW) {
    Serial.println("SETUP MODE");
    while(1) {
      SendComand("LOG TEST");
      delay(1000);
    }
  }

  Serial.println("START MAC SNIFFER");
  int try_outs = 0;
  while (await_clock_set) {
    delay(2500);
    try_outs = try_outs + 1;
    Serial.println("awating RTC clock set");
    if( try_outs == 12) {
      break;
    }
  }
  delay(500);
  SendComand("LOG TEST");

  // Start BT
  clean_bt_table();
  init_bt();
  delay(500);
  xTaskCreatePinnedToCore(listen_bt_serial, "read_bt", 8192, NULL, 1, NULL, 0);
  delay(100);
  xTaskCreatePinnedToCore(search_bt_devices, "search_bt", 8192, NULL, 1, NULL, 0);
  delay(100);

  // Start BLE
  clean_ble_table();
  init_ble();
  delay(500);
  xTaskCreatePinnedToCore(listen_ble_serial, "read_ble", 8192, NULL, 1, NULL, 0);
  delay(100);
  xTaskCreatePinnedToCore(search_ble_devices, "search_ble", 8192, NULL, 1, NULL, 0);
  delay(100);

   // Start Wifi
  clean_wifi_table();
  clean_wifi_blacklist();
  wifi_sniffer_init();
  xTaskCreatePinnedToCore(reset_mac_table, "reset_thread", 8192, NULL, 1, NULL, 0);
  delay(500);
  
  // get hardware id
  hardware_id = get_hardware_id();
}


void loop() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= 10000) {
    Serial.print("id,");
    Serial.println(hardware_id);
    send_wifi_table();
    send_bt_table();
    send_ble_table();  
    previousMillis = currentMillis;
  }
  else if (currentMillis - previousMillis >= 5000) {
    update_time();
  }
  vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
  wifi_sniffer_set_channel(channel);
}