#include "QrssPiG.h"

#include <chrono>
#include <stdexcept>
#include <string>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

void QrssPiG::listModules() {
	std::cout << "Available input modules:" << std::endl;
	for (const auto&m: QGInputDevice::listModules()) std::cout << "\t" << m << std::endl;

	std::cout << "Available upload modules:" << std::endl;
	for (const auto&m: QGUploader::listModules()) std::cout << "\t" << m << std::endl;
}

void QrssPiG::listDevices() {
	auto devices = QGInputDevice::listDevices();

  	std::cout << "Available input devices:" << std::endl;
	for (const auto& m: devices) {
		if (m.second.size()) for (const auto &d: m.second) std::cout << "\t" << m.first << ":\t" << d << std::endl;
		else std::cout << "\t" << m.first << ":\tNo device found" << std::endl;
	}
}

QrssPiG::QrssPiG(const std::string &configFile) {
	YAML::Node config = YAML::LoadFile(configFile);

std::cout << "Configuring input" << std::endl;
	if (config["input"] && config["input"].Type() != YAML::NodeType::Map) throw std::runtime_error("YAML: input must be a map");
	_inputDevice = QGInputDevice::CreateInputDevice(config["input"]);

	// Patch real input samplerate and basefreq back to config so image class will have the correct value
	// Same for remaining ppm
	if (!config["input"]) config["input"] = YAML::Load("{}");
	config["input"]["samplerate"] = _inputDevice->sampleRate();
	config["input"]["basefreq"] = _inputDevice->baseFreq();
	config["input"]["ppm"] = _inputDevice->ppm();

std::cout << "Configuring processing" << std::endl;
	if (config["processing"] && config["processing"].Type() != YAML::NodeType::Map) throw std::runtime_error("YAML: processing must be a map");
	_processor.reset(new QGProcessor(config));

	// Patch real processing samplerate back to config so image class will have the correct value
	if (!config["processing"]) config["processing"] = YAML::Load("{}");
	config["processing"]["samplerate"] = _processor->sampleRate();
	config["processing"]["chunksize"] = _processor->chunkSize();

std::cout << "Configuring output" << std::endl;
	if (config["output"]) {
		if (config["output"].Type() == YAML::NodeType::Map) _image.reset(new QGImage(config, 0));
		// else if (config["output"].Type() == YAML::NodeType::Sequence) for (unsigned int i = 0; i < config["output"].size(); i++) _image.reset(new QGImage((config, i));
		else throw std::runtime_error("YAML: output must be a map or a sequence");
	}

std::cout << "Configuring upload" << std::endl;
	if (config["upload"]) {
		if (config["upload"].IsMap()) _uploaders.push_back(QGUploader::CreateUploader(config["upload"]));
		else if (config["upload"].IsSequence()) for (YAML::const_iterator it = config["upload"].begin(); it != config["upload"].end(); it++) _uploaders.push_back(QGUploader::CreateUploader(*it));
		else throw std::runtime_error("YAML: upload must be a map or a sequence");
	}

std::cout << "Configured" << std::endl;
}

QrssPiG::~QrssPiG() {
}

void QrssPiG::run() {
	_inputDevice->setCb(std::bind(&QGProcessor::addIQ, _processor, _1), _processor->chunkSize());
	_processor->addCb(std::bind(&QGImage::addLine, _image, _1));
	for (auto&& uploader: _uploaders) _image->addCb(std::bind(&QGUploader::push, uploader, _1, _2, _3, _4, _5));

	std::cout << "Run" << std::endl;
	_inputDevice->run();
}

void QrssPiG::stop() {
	std::cout << "Stop" << std::endl;
	_inputDevice->stop();
}

class ProcessingTest {
public:
	void run(unsigned int fftSize) {
		_processed = 0;

		std::unique_ptr<QGProcessor> processor(new QGProcessor(YAML::Load("{input: {samplerate: 8000000}, processing: {samplerate: 8000000, chunksize: 32, fft: " + std::to_string(fftSize) + ", overlap: 0}}")));
		processor->addCb(std::bind(&ProcessingTest::cb, this, _1));

		std::vector<std::complex<float>> iq;
		iq.resize(32);

		std::chrono::milliseconds started = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

		for (unsigned int i = 0; i < 10 * fftSize; i+=32) {
			processor->addIQ(&iq[0]);
			_processed += 32;
		}

		std::chrono::milliseconds stopped = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

		std::chrono::milliseconds delta = stopped - started;
		if (delta.count() == 0) delta = std::chrono::milliseconds(1);
		std::cout << _processed << " samples processed in " << delta.count() << "ms: " << (_processed / (1000. * delta.count())) << "MS/s" << std::endl;
	};

	void cb(const std::complex<float>* fft) {
		(void)fft;
	};

private:
	unsigned int _processed;
};

void QrssPiG::testProcessing() {
	ProcessingTest t;

	for (unsigned int i = 8192; i <= 16 * 1024 * 1024; i *= 2) {
		std::cout << "Testing fft size " << i << std::endl;
		t.run(i);
	}
}
