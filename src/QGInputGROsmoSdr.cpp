#include "QGInputGROsmoSdr.h"

#include <iostream>
#include <stdexcept>

std::vector<std::string> QGInputGROsmoSdr::listDevices() {
	std::vector<std::string> list;

	osmosdr::devices_t devices = osmosdr::device::find();
	for (auto &device: devices) list.push_back(device.to_string());

	return list;
}

QGInputGROsmoSdr::QGInputGROsmoSdr(const YAML::Node &config): QGInputDevice(config) {
}

QGInputGROsmoSdr::~QGInputGROsmoSdr() {
}

void QGInputGROsmoSdr::_startDevice() {
}

void QGInputGROsmoSdr::_stopDevice() {
}
