#include <string>
#ifndef _WEBSETTINGSTEMPLATE_h
#define _WEBSETTINGSTEMPLATE_h

const std::string WebSettingHtmlTemplate = R"(
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Web Controlled Switch Setup</title>
</head>
<body>
    <div>
        <h1><a>Web Controlled Switch Setup</a></h1>
        <form id="set-access-point" method="post" action="setaccesspoint">
            <div>
                <h2>Web Controlled Switch Setup</h2>
                <p>These values will be saved in the device eeprom. To reset the device to factory setup, press the button for long period of time - 20 seconds by default.</p>
            </div>
            <ul>
                <li>
                    <h3>Router Settings</h3>
                    <p>
                        Set router network SSID and password. Once you press submit the device will reboot and will try to connect to the chosen network. When the green Led light is turned on, the device is connected to the WiFi using the DHCP IP address assigned by the router. The green Led light will blink the IP Address, digit by digit.
                        To Factory Reset the device, press the button for long period until both leds are blinking very fast. (the default period is 20 seconds).
                    </p>
                </li>
                <li>
                    <label>Choose WiFi Network:</label>
                    <ul>
                        <%=AccessPointList%>
                    </ul>
                    <p><small>Select the WiFi address that the device will try to connect to.</small></p>
                </li>
                <li>
                    <label>WiFi Password </label>
                    <div>
                        <input name="WiFiPassword" type="password" maxlength="64" value="<%=WiFiPassword%>" />
                    </div><p><small>Enter the WiFi password of the selected network.</small></p>
                </li>
                <li>
                    <h3>Microsoft Azure IoT Hub Configuration</h3>
                    <p>
                        Enter the Azure IoT Hub device connection string.
                        Read more information <a href="https://azure.microsoft.com/en-us/services/iot-hub/">here</a> and <a href="https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-sdk-c-intro/">here.</a>
                    </p>
                </li>
                <li>
                    <label>Azure IoT Hub Connection String: </label>
                    <div>
                        <input name="IoTHubConnectionString" type="text" size="160" maxlength="255" value="<%=IoTHubConnectionString%>" />
                    </div><p><small>Use Azure IoT Hub Explorer to register a device and generate the required connection string</small></p>
                </li>
                <li>
                    <label>Device Id Name </label>
                    <div>
                        <input name="DeviceId" type="text" maxlength="64" value="<%=DeviceId%>" />
                    </div><p><small>Enter the unique device ID</small></p>
                </li>
                <li>
                    <label>Use Azure IoT Hub or Web Server for issuing commands </label>
                    <ul>
                        <li>
                            <input name="WebServerOrAzureIoTHub" type="radio" value="WebServer" <%=WebServer%> /> 
                            <label>ESP8266 Web Server</label>
                        </li>
                        <li>
                            <input name="webserverorazureiothub" type="radio" value="AzureIoTHub" <%=AzureIoTHub%>/>
                            <label>Azure IoT Hub</label>
                        </li>
                    </ul>
                    <p><small>Choose the device communication mode. Once this mode is set, it will only receive commands using this mode.</small></p>
                </li>
                <li>
                    <h3>Push Button settings</h3>
                    <p>Set the push button behaviour</p>
                </li>
                <li>
                    <label>Long press period </label>
                    <div>
                        <input name="PBLongPress" type="number" min="1000" value="<%=PBLongPress%>" />
                    </div><p><small>Set the push button long time period in milliseconds. 5000 is a good value.</small></p>
                </li>
                <li>
                    <label>Very long press period </label>
                    <div>
                        <input name="PBVeryLongPress" type="number" min="1500" value="<%=PBVeryLongPress%>" />
                    </div><p><small>Set the push button very long time period in milliseconds. 20000 is a good value.</small></p>
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
                    <p><small>Set the Push Button behaviour. If Pulse is chozen, enter also the pulse period.</small></p>
                </li>
                <li>
                    <label>Push button and Activate command "On" period </label>
                    <div>
                        <input name="PBPulseActivationPeriod" type="number" min="100" value="<%=PBPulseActivationPeriod%>" />
                    </div><p><small>Set the "On" period for pulse mode. Activated when a short press on the push button occurs or when the Activate command is received. </small></p>
                </li>
                <li>
                    <input type="submit" name="submit" value="Submit" />
                </li>
            </ul>
        </form>
    </div>
</body>
< / html>)";
#endif //_WEBSETTINGSTEMPLATE_h