#include "QGInputAirSpyHF.h"

#include <iostream>
#include <stdexcept>

#define MAX_DEVICE 10

std::string QGInputAirSpyHF::moduleInfo() {
	airspyhf_lib_version_t v;
	airspyhf_lib_version(&v);
	return std::string("Version ") + std::to_string(v.major_version) + "." + std::to_string(v.minor_version) + "." + std::to_string(v.revision);
}

std::vector<std::string> QGInputAirSpyHF::listDevices() {
	std::vector<std::string> list;

	uint64_t serials[MAX_DEVICE];
	int numDevices = airspyhf_list_devices(serials, MAX_DEVICE);

	for (int i = 0; i < numDevices; i++) list.push_back(std::string("Serial: ") + std::to_string(serials[i]));

	return list;
}

QGInputAirSpyHF::QGInputAirSpyHF(const YAML::Node &config): QGInputDevice(config), _device(nullptr) {
	uint64_t deviceSN = 0;

	if (config["devicesn"]) deviceSN = config["devicesn"].as<uint64_t>();

	if (deviceSN) {
		if (airspyhf_open_sn(&_device, deviceSN)) throw std::runtime_error(std::string("Error opening device with serial ") + std::to_string(deviceSN));
	} else {
		if (airspyhf_open(&_device)) throw std::runtime_error("Error opening device");
	}

	char version[255];
	airspyhf_version_string_read(_device, version, 255);

	airspyhf_read_partid_serialno_t partserial;
	airspyhf_board_partid_serialno_read(_device, &partserial);

	std::cout << "Device version: " << version << std::endl;
}

QGInputAirSpyHF::~QGInputAirSpyHF() {
	if (_device) airspyhf_close(_device);
}

void QGInputAirSpyHF::_startDevice() {
}

void QGInputAirSpyHF::_stopDevice() {
}
