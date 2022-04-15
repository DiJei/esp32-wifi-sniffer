#include "utils.h"
#define TEMP_BASE	10

#define DT_MODULE_Pin 33
#define CK_MODULE_Pin 32

#define EEPROM_SIZE 6

RTC_DS3231 rtc;
String date = "";
String hour = "";
// Table

void delete_item(device_data table[], String mac_add, uint16_t table_size) {
  
  // Find mac_add if there are in table
  int x = 0;
  for (;x < table_size; x++) {
    if (table[x].mac_add.equals(mac_add) || table[x].mac_add.equals(""))
      break;
  }
  // if not the last push up all itens
  if (x < table_size - 1) {
    table[x].mac_add = table[x + 1].mac_add;
    table[x].hits = table[x + 1].hits;
    table[x].rsst = table[x + 1].rsst;
    table[x].date = table[x + 1].date;
    table[x].first_time = table[x + 1].first_time;
    table[x].last_time = table[x + 1].last_time;


    x++;
    for (; x  < table_size - 1; x++) {
      if (table[x].mac_add.equals(""))
        break;
      table[x].mac_add = table[x + 1].mac_add;
      table[x].hits = table[x + 1].hits;
      table[x].rsst = table[x + 1].rsst;
      table[x].date = table[x + 1].date;
      table[x].first_time = table[x + 1].first_time;
      table[x].last_time = table[x + 1].last_time;
    }
    table[x].mac_add = "";
    table[x].date = "";
    table[x].first_time = "";
    table[x].last_time = ""; 
    table[x].hits = 0;
    table[x].rsst = 0;
  }
}

void clean_table(device_data table[], uint16_t table_size) {
  for (int x = 0; x < table_size; x++)  {
    table[x].mac_add = "";
    table[x].hits = 0;
    table[x].rsst = 0;
    table[x].date = "";
    table[x].first_time = "";
    table[x].last_time = "";
  }
}


uint16_t is_in_table(device_data table[], String mac_add, int rsst, String hour, uint16_t table_size) {
  uint8_t hits = 0;
  for (int x = 0; x < table_size; x++) {
    if ((table[x].mac_add).equals("")) 
      break;
    if( (table[x].mac_add).equals(mac_add)) {
      table[x].rsst = rsst;
      table[x].last_time = hour;
      if (table[x].hits < 255)
        table[x].hits += 1;
      hits = table[x].hits;
    }
  }
  return hits;
}


// VERIFICAR
void table_add(device_data table[], String mac_add, int rsst, String date, String hour, uint16_t table_size) {
  String temp_date = table[0].date;
  String act_date = "";

  String temp_first_hour = table[0].first_time;
  String act_first_hour = "";

  String temp_last_hour = table[0].last_time;
  String act_last_hour = "";

  String temp = table[0].mac_add;
  String actual = "";

  uint8_t tmp_hits = table[0].hits;
  uint8_t act_hits = 0;

  int tmp_rssi = table[0].rsst;
  int act_rssi = 0;

  table[0].date = date;
  table[0].first_time = hour;
  table[0].last_time = hour;
  table[0].mac_add = mac_add;
  table[0].rsst = rsst;
  table[0].hits = 1;
  
  for (int x = 1; x < table_size; x++) {
    act_date = table[x].date;
    table[x].date = temp_date;
    temp_date = act_date;
  
    act_first_hour = table[x].first_time;
    table[x].first_time = temp_first_hour;
    temp_first_hour = act_first_hour;

    act_last_hour = table[x].last_time;
    table[x].last_time = temp_last_hour;
    temp_last_hour = act_last_hour;

    actual = table[x].mac_add;
    table[x].mac_add = temp;
    temp = actual;

    act_hits = table[x].hits;
    table[x].hits = tmp_hits;
    tmp_hits = act_hits;

    act_rssi = table[x].rsst;
    table[x].rsst = tmp_rssi;
    tmp_rssi = act_rssi;

    if (temp.equals(""))
      break;
  }
}

int getTableSize(device_data table[], uint16_t table_size) {
  int size = 0;
  for(int x = 0; x < table_size; x++) {
    if ((table[x].mac_add).equals("")) 
      break;
    size += 1;
  }
  return size;
}

void send_table(device_data table[], String type, uint16_t table_size) {
  int size = getTableSize(table, table_size);
  Serial.println(type + "," + String(size));
  for(int x = 0; x < table_size; x++) {
    if ((table[x].mac_add).equals(""))
      break;
    Serial.println(table[x].mac_add + "," + String(table[x].rsst) + "," + String(table[x].hits) + "," + table[x].date + "," + table[x].first_time + "," + table[x].last_time);
  }
}

// RTC
String get_date() {
    return(date);
}

String get_hour() {
    return(hour);
}

String  get_datetime() {
    
    DateTime now = rtc.now();
    String datetime = "";
    
    datetime += String(now.hour(), DEC) + ":";
    datetime += String(now.minute(), DEC) + ":";
    datetime += String(now.second(), DEC) + " ";

    datetime += String(now.day(), DEC) + "/";
    datetime += String(now.month(), DEC) + "/";
    datetime += String(now.year(), DEC);

    return(datetime);
}


void init_rtc() {
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
  }
}

void set_rtc_time(int year, int month, int day, int hour, int min, int sec) {
  rtc.adjust(DateTime(year, month, day, hour, min, sec));
}

void update_time() {
    //rtc.begin();
    String time = "";
    int split = 0;
    time = get_datetime();
    split = time.indexOf(" ");
    hour = time.substring(0,split);
    date = time.substring(split + 1);
}


void SendComand(String msg)
{
	uint8_t x;
	uint8_t c;
	uint8_t command;
  uint8_t buf[255];
  
  msg.getBytes(buf, 255);
  
	digitalWrite( DT_MODULE_Pin, HIGH);
	digitalWrite( CK_MODULE_Pin, HIGH);
  
	for(c = 0; c < msg.length(); c++)
	{
		command = buf[c];
		for(x = 0;x < 8;x++)
		{
			if(command & 0x80)
				digitalWrite( DT_MODULE_Pin, HIGH);
			else
				digitalWrite( DT_MODULE_Pin, LOW);

			command = command << 1;

			vTaskDelay(TEMP_BASE);
			digitalWrite( CK_MODULE_Pin, LOW);
			vTaskDelay(TEMP_BASE);
			digitalWrite( CK_MODULE_Pin, HIGH);
		}

		digitalWrite( DT_MODULE_Pin, HIGH);
		digitalWrite( CK_MODULE_Pin, HIGH);
		vTaskDelay(TEMP_BASE/2);
	}
}


boolean set_hardware_id(char *id) {
    if (!EEPROM.begin(EEPROM_SIZE)) {
      return false;
    }

   for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, id[i]);
    }
    EEPROM.commit();
    return true;
}

String get_hardware_id() {
  String id = "";
  char id_raw[6];
  if (!EEPROM.begin(EEPROM_SIZE)) {
      return id;
  }
  for (int i = 0; i < EEPROM_SIZE; i++) {
    byte readValue = EEPROM.read(i);
    id_raw[i] = char(readValue);
  }
  id = String(id_raw);
  return id;
}