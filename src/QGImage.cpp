#include "QGImage.h"

#include <math.h>

QGImage::QGImage(int size, int sampleRate, int N): _size(size), _sampleRate(sampleRate), N(N) {
	_im = gdImageCreateTrueColor(100 + N, 10 + _size + 100);

	_imBuffer = nullptr;
	_imBufferSize = 0;

	_cd = 256;
	_c = new int[_cd];

	_dBmin = -20.;
	_dBmax = -10.;
	_dBdelta = _dBmax - _dBmin;

	// Allocate colormap, taken from 'qrx' colormap from RFAnalyzer, reduced to 256 palette due to use of libgd
	int ii = 0;
	for (int i = 0; i <= 255; i += 4) _c[ii++] = gdImageColorAllocate(_im, 0, 0, i);
	for (int i = 0; i <= 255; i += 4) _c[ii++] = gdImageColorAllocate(_im, 0, i, 255);
	for (int i = 0; i <= 255; i += 4) _c[ii++] = gdImageColorAllocate(_im, i, 255, 255 - i);
	for (int i = 0; i <= 255; i += 4) _c[ii++] = gdImageColorAllocate(_im, 255, 255 -  i, 0);

	//int bucket = (_sampleRate/100) / (_sampleRate/N);
	int bucket = N/(2*100);
	int white = gdTrueColor(255, 255, 255);

	for (int i = 0; i < N/2; i += bucket) {
		gdImageLine(_im, 100 + N/2 - i*bucket, 0, 100 + N/2 - i*bucket, 10, white);
		gdImageLine(_im, 100 + N/2 + i*bucket, 0, 100 + N/2 + i*bucket, 10, white);
	}

	for (int i = -100; i <= 0; i += 10) {
		gdImageLine(_im, 90, 10 + _size - i, 93, 10 + _size - i, white);
	}

	for (double i = -100.; i <= 0.; i++) {
		int c = db2Color(i);
		gdImageSetPixel(_im, 95, 10 + _size - i, c);
		gdImageSetPixel(_im, 96, 10 + _size - i, c);
		gdImageSetPixel(_im, 97, 10 + _size - i, c);
		gdImageSetPixel(_im, 98, 10 + _size - i, c);
	}
}

QGImage::~QGImage() {
	delete [] _c;
	if (_imBuffer) gdFree(_imBuffer);
	gdImageDestroy(_im);
}

void QGImage::drawLine(const std::complex<double> *fft, int lineNumber) {
	int whiteA = gdTrueColorAlpha(255, 255, 255, 125);

	for (int i = 1; i < N; i++) {
		gdImageLine(_im, 100 + (i + N/2) % N - 1, 10 + _size - 10 * log10(abs(fft[i - 1]) / N), 100 + (i + N/2) % N, 10 + _size - 10 * log10(abs(fft[i]) / N), whiteA);
	}

	for (int i = 2; i < N - 2; i++) {
		// Get normalized value with DC centered
		gdImageSetPixel(_im, 100 + (i + N/2) % N, 10 + lineNumber, db2Color(10 * log10(abs(fft[i]) / N)));
	}
}

void QGImage::save2Buffer() {
	if (_imBuffer) gdFree(_imBuffer);

	_imBuffer = (char *)gdImagePngPtr(_im, &_imBufferSize);
}

void QGImage::save(const std::string &fileName) {
	FILE *pngout;

	pngout = fopen(fileName.c_str(), "wb");
	gdImagePng(_im, pngout);
	fclose(pngout);
}

int QGImage::db2Color(double v) {
	if (v < _dBmin) v = _dBmin;
	if (v > _dBmax) v = _dBmax;

	return _c[(int)trunc((_cd - 1) * (v - _dBmin) / _dBdelta)];
}
