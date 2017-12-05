#pragma once

#include "QGInputDevice.h"

class QGInputPulseAudio: public QGInputDevice {
public:
	static std::vector<std::string> listDevices();

	QGInputPulseAudio(const YAML::Node &config);
	~QGInputPulseAudio();

private:
	void _startDevice();
	void _stopDevice();

};
