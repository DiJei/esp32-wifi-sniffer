#include <WiFi.h>
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"

//structs
typedef struct {
  unsigned frame_ctrl:16;
  unsigned duration_id:16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl:16;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;


typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

// Prototypes
esp_err_t event_handler(void *ctx, system_event_t *event);
void wifi_sniffer_init(void);
void wifi_sniffer_set_channel(uint8_t & channel);
const char * wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type);
void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type);

//Table
void send_wifi_table();
void clean_wifi_table();
void clean_wifi_blacklist();