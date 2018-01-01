#include "QGInputLime.h"

#include <iostream>
#include <stdexcept>

std::vector<std::string> QGInputLime::listDevices() {
	std::vector<std::string> list;

	lms_info_str_t *deviceList = nullptr;
	int numDevices = LMS_GetDeviceList(deviceList);
	for (int i = 0; i < numDevices; i++) list.push_back(deviceList[i]);

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
