#include "QGInputLime.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

std::string QGInputLime::moduleInfo() {
	return std::string("Version ") + LMS_GetLibraryVersion();
}

std::vector<std::string> QGInputLime::listDevices() {
	std::vector<std::string> list;

	lms_info_str_t *deviceList = nullptr;
	int numDevices = LMS_GetDeviceList(deviceList);
	for (int i = 0; i < numDevices; i++) list.push_back(deviceList[i]);

	return list;
}

QGInputLime::QGInputLime(const YAML::Node &config): QGInputDevice(config), _device(nullptr) {
	std::string deviceName;

	if (config["device"]) deviceName = config["device"].as<std::string>();

	if (deviceName.size()) {
		lms_info_str_t d;
		strncpy(d, deviceName.c_str(), sizeof(d));
		if (LMS_Open(&_device, d, nullptr)) throw std::runtime_error(std::string("Error opening device ") + deviceName);
	} else {
		if (LMS_Open(&_device, nullptr, nullptr)) throw std::runtime_error("Error opening default device");
	}

	if (LMS_Init(_device)) {
		std::string e(LMS_GetLastErrorMessage());
		if (_device) LMS_Close(_device);
		throw std::runtime_error(std::string("Init failed: ") + e);
	}

	std::cout << "Device: " << LMS_GetDeviceInfo(_device) << std::endl;

	int numRXChannels = LMS_GetNumChannels(_device, LMS_CH_RX);
	std::cout << numRXChannels << " RX channels" << std::endl;

	int numTXChannels = LMS_GetNumChannels(_device, LMS_CH_TX);
	std::cout << numTXChannels << " TX channels" << std::endl;
}

QGInputLime::~QGInputLime() {
	if (_device) LMS_Close(_device);
}

void QGInputLime::_startDevice() {
}

void QGInputLime::_stopDevice() {
}
