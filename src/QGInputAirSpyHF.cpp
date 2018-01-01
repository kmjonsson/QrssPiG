#include "QGInputAirSpyHF.h"

#include <iostream>
#include <stdexcept>

std::string QGInputAirSpyHF::moduleInfo() {
	airspyhf_lib_version_t v;
	airspyhf_lib_version(&v);
	return std::string("Version ") + std::to_string(v.major_version) + "." + std::to_string(v.minor_version) + "." + std::to_string(v.revision);
}

std::vector<std::string> QGInputAirSpyHF::listDevices() {
	std::vector<std::string> list;

	return list;
}

QGInputAirSpyHF::QGInputAirSpyHF(const YAML::Node &config): QGInputDevice(config) {
}

QGInputAirSpyHF::~QGInputAirSpyHF() {
}

void QGInputAirSpyHF::_startDevice() {
}

void QGInputAirSpyHF::_stopDevice() {
}
