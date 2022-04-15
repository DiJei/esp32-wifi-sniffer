#include "wifi_sniffer_task.h"
#include "../utils/utils.h"

#define WIFI_CHANNEL_MIN               (1)
#define WIFI_CHANNEL_MAX               (13)
#define table_size  200


//Mac Table
device_data wifi_table[table_size];
device_data wifi_blacklist[5];

uint8_t global_channel = WIFI_CHANNEL_MIN;

void send_wifi_table() {
  send_table(wifi_table, "WIFI", table_size);
}

void clean_wifi_table() {
  clean_table(wifi_table, table_size);
}

void clean_wifi_blacklist() {
  clean_table(wifi_table, table_size);
}


//Wifi
static wifi_country_t wifi_country = {.cc="CN", .schan = 1, .nchan = 13}; //Most recent esp32 library struct

esp_err_t event_handler(void *ctx, system_event_t *event)
{
  return ESP_OK;
}

void wifi_sniffer_init(void)
{
  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_country(&wifi_country) ); /* set country for channel range [1, 13] */
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
  ESP_ERROR_CHECK( esp_wifi_start() );
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void wifi_sniffer_set_channel(uint8_t & channel)
{
  global_channel = channel;
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  channel += 1;
  if (channel > WIFI_CHANNEL_MAX) 
    channel = WIFI_CHANNEL_MIN;
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
  if (type != WIFI_PKT_MGMT)
    return;

  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  char mac_add[35];
  sprintf(mac_add,"%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],hdr->addr2[3],hdr->addr2[4],hdr->addr2[5]);

  String hour = get_hour();
  String date = get_date();

  uint8_t hits = 0;
  hits = is_in_table(wifi_table, mac_add, ppkt->rx_ctrl.rssi, hour, table_size);
  if(hits == 0) {
    table_add(wifi_table, mac_add, ppkt->rx_ctrl.rssi, date, hour, table_size);
  }  
}