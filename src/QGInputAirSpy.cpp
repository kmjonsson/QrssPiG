#include "QGInputAirSpy.h"

#include <iostream>
#include <stdexcept>

std::vector<std::string> QGInputAirSpy::listDevices() {
	std::vector<std::string> list;

	return list;
}

QGInputAirSpy::QGInputAirSpy(const YAML::Node &config): QGInputDevice(config) {
}

QGInputAirSpy::~QGInputAirSpy() {
}

void QGInputAirSpy::_startDevice() {
}

void QGInputAirSpy::_stopDevice() {
}
