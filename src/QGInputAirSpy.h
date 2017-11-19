#pragma once

#include "QGInputDevice.h"

#include <libairspy/airspy.h>

class QGInputAirSpy: public QGInputDevice {
public:
	QGInputAirSpy(const YAML::Node &config);
	~QGInputAirSpy();

private:
	void _startDevice();
	void _stopDevice();
};
