// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

//Wifi Configuration
static const char *SSID = "AlonIoT"; //The default configuration access point ssid
static const char *password = "12345678"; //The default configuration access point password - at least 8 chars
static const char *webSiteHeader = "Welcome to the web light switch server";
static const char *appKey = "appkey"; //your secret app key - should be long enough
//static const char* azureIoTHubDeviceConnectionString = "HostName=[IoTHub].azure-devices.net;DeviceId=[MyLightSwitch];SharedAccessKey=k6UElnivbQgrFGD86LSTPNRmV36cpKMe1OOH5LMc/42k=";
static char* deviceId = "MyLightSwitch";

static const char* azureIoTHubDeviceConnectionString = "HostName=BlinkyHub.azure-devices.net;DeviceId=MyLightSwitch;SharedAccessKey=B333wsfq0fh3/w2gA98LPMTLPvVxwmyikM3/1ah1JTo=";
static const char *turnOnMenuEntry = "Turn On";
static const char *turnOffMenuEntry = "Turn Off";
static const char *pulseMenuEntry = "Activate";

const unsigned int defaultButtonLongTimePeriod = 5000; //5 seconds -> reset
const unsigned int defaultButtonVeryLongTimePeriod = 20000; //20 seconds -> factory reset
const unsigned int defaultPulseActivationTimePeriod = 2000; 

//Board configuration
const int pushButton = 5; //D1
const int redLed = 4; //D2
const int greenLed = 15; //D8
const int relay = 2; //D4
const int ButtonPressed = HIGH;
const int ButtonReleased = ButtonPressed == HIGH ? LOW : HIGH;

//Wifi Witty ESP-12F Board - https://blog.the-jedi.co.uk/2016/01/02/wifi-witty-esp12f-board/
//const int LDR = A0;
//const int pushButton = 4;
//const int redLed = 15;
//const int greenLed = 12;
//const int relay = 13; //BLUE led
//const int ButtonPressed = LOW;
//const int ButtonReleased = ButtonPressed == HIGH ? LOW : HIGH;


//uncomment this line to change the default behavior of the controller to act as a switch (pulse) instead of an On/Off button
//for example, in case you need to control a garage gate that use the same signal 
//(activate to open the door and activate again to close it)
#define PULSE_COMMAND


//https://azure.microsoft.com/en-us/services/iot-hub/
//https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-sdk-c-intro/
//uncomment this line to use Azure IoT Hub in addition to the web server
//#define USEAZUREIOTHUB

#endif

