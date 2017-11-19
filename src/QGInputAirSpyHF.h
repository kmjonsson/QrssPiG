#pragma once

#include "QGInputDevice.h"

#include <libairspyhf/airspyhf.h>

class QGInputAirSpyHF: public QGInputDevice {
public:
	static std::vector<std::string> listDevices();

	QGInputAirSpyHF(const YAML::Node &config);
	~QGInputAirSpyHF();

private:
	void _startDevice();
	void _stopDevice();
};
