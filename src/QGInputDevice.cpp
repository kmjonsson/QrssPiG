#include "QGInputDevice.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "Config.h"

#include "QGInputStdIn.h"

#ifdef HAVE_LIBAIRSPY
#include "QGInputAirSpy.h"
#endif // HAVE_LIBAIRSPY

#ifdef HAVE_LIBAIRSPYHF
#include "QGInputAirSpyHF.h"
#endif // HAVE_LIBAIRSPYHF

#ifdef HAVE_LIBALSA
#include "QGInputAlsa.h"
#endif // HAVE_LIBALSA

#ifdef HAVE_LIBGROSMOSDR
#include "QGInputGROsmoSdr.h"
#endif // HAVE_LIBGROSMOSDR

#ifdef HAVE_LIBHACKRF
#include "QGInputHackRF.h"
#endif // HAVE_LIBHACKRF

#ifdef HAVE_LIBLIMESUITE
#include "QGInputLime.h"
#endif // HAVE_LIBLIMESUITE

#ifdef HAVE_LIBRTLSDR
#include "QGInputRtlSdr.h"
#endif // HAVE_LIBRTLSDR

std::vector<std::pair<std::string, std::string>> QGInputDevice::listModules() {
    std::vector<std::pair<std::string, std::string>> modules;

    modules.push_back(std::make_pair("StdIn", ""));
#ifdef HAVE_LIBAIRSPY
    modules.push_back(std::make_pair("AirSpy", QGInputAirSpy::moduleInfo()));
#endif //HAVE_LIBAIRSPY
#ifdef HAVE_LIBAIRSPYHF
    modules.push_back(std::make_pair("AirSpyHF+", QGInputAirSpyHF::moduleInfo()));
#endif //HAVE_LIBAIRSPYHF
#ifdef HAVE_LIBALSA
    modules.push_back(std::make_pair("Alsa", ""));
#endif //HAVE_LIBALSA
#ifdef HAVE_LIBGROSMOSDR
    modules.push_back(std::make_pair("GROsmoSdr", ""));
#endif //HAVE_LIBGROSMOSDR
#ifdef HAVE_LIBHACKRF
    modules.push_back(std::make_pair("HackRF", QGInputHackRF::moduleInfo()));
#endif //HAVE_LIBHACKRF
#ifdef HAVE_LIBLIMESUITE
    modules.push_back(std::make_pair("LimeSdr", QGInputLime::moduleInfo()));
#endif //HAVE_LIBLIMESUITE
#ifdef HAVE_LIBRTLSDR
    modules.push_back(std::make_pair("RtlSdr", ""));
#endif //HAVE_LIBRTLSDR

    return modules;
}

std::vector<std::pair<std::string, std::vector<std::string>>> QGInputDevice::listDevices() {
    std::vector<std::pair<std::string, std::vector<std::string>>> devices;

#ifdef HAVE_LIBAIRSPY
    devices.push_back(std::make_pair("AirSpy", QGInputAirSpy::listDevices()));
#endif //HAVE_LIBAIRSPY
#ifdef HAVE_LIBAIRSPYHF
    devices.push_back(std::make_pair("AirSpyHF+", QGInputAirSpyHF::listDevices()));
#endif //HAVE_LIBAIRSPYHF
#ifdef HAVE_LIBALSA
    devices.push_back(std::make_pair("Alsa", QGInputAlsa::listDevices()));
#endif //HAVE_LIBALSA
#ifdef HAVE_LIBGROSMOSDR
    devices.push_back(std::make_pair("GROsmoSdr", QGInputGROsmoSdr::listDevices()));
#endif //HAVE_LIBGROSMOSDR
#ifdef HAVE_LIBHACKRF
    devices.push_back(std::make_pair("HackRF", QGInputHackRF::listDevices()));
#endif //HAVE_LIBHACKRF
#ifdef HAVE_LIBLIMESUITE
    devices.push_back(std::make_pair("LimeSdr", QGInputLime::listDevices()));
#endif //HAVE_LIBLIMESUITE
#ifdef HAVE_LIBRTLSDR
    devices.push_back(std::make_pair("RtlSdr", QGInputRtlSdr::listDevices()));
#endif //HAVE_LIBRTLSDR

    return devices;
}

QGInputDevice::QGInputDevice(const YAML::Node &config) {
    _sampleRate = 48000;
    _baseFreq = 0;
    _ppm = 0;
    _bufferlength = 1000;
    _debugBufferMonitor = false;

    if (config["samplerate"]) _sampleRate = config["samplerate"].as<unsigned int>();
    if (config["basefreq"]) _baseFreq = config["basefreq"].as<unsigned int>();
    if (config["ppm"]) _ppm = config["ppm"].as<int>();
    if (config["bufferlength"]) _bufferlength = config["bufferlength"].as<int>();

    if (config["debug"]) {
        if (config["debug"]["buffermonitor"]) _debugBufferMonitor = config["debug"]["buffermonitor"].as<bool>();
    }

    _running = false;
}

