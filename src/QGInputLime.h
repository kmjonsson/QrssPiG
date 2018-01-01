#pragma once

#include "QGInputDevice.h"

#include <lime/LimeSuite.h>

class QGInputLime: public QGInputDevice {
public:
	static std::string moduleInfo();
	static std::vector<std::string> listDevices();

	QGInputLime(const YAML::Node &config);
	~QGInputLime();

private:
	void _startDevice();
	void _stopDevice();
};
