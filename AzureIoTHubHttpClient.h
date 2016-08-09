// AzureIoTHubHttpClient.h

#ifndef _AZUREIOTHUBHTTPCLIENT_h
#define _AZUREIOTHUBHTTPCLIENT_h

#include "arduino.h"
bool InitAzureIoTHubClient(void);
bool AzureIoTHubClientLoop(int deviceStatus);
#endif

