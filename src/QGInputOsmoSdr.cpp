#include "QGInputOsmoSdr.h"

#include <iostream>
#include <stdexcept>

std::vector<std::string> QGInputOsmoSdr::listDevices() {
	std::vector<std::string> list;

	return list;
}

QGInputOsmoSdr::QGInputOsmoSdr(const YAML::Node &config): QGInputDevice(config) {
}

QGInputOsmoSdr::~QGInputOsmoSdr() {
}

void QGInputOsmoSdr::_startDevice() {
}

void QGInputOsmoSdr::_stopDevice() {
}
