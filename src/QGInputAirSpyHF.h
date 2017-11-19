#pragma once

#include "QGInputDevice.h"

#include <libairspyhf/airspyhf.h>

class QGInputAirSpyHF: public QGInputDevice {
public:
	QGInputAirSpyHF(const YAML::Node &config);
	~QGInputAirSpyHF();

private:
	void _startDevice();
	void _stopDevice();
};
