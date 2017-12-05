#include "QGInputPulseAudio.h"

#include <iostream>
#include <stdexcept>

std::vector<std::string> QGInputPulseAudio::listDevices() {
	std::vector<std::string> list;

	return list;
}

QGInputPulseAudio::QGInputPulseAudio(const YAML::Node &config): QGInputDevice(config) {
}

QGInputPulseAudio::~QGInputPulseAudio() {
}

void QGInputPulseAudio::_startDevice() {
}

void QGInputPulseAudio::_stopDevice() {
}
