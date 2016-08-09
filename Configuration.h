// Configuration.h

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

//Wifi Configuration
//static const char *SSID = "[your wifi access point ssid]";
//static const char *password = "[your wifi access point password]";
//static const char *webSiteHeader = "Welcome to the web light switch Server";
//static const char *appKey = "[your secret app key]"; 

static const char *turnOnMenuEntry = "Turn On";
static const char *turnOffMenuEntry = "Turn Off";
static const char *pulseMenuEntry = "Activate";

//uncomment this line to change the behavior of the controller to act as a switch (pulse) instead of an On/Off button
//for example, in case you need to control a garage gate that use the same signal 
//(activate to open the door and activate again to close it)
#define PULSE_COMMAND


#endif

