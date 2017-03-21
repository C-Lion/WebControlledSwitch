#include "WaterSensorManager.h"
using namespace std;

WaterSensorManager::WaterSensorManager(int sensorPin, int waterIndicatorPin, int stateReportInterval, std::function<void(bool state)> stateSubscriber) : _sensorPin(sensorPin), _waterIndicatorPin(waterIndicatorPin), _stateReportInterval(stateReportInterval), _stateSubscriber(stateSubscriber)
{
	pinMode(sensorPin, INPUT);
	pinMode(waterIndicatorPin, OUTPUT);
}

void WaterSensorManager::Loop() 
{
	if (millis()  - _lastUpdateIndicatorTime >= 1000) //every second
	{
		_lastUpdateIndicatorTime = millis(); //reset the time
		digitalWrite(_waterIndicatorPin, State() ? HIGH : LOW);
	}

	if (millis() - _lastReportTime >= _stateReportInterval)
	{
		Serial.println(analogRead(A0));
		_lastReportTime = millis(); //reset the time
		if (_stateSubscriber)
			_stateSubscriber(State());
	}
}

