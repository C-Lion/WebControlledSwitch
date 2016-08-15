// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

//Wifi Configuration
static const char *SSID = "[your wifi access point ssid]";
static const char *password = "[your wifi access point password]";
static const char *webSiteHeader = "Welcome to the web light switch Server";
static const char *appKey = "[your secret app key]"; 

static const char *turnOnMenuEntry = "Turn On";
static const char *turnOffMenuEntry = "Turn Off";
static const char *pulseMenuEntry = "Activate";

//uncomment this line to change the behavior of the controller to act as a switch (pulse) instead of an On/Off button
//for example, in case you need to control a garage gate that use the same signal 
//(activate to open the door and activate again to close it)
//#define PULSE_COMMAND


//https://azure.microsoft.com/en-us/services/iot-hub/
//https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-sdk-c-intro/
//uncomment this line to use Azure IoT Hub in addition to the web server
//#define USEAZUREIOTHUB

#ifdef USEAZUREIOTHUB
//Create this using IoT Hub Device Explorer: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
static const char* azureIoTHubDeviceConnectionString = "HostName=[YourIoTHubName].azure-devices.net;DeviceId=[YourDeviceId];SharedAccessKey=[DeviceAccessKey]";
static char* deviceId = "[YourDeviceId]";
#endif

#endif

