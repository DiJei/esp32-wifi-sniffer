#include <Arduino.h>
#include "EEPROM.h"
#include "../RTClib/RTClib.h"


//structs

// General data struct
typedef struct {
  int8_t rsst;
  String mac_add;
  uint16_t hits;
  String date;
  String first_time;
  String last_time;
} device_data;

// Table
void clean_table(device_data table[], uint16_t table_size);
void table_add(device_data table[], String mac_add, int rsst, String date, String hour, uint16_t table_size);
uint16_t is_in_table(device_data table[], String mac_add, int rsst, String hour, uint16_t table_size);
int getTableSize(device_data table[], uint16_t table_size);
void send_table(device_data table[], String type, uint16_t table_size);
void delete_item(device_data table[], String mac_add, uint16_t table_size);

// RTC
String get_datetime();
void update_time();
String get_date();
String get_hour();
void init_rtc();
void set_rtc_time(int year, int month, int day, int hour, int min, int sec);

void SendComand(String msg);

boolean set_hardware_id(char *id);
String get_hardware_id();