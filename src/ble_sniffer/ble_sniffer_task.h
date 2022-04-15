#include <Arduino.h>

//BLE
void search_ble_devices(void*z);
void listen_ble_serial(void*z);
void init_ble();

//Table
void clean_ble_table();
void send_ble_table();

//util
String read_line();