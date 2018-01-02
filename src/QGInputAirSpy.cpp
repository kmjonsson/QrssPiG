#include "QGInputAirSpy.h"

#include "airspy_list_devices.h"

#include <iostream>
#include <stdexcept>

std::string QGInputAirSpy::moduleInfo() {
	airspy_lib_version_t v;
	airspy_lib_version(&v);
	return std::string("Version ") + std::to_string(v.major_version) + "." + std::to_string(v.minor_version) + "." + std::to_string(v.revision);
}

std::vector<std::string> QGInputAirSpy::listDevices() {
	std::vector<std::string> list;

	int numDevices = airspy_list_devices(nullptr, 0);
	std::unique_ptr<uint64_t[]> serials(new uint64_t[numDevices]);
	airspy_list_devices(serials.get(), numDevices);

	for (int i = 0; i < numDevices; i++) list.push_back(std::string("Serial: ") + std::to_string(serials[i]));

	return list;
}

QGInputAirSpy::QGInputAirSpy(const YAML::Node &config): QGInputDevice(config), _device(nullptr) {
	uint64_t deviceSN = 0;

	if (config["devicesn"]) deviceSN = config["devicesn"].as<uint64_t>();

	if (deviceSN) {
		if (airspy_open_sn(&_device, deviceSN)) throw std::runtime_error(std::string("Error opening device with serial ") + std::to_string(deviceSN));
	} else {
		if (airspy_open(&_device)) throw std::runtime_error("Error opening device");
	}

	char version[255];
	airspy_version_string_read(_device, version, 255);

	airspy_read_partid_serialno_t partserial;
	airspy_board_partid_serialno_read(_device, &partserial);

	std::cout << "Device version: " << version << std::endl;
}

QGInputAirSpy::~QGInputAirSpy() {
	if (_device) airspy_close(_device);
}

void QGInputAirSpy::_startDevice() {
}

void QGInputAirSpy::_stopDevice() {
}
