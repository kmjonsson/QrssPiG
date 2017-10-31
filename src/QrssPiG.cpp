#include "QrssPiG.h"

#include <stdexcept>
#include <string>

QrssPiG::QrssPiG() :
	_inputDevice(nullptr),
	_chunkSize(32),
	_input(nullptr),
	_resampled(nullptr),
	_hannW(nullptr),
	_fftIn(nullptr),
	_fftOut(nullptr),
	_fft(nullptr) {
		_N = 2048;
		_overlap = (3 * _N) / 4;
}

QrssPiG::QrssPiG(const std::string &format, int sampleRate, int N, const std::string &dir, const std::string &sshHost, const std::string &sshUser, int sshPort) : QrssPiG() {
	_N = N;

	_inputDevice = QGInputDevice::CreateInputDevice(YAML::Load("{format: " + format + ", samplerate: " + std::to_string(sampleRate) + ", basefreq: 0}"));

	if (sshHost.length()) {
		_uploaders.push_back(std::unique_ptr<QGUploader>(QGUploader::CreateUploader(YAML::Load("{type: scp, host: " + sshHost + ", port: " + std::to_string(sshPort) + ", user: " + sshUser + ", dir: " + dir + "}"))));
	} else {
		_uploaders.push_back(std::unique_ptr<QGUploader>(QGUploader::CreateUploader(YAML::Load("{type: local, dir: " + dir + "}"))));
	}

	_init();

	_im->configure(YAML::Load("")); // Start with default config
}

QrssPiG::QrssPiG(const std::string &configFile) : QrssPiG() {
	int iSampleRate = 48000;
	int pSampleRate = 6000;

	YAML::Node config = YAML::LoadFile(configFile);

	if (config["input"]) {
		if (config["input"].Type() != YAML::NodeType::Map) throw std::runtime_error("YAML: input must be a map");

		_inputDevice = QGInputDevice::CreateInputDevice(config["input"]);

		// Patch real samplerate back to config so image class will have the correct value
		config["input"]["samplerate"] = _inputDevice->sampleRate();
		config["input"]["basefreq"] = _inputDevice->baseFreq();
		config["input"]["ppm"] = _inputDevice->ppm();

		iSampleRate = config["input"]["samplerate"].as<int>();
	}

	if (config["processing"]) {
		if (config["processing"].Type() != YAML::NodeType::Map) throw std::runtime_error("YAML: processing must be a map");

		YAML::Node processing = config["processing"];

		if (processing["chunksize"]) _chunkSize = processing["chunksize"].as<int>();
		if (processing["samplerate"]) pSampleRate = processing["samplerate"].as<int>();
		if (processing["fft"]) _N = processing["fft"].as<int>();
		if (processing["fftoverlap"]) {
			int o = processing["fftoverlap"].as<int>();
			if ((o < 0) || (o >= _N)) throw std::runtime_error("YAML: overlap value out of range [0..N[");
			_overlap = (o * _N) / (o + 1);
		}
	}

	if (pSampleRate && (iSampleRate != pSampleRate)) {
		_resampler.reset(new QGDownSampler((float)iSampleRate/(float)pSampleRate, _chunkSize));

		// Patch config with real samplerate from resampler
		pSampleRate = iSampleRate / _resampler->getRealRate();
		if (config["processing"]) config["processing"]["samplerate"] = pSampleRate;

		_input = (std::complex<float>*)fftwf_malloc(sizeof(std::complex<float>) * _chunkSize);
		// Add provision to add a full resampled chunksize (+10% + 4 due to variable resampler output) when only one sample left before reaching N
		_resampled = (std::complex<float>*)fftwf_malloc(sizeof(std::complex<float>) * (_N + (1.1 * _chunkSize / pSampleRate + 4) - 1));
		_inputIndexThreshold = _chunkSize; // Trig when input size reaches chunksize to start resampling

	} else {
		// Add provision to add a full chunksize when only one sample left before reaching N
		_input = (std::complex<float>*)fftwf_malloc(sizeof(std::complex<float>) * (_N + _chunkSize - 1));
		_resampled = _input;
		_inputIndexThreshold = _N; // Trig when input size reaches N to start fft
	}
	_inputIndex = 0;
	_resampledIndex = 0;

	// Could be inlined, should be part of a processing init
	_init();

	_im.reset(new QGImage(_N, _overlap));
	if (config["output"]) {
		if (config["output"].Type() == YAML::NodeType::Map) _im->configure(config, 0);
		// else if (config["output"].Type() == YAML::NodeType::Sequence) for (unsigned int i = 0; i < config["output"].size(); i++) _im->configure(config, i);
		else throw std::runtime_error("YAML: output must be a map or a sequence");
	}

	if (config["upload"]) {
		if (config["upload"].IsMap()) _uploaders.push_back(std::move(QGUploader::CreateUploader(config["upload"])));
		else if (config["upload"].IsSequence()) for (YAML::const_iterator it = config["upload"].begin(); it != config["upload"].end(); it++) _uploaders.push_back(std::move(QGUploader::CreateUploader(*it)));
		else throw std::runtime_error("YAML: upload must be a map or a sequence");
	}
}

