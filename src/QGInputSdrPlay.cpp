#include "QGInputSdrPlay.h"

#include <iostream>
#include <stdexcept>

#define MAX_DEVICE 10

std::string QGInputSdrPlay::moduleInfo() {
	float apiVersion = 0;

	if (mir_sdr_Success != mir_sdr_ApiVersion(&apiVersion)) {
		throw std::runtime_error("Unable to retrieve api verison");
	}

	return "API Version " + std::to_string(apiVersion);
}

std::vector<std::string> QGInputSdrPlay::listDevices() {
	std::vector<std::string> list;
	unsigned int numDevices = 0;
	mir_sdr_DeviceT devices[MAX_DEVICE];

	if (mir_sdr_Success != mir_sdr_GetDevices(devices, &numDevices, MAX_DEVICE)) {
		throw std::runtime_error("Unable to retrieve device list");
	}

	for (unsigned int i = 0; i < numDevices; i++) {
		std::string devName = std::to_string(i) + ": " + devices[i].DevNm + " (serial: " + devices[i].SerNo + ")";
		if (devices[0].devAvail == 0) devName += " (in use)";
		list.push_back(devName);
	}

	return list;
}

QGInputSdrPlay::QGInputSdrPlay(const YAML::Node &config): QGInputDevice(config), _device(nullptr) {
	bool verbose = false;
	int deviceIndex = 0;
	unsigned int err;

	if (config["verbose"]) verbose = config["verbose"].as<bool>();
	if (config["deviceindex"]) deviceIndex = config["deviceindex"].as<int>();

	if (verbose) mir_sdr_DebugEnable(1);

	listDevices(); // Mandatory before setting device index

	if (mir_sdr_Success != (err = mir_sdr_SetDeviceIdx(deviceIndex))) {
		throw std::runtime_error(std::string("Unable to set device index: ") + std::to_string(err));
	}
}

QGInputSdrPlay::~QGInputSdrPlay() {
}

void QGInputSdrPlay::_startDevice() {
}

void QGInputSdrPlay::_stopDevice() {
}
