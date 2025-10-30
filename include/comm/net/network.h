/**
 * @file network.h
 *
 */

#ifndef G_NETWORK_H
#define G_NETWORK_H

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
#define NM_SSID_MAX_LEN                 33  /* IEEE 802.11 */

#ifndef MAX_ENTRIES
#define MAX_ENTRIES                     32
#endif
#define WIFI_MAX_AP_CACHE               MAX_ENTRIES
/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    char ssid[NM_SSID_MAX_LEN];
    char bssid[18];
    uint32_t freq_mhz;
    uint32_t bitrate_mbps;
    uint32_t bandwidth_mhz;
    uint8_t strength;
    uint32_t wpa_flags;
    uint32_t rsn_flags;
    uint32_t mode;
} ap_info_t;

typedef struct {
    ap_info_t active_ap;
    ap_info_t cached_ap[WIFI_MAX_AP_CACHE];
    int8_t ap_count;
} wifi_info_t;

/**********************
 *      MACROS
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

#endif /* G_NETWORK_H */
