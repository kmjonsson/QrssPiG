#pragma once

#include "QGInputDevice.h"

#include <mirsdrapi-rsp.h>

class QGInputSdrPlay: public QGInputDevice {
public:
	static std::string moduleInfo();
	static std::vector<std::string> listDevices();

	QGInputSdrPlay(const YAML::Node &config);
	~QGInputSdrPlay();

private:
	void _startDevice();
	void _stopDevice();

	struct airspyhf_device *_device;
};
