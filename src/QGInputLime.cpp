#include "QGInputLime.h"

#include <iostream>
#include <stdexcept>

std::vector<std::string> QGInputLime::listDevices() {
	std::vector<std::string> list;

	return list;
}

QGInputLime::QGInputLime(const YAML::Node &config): QGInputDevice(config) {
}

QGInputLime::~QGInputLime() {
}

void QGInputLime::_startDevice() {
}

void QGInputLime::_stopDevice() {
}
