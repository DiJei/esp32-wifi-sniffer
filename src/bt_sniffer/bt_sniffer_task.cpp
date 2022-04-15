#include "bt_sniffer_task.h"
#include "../utils/utils.h"

#define BT_ADDRESS_table_size 150
HardwareSerial BTSerial(2);
bool IS_BT_CONNECTED = false;

//Mac Table
device_data bt_mac_table[BT_ADDRESS_table_size];
boolean bt_running = true;

void send_bt_table() {
  if (IS_BT_CONNECTED == true) {
    send_table(bt_mac_table, "BT", BT_ADDRESS_table_size);
  }
  else{
    Serial.println("BT,-1");
  }
}

void clean_bt_table() {
  clean_table(bt_mac_table, BT_ADDRESS_table_size);
}

void init_bt() {
    String temp = "";
    int8_t aux = -1;
    BTSerial.begin(38400);
    delay(1000);
    BTSerial.flush();
    delay(1000);
    BTSerial.write("AT\r\n");

    delay(2000);
    temp = BTSerial.readString();
    delay(500);
    aux = temp.indexOf("OK");
    if (aux < 0) {
        IS_BT_CONNECTED = false;
    }
    else {
        IS_BT_CONNECTED = true;
        BTSerial.write("AT+RESET\r\n");
        delay(500);
        BTSerial.write("AT+ROLE=1\r\n");
        delay(120);
        BTSerial.write("AT+CLASS=0\r\n");
        delay(120);
        BTSerial.write("AT+INIT\r\n");
    }
    BTSerial.flush();
    delay(500);
}

void search_bt_devices(void*z) {
    while(bt_running) {
        if (IS_BT_CONNECTED == true) {
          BTSerial.write("AT+IAC=9e8b33\r\n");
          delay(120);
          BTSerial.write("AT+INQM=1,9,4\r\n");
          delay(120);
          BTSerial.write("AT+INQ\r\n");
        }
        delay(5120);
    }
}

void listen_bt_serial(void*z) {
    String temp = "";
    String time = "";
    String hour = "";
    String date = "";

    while(bt_running) {
        while(BTSerial.available()) {
            temp = BTSerial.readString();
            if (temp.substring(0, 4) == "+INQ") {
                hour = get_hour();
                date = get_date();
              if (is_in_table(bt_mac_table, temp.substring(5, 19), 0 , hour, BT_ADDRESS_table_size) == 0)
                table_add(bt_mac_table, temp.substring(5, 19), 0, date, hour, BT_ADDRESS_table_size);
            }
        }
        delay(100);
    }  
}