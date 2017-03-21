// WaterSensorManager.h

#ifndef _WATERMANAGER_h
#define _WATERMANAGER_h
#include "arduino.h"
#include <functional>
#include "Singleton.h"
#include "ArduinoLoopManager.h"
#include <memory>

class WaterSensorManager : public Singleton<WaterSensorManager>, public IProcessor
{
	friend class Singleton<WaterSensorManager>;
private:
	const int _sensorPin;
	const int _waterIndicatorPin;
	int _lastReportTime = 0;
	const int _stateReportInterval;
	std::function<void(bool state)> _stateSubscriber;
	unsigned long _lastUpdateIndicatorTime = 0;
	WaterSensorManager(int sensorPin, int _waterIndicatorPint, int stateReportInterval, std::function<void(bool state)> stateSubscriber);

 public:
	 virtual ~WaterSensorManager() {}
	 bool State() const
	 {
		 return analogRead(_sensorPin) > 200;
	 }
	void Loop() override;
};

typedef std::shared_ptr<WaterSensorManager> WaterSensorManagerPtr_t;
#endif

