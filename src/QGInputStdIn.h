#pragma once

#include "QGInputDevice.h"

#include <memory>
#include <thread>

class QGInputStdIn: public QGInputDevice {
public:
	QGInputStdIn(const YAML::Node &config);
	~QGInputStdIn();

private:
	void _startDevice();
	void _stopDevice();

	void _process();

	enum class Format { U8IQ, S8IQ, U16IQ, S16IQ, S16MONO, S16LEFT, S16RIGHT };

	Format _format;

	std::thread _t;
	int _readBufferSize;
	std::unique_ptr<char[]> _readBuffer;
};
