#include "QGInputAirSpy.h"

#include <iostream>
#include <stdexcept>

std::string QGInputAirSpy::moduleInfo() {
	airspy_lib_version_t v;
	airspy_lib_version(&v);
	return std::string("Version ") + std::to_string(v.major_version) + "." + std::to_string(v.minor_version) + "." + std::to_string(v.revision);
}

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
