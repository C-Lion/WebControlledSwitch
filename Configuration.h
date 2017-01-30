// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

//Wifi Configuration
static const char *SSID = "AlonIoT"; //The default configuration access point ssid
static const char *password = "12345678"; //The default configuration access point password - at least 8 chars
static const char *webSiteHeader = "Welcome to the web light switch server";
static const char *appKey = "appkey"; //your secret app key - should be long enough
static const char* azureIoTHubDeviceConnectionString = "HostName=[IoTHub].azure-devices.net;DeviceId=[MyLightSwitch];SharedAccessKey=k6UElnivbQgrFGD86LSTPNRmV36cpKMe1OOH5LMc/42k=";
static char* deviceId = "MyLightSwitch";

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

#endif

