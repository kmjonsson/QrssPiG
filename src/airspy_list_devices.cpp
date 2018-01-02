/*
	This file represent an enhancement proposed to libairspy:
		Github Issue #48 and PR #49
	Until it is approved or rejected it is reimplemented here.
	Will be required for back compatibility with older version as well.
*/

/*
Copyright (c) 2013, Michael Ossmann <mike@ossmann.com>
Copyright (c) 2012, Jared Boone <jared@sharebrained.com>
Copyright (c) 2014, Youssef Touil <youssef@airspy.com>
Copyright (c) 2014, Benjamin Vernoux <bvernoux@airspy.com>
Copyright (c) 2015, Ian Gilmour <ian@sdrsharp.com>

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

		Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
		Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.
		Neither the name of AirSpy nor the names of its contributors may be used to endorse or promote products derived from this software
		without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "airspy_list_devices.h"

#include <cstdlib>
#include <cstring>

static const uint16_t airspy_usb_vid = 0x1d50;
static const uint16_t airspy_usb_pid = 0x60a1;

#define STR_PREFIX_SERIAL_AIRSPY_SIZE (10)
#define SERIAL_AIRSPY_EXPECTED_SIZE (26)

int airspy_list_devices(uint64_t *serials, int count) {
	libusb_device_handle* libusb_dev_handle;
	struct libusb_context *context;
	libusb_device** devices = NULL;
	libusb_device *dev;
	struct libusb_device_descriptor device_descriptor;

	int serial_descriptor_index;
	int serial_number_len;
	int output_count;
	int i;
	unsigned char serial_number[SERIAL_AIRSPY_EXPECTED_SIZE + 1];

	if (serials) {
		memset(serials, 0, sizeof(uint64_t) * count);
	}

	if (libusb_init(&context) != 0) {
		return -1;
	}

	if (libusb_get_device_list(context, &devices) < 0) {
		return -1;
	}

	i = 0;
	output_count = 0;

	while ((dev = devices[i++]) != NULL && (!serials || output_count < count)) {
		libusb_get_device_descriptor(dev, &device_descriptor);

		if ((device_descriptor.idVendor == airspy_usb_vid) &&
			(device_descriptor.idProduct == airspy_usb_pid)) {
			serial_descriptor_index = device_descriptor.iSerialNumber;

			if (serial_descriptor_index > 0) {
				if (libusb_open(dev, &libusb_dev_handle) != 0) {
					continue;
				}

				serial_number_len = libusb_get_string_descriptor_ascii(libusb_dev_handle,
					serial_descriptor_index,
					serial_number,
					sizeof(serial_number));

				if (serial_number_len == SERIAL_AIRSPY_EXPECTED_SIZE) {
					char *start, *end;
					uint64_t serial;

					serial_number[SERIAL_AIRSPY_EXPECTED_SIZE] = 0;
					start = (char*)(serial_number + STR_PREFIX_SERIAL_AIRSPY_SIZE);
					end = NULL;
					serial = strtoull(start, &end, 16);

					if (serial == 0 && start == end) {
						libusb_close(libusb_dev_handle);
						continue;
					}

					if (serials) {
						serials[output_count] = serial;
					}

					output_count++;
				}

				libusb_close(libusb_dev_handle);
			}
		}
	}

	libusb_free_device_list(devices, 1);
	libusb_exit(context);

	return output_count;
}