QrssPiG::~QrssPiG() {
	// Draw residual data if any
	try {
		_im->addLine(_fftOut);
	} catch (const std::exception &e) {};

	// Push and wait on dtor
	_pushImage(true);

	if (_fft) delete _fft;
	if (_input) fftwf_free(_input);
	if (_resampled && (_resampled != _input)) fftwf_free(_resampled); // When no resampling is used both pointer point to the same buffer
}

void QrssPiG::run() {
	using std::placeholders::_1;
	_inputDevice->run(std::bind(&QrssPiG::_addIQ, this, _1));
}

void QrssPiG::stop() {
	_inputDevice->stop();
}

//
// Private members
//
void QrssPiG::_init() {
	_hannW.reset(new float[_N]);
	for (int i = 0; i < _N/2; i++) {
		_hannW[i] = .5 * (1 - cos((2 * M_PI * i) / (_N / 2 - 1)));
	}

	_fft = new QGFft(_N);
	_fftIn = _fft->getInputBuffer();
	_fftOut = _fft->getFftBuffer();

	_frameIndex = 0;
}

void QrssPiG::_addIQ(std::complex<float> iq) {
	_input[_inputIndex++] = iq;

	if (_inputIndex >= _inputIndexThreshold) {
		if (_resampler) {
			// Adding 1 sample, we know _inputIndex == _inputIndexThreshold == _chunksSize, and not bigger
			_resampledIndex += _resampler->processChunk(_input, _resampled + _resampledIndex);
			_inputIndex = 0;
			if (_resampledIndex >= _N) {
				_computeFft();
				// TODO: Usually adds at most 1 sample, so we know _inputIndex == _inputIndexThreshold == _N, and not bigger
				for (auto i = 0; i < _overlap + _resampledIndex - _N; i++) _resampled[i] = _resampled[_N - _overlap + i];
				_resampledIndex = _overlap + _resampledIndex - _N;
			}
		} else {
			_computeFft();
			// Adding 1 sample, we know _inputIndex == _inputIndexThreshold == _N, and not bigger
			for (auto i = 0; i < _overlap; i++) _input[i] = _input[_N - _overlap + i];
			_inputIndex = _overlap;
		}
	}
}

void QrssPiG::_computeFft() {
	for (int i = 0; i < _N; i++) _fftIn[i] = _resampled[i] * _hannW[i / 2];
	_fft->process();

	switch(_im->addLine(_fftOut)) {
	case QGImage::Status::IntermediateReady:
		_pushIntermediateImage();
		break;

	case QGImage::Status::FrameReady:
		_pushImage();
		break;

	default:
		break;
	}
}

void QrssPiG::_pushIntermediateImage() {
	try {
		int frameSize;
		std::string frameName;
		char * frame;

		frame = _im->getFrame(frameSize, frameName);

std::cout << "pushing intermediate" << frameName << std::endl;
		for (auto& up: _uploaders) up->pushIntermediate(frameName, frame, frameSize);
	} catch (const std::exception &e) {
	}
}

void QrssPiG::_pushImage(bool wait) {
	try {
		int frameSize;
		std::string frameName;
		char * frame;

		frame = _im->getFrame(frameSize, frameName);

std::cout << "pushing " << frameName << std::endl;
		for (auto& up: _uploaders) up->push(frameName, frame, frameSize, wait);

		_im->startNewFrame();
	} catch (const std::exception &e) {
		std::cerr << "Error pushing file: " << e.what() << std::endl;
	}

	_frameIndex++;
}
