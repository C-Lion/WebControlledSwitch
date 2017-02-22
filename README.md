# Web and Cloud Controlled Switch

A web controlled switch based on ESP8266 devices such as the NodeMCU and WeMos D1. 

## Development Environemnt
The code is written in modern C++ using Visual Studio with [VisualMicro](http://www.visualmicro.com) add-on.  

Before opening the solution make sure that you've installed both Esp8266 and Azure IoT SDK libraries. The best way is to use Arduino IDE (download from http://https://www.arduino.cc/en/Main/Software).  

*Note: use the Win32 application (Windows Installer) and not the "Windows App" version.*

##### Setting Esp8266 support
First add esp8266 boards to board manager:  
- Open Arduino IDE
- Open preferences (Files -> Preferences)
- Copy: http://arduino.esp8266.com/stable/package_esp8266com_index.json to "Addtional boards Manager"
- Press 'Ok'

Install esp8266 boards
- Open boards Manager (Tools -> Board:xyz -> Boards Manager)
- Navigate to esp8266 by esp8266 and click install

##### Add Azure IoT SDK libraries
The project needs the following libraries:
- AzureIoTHub
- AzureIoTProtocol_HTTP
- AzureIoTProtocol_MQTT
- AzureIoTUtility

All of those can be installed using Arduino Library Manager (Sketch -> Include Library -> Manage Libraries...)

After adding board support and installing the libraries above you can open the solution file using Micro Studio and compile.

## Running the code

Build the circuit by following the instructions [here](https://cdn.thingiverse.com/assets/81/ee/bb/0f/e0/Building_web_controlled_led_bar_light_for_the_Printrbot_Simple_Metal_updated.pdf)   

Download files for printing from [here](http://www.thingiverse.com/thing:1658690). 

Look at the [configuration.h]((http://prose.io/#alonf/WebControlledSwitch/edit/master/Configuration.h ) file, here you can set the default access point mode router credentials.

Once you flash the device, you should point your computer/phone/tablet to the device Access Point SSID, and go to settings: http://192.168.4.1/appkey/setup

Once you select the local Access Point and reset the device. Use a browser that is connected to the same network and browse to: http://<ip assigned to the device>/appkey 

to make the communicationm with the device secured, change the appkey in the configuration file to any secret path you like.

Enjoy,

Alon Fliess.