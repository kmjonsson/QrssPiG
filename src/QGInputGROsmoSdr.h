#pragma once

#include "QGInputDevice.h"

#include <osmosdr/device.h>

class QGInputGROsmoSdr: public QGInputDevice {
public:
	static std::vector<std::string> listDevices();

	QGInputGROsmoSdr(const YAML::Node &config);
	~QGInputGROsmoSdr();

private:
	void _startDevice();
	void _stopDevice();
};
