#include "QGInputAirSpyHF.h"

#include <iostream>
#include <stdexcept>

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
