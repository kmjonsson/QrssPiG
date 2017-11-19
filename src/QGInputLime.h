#pragma once

#include "QGInputDevice.h"

#include <lime/LimeSuite.h>

class QGInputLime: public QGInputDevice {
public:
	QGInputLime(const YAML::Node &config);
	~QGInputLime();

private:
	void _startDevice();
	void _stopDevice();
};
