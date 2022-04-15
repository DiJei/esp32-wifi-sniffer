#include "ble_sniffer_task.h"
#include "../utils/utils.h"

#define BLE_ADDRESS_table_size 200
HardwareSerial BLESerial(1);
bool IS_BLE_CONNECTED = false;

//Mac Table
device_data ble_mac_table[BLE_ADDRESS_table_size];

boolean ble_running = true;

void send_ble_table() {
  if (IS_BLE_CONNECTED == true) {
    send_table(ble_mac_table, "BLE", BLE_ADDRESS_table_size);
  }
  else{
    Serial.println("BLE,-1");
  }
}

void clean_ble_table() {
  clean_table(ble_mac_table, BLE_ADDRESS_table_size);
}


//util
String read_line() {
    String line = "";
    char temp;
    while(BLESerial.available()) {
       temp = BLESerial.read();
       if(temp == '\n') 
           break;
        line += temp;
    }
    return line;

}

//BLE
void init_ble() {
    String temp;
    int8_t aux = -1;
    // Start BLE Serial
    BLESerial.begin(9600, SERIAL_8N1, 22, 23);
    delay(1000);
    // TEST Conection
    BLESerial.flush();
    delay(1000);
    BLESerial.write("AT\r\n");
    delay(2000);
    temp = read_line();
    delay(500);
    aux = temp.indexOf("OK");
    if (aux < 0) {
        IS_BLE_CONNECTED = false;
    }
    else {
        IS_BLE_CONNECTED = true;
        BLESerial.write("AT+ROLE1\r\n");
        delay(120);
        BLESerial.write("AT+ADVI5\r\n");
        delay(120);
    }
    BLESerial.flush();
}

void search_ble_devices(void*z) {
    while(ble_running) {
        if (IS_BLE_CONNECTED == true) {
            BLESerial.write("AT+INQ\r\n");
        }
        delay(5120);
    }
}

void listen_ble_serial(void*z) {
    String temp = "";
    String term = "";
    String mac = "";
    int aux_a = -1;
    int aux_b= -1;

    String time = "";
    String hour = "";
    String date = "";

    while(ble_running) {
        while(BLESerial.available()) {
            temp = read_line();
            aux_b = temp.indexOf("+INQ:");
            if (aux_b >= 0) {
                aux_a = temp.indexOf(" ");
                if (aux_a > 0) {
                    term = temp.substring(aux_b + 5, aux_a);
                    if (term.toInt() > 0) {
                        mac = temp.substring(aux_a + 1, aux_a + 14);
                        hour = get_hour();
                        date = get_date();
                        if (is_in_table(ble_mac_table, mac, 0 , hour, BLE_ADDRESS_table_size) == 0)
                            table_add(ble_mac_table, mac, 0, date, hour, BLE_ADDRESS_table_size);
                    }
                } 
            }
        }
        delay(100);
    }  
}