void QGInputDevice::setCb(std::function<void(const std::complex<float>*)>cb, unsigned int chunkSize) {
    _cb = cb;
    _chunkSize = chunkSize;

    // buffercapacity based bufferlength in milliseconds, rounded down to multiple of chunksize
    _bufferCapacity = _bufferlength / 1000. * _sampleRate;
    _bufferCapacity = (_bufferCapacity / _chunkSize) * _chunkSize;
    _buffer.resize(_bufferCapacity);
    _bufferSize = 0;
    _bufferHead = 0;
    _bufferTail = 0;
}

void QGInputDevice::run() {
	if (_running) throw std::runtime_error("Already running");

    _running = true;
    bool stopping = false;
    bool running;

    _startDevice();

    std::thread monitor;
    if (_debugBufferMonitor) monitor = std::thread(std::bind(&QGInputDevice::_bufferMonitor, this));

    do {
        running = _running;

        // Detect stop() and shutdown device only once
        if (!running && !stopping) {
            _stopDevice();
            stopping = true;
        }

        if (_bufferSize >= _chunkSize) {
            _cb(&_buffer[_bufferTail]);
            _bufferTail += _chunkSize;
            _bufferTail %= _bufferCapacity;

            // Update size
            _bufferSize -= _chunkSize;
        } else {
	           std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    } while (running || (_bufferSize >= _chunkSize));

    if (_debugBufferMonitor) monitor.join();
}

void QGInputDevice::stop() {
    // Called from signal handler, only do signal handler safe stuff here !
    _running = false;
}

std::unique_ptr<QGInputDevice> QGInputDevice::CreateInputDevice(const YAML::Node &config) {
    if (!config["type"] || (config["type"].as<std::string>().compare("stdin") == 0)) {
        std::cout << "Input type stdin" << std::endl;
        return std::unique_ptr<QGInputDevice>(new QGInputStdIn(config));
    } else if (config["type"].as<std::string>().compare("airspy") == 0) {
#ifdef HAVE_LIBAIRSPY
        std::cout << "Input type airspy" << std::endl;
        return std::unique_ptr<QGInputDevice>(new QGInputAirSpy(config));
#else
        throw std::runtime_error(std::string("QGInputDevice: airspy support not builtin into this build"));
#endif //HAVE_LIBAIRSPY
    } else if (config["type"].as<std::string>().compare("airspyhf") == 0) {
#ifdef HAVE_LIBAIRSPYHF
        std::cout << "Input type airspyhf" << std::endl;
        return std::unique_ptr<QGInputDevice>(new QGInputAirSpyHF(config));
#else
        throw std::runtime_error(std::string("QGInputDevice: airspyhf support not builtin into this build"));
#endif //HAVE_LIBAIRSPYHF
    } else if (config["type"].as<std::string>().compare("alsa") == 0) {
#ifdef HAVE_LIBALSA
        std::cout << "Input type alsa" << std::endl;
        return std::unique_ptr<QGInputDevice>(new QGInputAlsa(config));
#else
        throw std::runtime_error(std::string("QGInputDevice: alsa support not builtin into this build"));
#endif //HAVE_LIBALSA
    } else if (config["type"].as<std::string>().compare("hackrf") == 0) {
#ifdef HAVE_LIBHACKRF
        std::cout << "Input type hackrf" << std::endl;
        return std::unique_ptr<QGInputDevice>(new QGInputHackRF(config));
#else
        throw std::runtime_error(std::string("QGInputDevice: hackrf support not builtin into this build"));
#endif //HAVE_LIBHACKRF
    } else if (config["type"].as<std::string>().compare("limesdr") == 0) {
#ifdef HAVE_LIBLIMESUITE
        std::cout << "Input type limesdr" << std::endl;
        return std::unique_ptr<QGInputDevice>(new QGInputLime(config));
#else
        throw std::runtime_error(std::string("QGInputDevice: limesdr support not builtin into this build"));
#endif //HAVE_LIBLIMESUITE
    } else if (config["type"].as<std::string>().compare("rtlsdr") == 0) {
#ifdef HAVE_LIBRTLSDR
        std::cout << "Input type rtlsdr" << std::endl;
        return std::unique_ptr<QGInputDevice>(new QGInputRtlSdr(config));
#else
        throw std::runtime_error(std::string("QGInputDevice: rtlsdr support not builtin into this build"));
#endif //HAVE_LIBRTLSDR
    } else {
        throw std::runtime_error(std::string("QGInputDevice: unknown type ") + config["type"].as<std::string>());
    }
}

void QGInputDevice::_bufferMonitor() {
    while (_running || _bufferSize >= _chunkSize) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << (_running ? "[Running]" : "[Stopping]") << "\tBuffer: " << _bufferSize << " / " << _bufferCapacity << " (" << (100.*_bufferSize/_bufferCapacity) << "%)" << std::endl;
    }
    return;
}
