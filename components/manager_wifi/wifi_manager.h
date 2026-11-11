#ifndef WIFI_MANAGER_H_   /* Include guard */
#define WIFI_MANAGER_H_ 

void wifi_init_sta(const char *WIFI_SSID, const char *WIFI_PASS);
void wifi_init_softap(void);
void wifi_stop(void);
void wifi_prepare(void);
char* ap_address(void);
char* wifi_address(void);

#endif
