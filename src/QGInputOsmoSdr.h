#pragma once

#include "QGInputDevice.h"

#include <osmosdr.h>

class QGInputOsmoSdr: public QGInputDevice {
public:
	static std::vector<std::string> listDevices();

	QGInputOsmoSdr(const YAML::Node &config);
	~QGInputOsmoSdr();

private:
	void _startDevice();
	void _stopDevice();
};
