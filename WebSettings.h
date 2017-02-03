#include <String>
#ifndef _WEBSETTINGSTEMPLATE_h
#define _WEBSETTINGSTEMPLATE_h

const char* WebSettingHtmlTemplate = {R"(
<html xmlns="http://www.w3.org/1999/xhtml"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/><title>Web Controlled Switch Setup</title></head><body><div><h1><a>Web Controlled Switch Setup</a></h1> <form id="set-access-point" method="post" action="setconfiguration"><div><p>These values will be saved in the device eeprom. To reset the device to factory setup, press the button for long period of time - 20 seconds by default.</p></div><ul> <li> <h3>Router Settings</h3> <p> Once you press submit the device will reboot and will try to connect to the chosen network. The green Led light will blink the IP Address. To Factory Reset the device, press the button for long period until both leds are blinking very fast. </p></li><li> <label>Choose WiFi Network:</label> <ul> <%=AccessPointList%> </ul> </li><li> <label>WiFi Password </label> <div> <input name="WiFiPassword" type="password" maxlength="64" value="<%=WiFiPassword%>"/> </div></li><li> <h3>Microsoft Azure IoT Hub Configuration</h3> <p> Enter the Azure IoT Hub device connection String. Read more information <a href="https://azure.microsoft.com/en-us/services/iot-hub/">here</a> and <a href="https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-sdk-c-intro/">here.</a> </p></li><li> <label>Azure IoT Hub Connection String: </label> <div> <input name="IoTHubConnectionString" type="text" size="160" maxlength="255" value="<%=IoTHubConnectionString%>"/> </div></li><li> <label>Device Id Name </label> <div> <input name="DeviceId" type="text" maxlength="64" value="<%=DeviceId%>"/> </div></li><li> <label>Use Azure IoT Hub or Web Server for issuing commands </label> <ul> <li> <input name="WebServerOrAzureIoTHub" type="radio" value="WebServer" <%=WebServer%>/> <label>ESP8266 Web Server</label> </li><li> <input name="WebServerOrAzureIoTHub" type="radio" value="AzureIoTHub" <%=AzureIoTHub%>/> <label>Azure IoT Hub</label> </li></ul> </li><li> <h3>Push Button settings</h3> </li><li> <label>Long press period </label> <div> <input name="PBLongPress" type="number" min="1000" value="<%=PBLongPress%>"/> </div></li><li> <label>Very long press period </label> <div> <input name="PBVeryLongPress" type="number" min="1500" value="<%=PBVeryLongPress%>"/> </div></li><li> <label>Push Button Behaviour </label> <ul> <li> <input name="PBBehaviour" type="radio" value="PBBehaviourToggle" <%=PBBehaviourToggle%>/> <label>Toggle (On/Off)</label> </li><li> <input name="PBBehaviour" type="radio" value="PBBehaviourPulse" <%=PBBehaviourPulse%>/> <label>Pulse (On and then Off)</label> </li></ul> </li><li> <label>Push button and Activate command "On" period </label> <div> <input name="PBPulseActivationPeriod" type="number" min="100" value="<%=PBPulseActivationPeriod%>"/> </div></li><li> <input type="submit" name="submit" value="Submit"/> </li></ul> </form> </div></body></html>
)"};
/* http://www.willpeavy.com/minifier/
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Web Controlled Switch Setup</title>
</head>
<body>
    <div>
        <h1><a>Web Controlled Switch Setup</a></h1>
        <form id="set-access-point" method="post" action="setconfiguration">
            <div>
                <p>These values will be saved in the device eeprom. To reset the device to factory setup, press the button for long period of time - 20 seconds by default.</p>
            </div>
            <ul>
                <li>
                    <h3>Router Settings</h3>
                    <p>
                        Once you press submit the device will reboot and will try to connect to the chosen network. The green Led light will blink the IP Address.
                        To Factory Reset the device, press the button for long period until both leds are blinking very fast.
                    </p>
                </li>
                <li>
                    <label>Choose WiFi Network:</label>
                    <ul>
                        <%=AccessPointList%>
                    </ul>
                </li>
                <li>
                    <label>WiFi Password </label>
                    <div>
                        <input name="WiFiPassword" type="password" maxlength="64" value="<%=WiFiPassword%>" />
                    </div>
                </li>
                <li>
                    <h3>Microsoft Azure IoT Hub Configuration</h3>
                    <p>
                        Enter the Azure IoT Hub device connection String.
                        Read more information <a href="https://azure.microsoft.com/en-us/services/iot-hub/">here</a> and <a href="https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-sdk-c-intro/">here.</a>
                    </p>
                </li>
                <li>
                    <label>Azure IoT Hub Connection String: </label>
                    <div>
                        <input name="IoTHubConnectionString" type="text" size="160" maxlength="255" value="<%=IoTHubConnectionString%>" />
                    </div>
                </li>
                <li>
                    <label>Device Id Name </label>
                    <div>
                        <input name="DeviceId" type="text" maxlength="64" value="<%=DeviceId%>" />
                    </div>
                </li>
                <li>
                    <label>Use Azure IoT Hub or Web Server for issuing commands </label>
                    <ul>
                        <li>
                            <input name="WebServerOrAzureIoTHub" type="radio" value="WebServer" <%=WebServer%> /> 
                            <label>ESP8266 Web Server</label>
                        </li>
                        <li>
                            <input name="WebServerOrAzureIoTHub" type="radio" value="AzureIoTHub" <%=AzureIoTHub%>/>
                            <label>Azure IoT Hub</label>
                        </li>
                    </ul>
                </li>
                <li>
                    <h3>Push Button settings</h3>
                </li>
                <li>
                    <label>Long press period </label>
                    <div>
                        <input name="PBLongPress" type="number" min="1000" value="<%=PBLongPress%>" />
                    </div>
                </li>
                <li>
                    <label>Very long press period </label>
                    <div>
                        <input name="PBVeryLongPress" type="number" min="1500" value="<%=PBVeryLongPress%>" />
                    </div>
                </li>
                <li>
                    <label>Push Button Behaviour </label>
                    <ul>
                        <li>
                            <input name="PBBehaviour" type="radio" value="PBBehaviourToggle" <%=PBBehaviourToggle%> />
                            <label>Toggle (On/Off)</label>
                        </li>
                        <li>
                            <input name="PBBehaviour" type="radio" value="PBBehaviourPulse" <%=PBBehaviourPulse%>/>
                            <label>Pulse (On and then Off)</label>
                        </li>
                    </ul>
                </li>
                <li>
                    <label>Push button and Activate command "On" period </label>
                    <div>
                        <input name="PBPulseActivationPeriod" type="number" min="100" value="<%=PBPulseActivationPeriod%>" />
                    </div>
                </li>
                <li>
                    <input type="submit" name="submit" value="Submit" />
                </li>
            </ul>
        </form>
    </div>
</body>
</html>
*/
#endif //_WEBSETTINGSTEMPLATE_h