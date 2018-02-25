# Set compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
add_definitions(-std=c++11)

# Look for mandatory libraries
find_package(LibBoostProgramOptions REQUIRED)
if (LibBoostProgramOptions_FOUND)
	include_directories(${LibBoostProgramOptions_INCLUDE_DIRS})
endif (LibBoostProgramOptions_FOUND)

find_package(LibYaml REQUIRED)
if (LibYaml_FOUND)
	include_directories(${LibYaml_INCLUDE_DIRS})
endif (LibYaml_FOUND)

find_package(LibFftw3 REQUIRED)
if (LibFftw3_FOUND)
	include_directories(${LibFftw3_INCLUDE_DIRS})
endif (LibFftw3_FOUND)

find_package(LibGd REQUIRED)
if (LibGd_FOUND)
	include_directories(${LibGd_INCLUDE_DIRS})
endif (LibGd_FOUND)

# Look for optional input module libraries
find_package(LibAirSpy)
if (LibAirSpy_FOUND)
	include_directories(${LibAirSpy_INCLUDE_DIRS})
endif (LibAirSpy_FOUND)

find_package(LibAirSpyHF)
if (LibAirSpyHF_FOUND)
	include_directories(${LibAirSpyHF_INCLUDE_DIRS})
endif (LibAirSpyHF_FOUND)

find_package(LibAlsa)
if (LibAlsa_FOUND)
	include_directories(${LibAlsa_INCLUDE_DIRS})
endif (LibAlsa_FOUND)

find_package(LibGROsmoSdr)
if (LibGROsmoSdr_FOUND)
	include_directories(${LibGROsmoSdr_INCLUDE_DIRS})
endif (LibGROsmoSdr_FOUND)

find_package(LibHackRF)
if (LibHackRF_FOUND)
	include_directories(${LibHackRF_INCLUDE_DIRS})
endif (LibHackRF_FOUND)

find_package(LibLimeSuite)
if (LibLimeSuite_FOUND)
	include_directories(${LibLimeSuite_INCLUDE_DIRS})
endif (LibLimeSuite_FOUND)

find_package(LibRtlSdr)
if (LibRtlSdr_FOUND)
	include_directories(${LibRtlSdr_INCLUDE_DIRS})
endif (LibRtlSdr_FOUND)

# Look for optional processing libraries
find_package(LibLiquidSDR)
if (LibLiquidSDR_FOUND)
	include_directories(${LibLiquidSDR_INCLUDE_DIRS})
endif (LibLiquidSDR_FOUND)

# Don't look for librtfilter if libliquid available
if (NOT LibLiquidSDR_FOUND)
	find_package(LibRTFilter)
	if (LibRTFilter_FOUND)
		include_directories(${LibRTFilter_INCLUDE_DIRS})
	endif (LibRTFilter_FOUND)
endif (NOT LibLiquidSDR_FOUND)

# Loop for optional upload libraries
find_package(LibSSH)
if (LibSSH_FOUND)
	include_directories(${LibSSH_INCLUDE_DIRS})
endif (LibSSH_FOUND)

find_package(LibCurl)
if (LibCurl_FOUND)
	include_directories(${LibCurl_INCLUDE_DIRS})
endif (LibCurl_FOUND)

list(APPEND QRSSPIG_SOURCES main.cpp QrssPiG.cpp QGUtils.cpp QGInputDevice.cpp QGInputStdIn.cpp QGProcessor.cpp QGImage.cpp QGUploader.cpp QGUploaderLocal.cpp)

if (LibAirSpy_FOUND)
	list(APPEND QRSSPIG_SOURCES QGInputAirSpy.cpp airspy_list_devices.cpp)
endif (LibAirSpy_FOUND)

if (LibAirSpyHF_FOUND)
	list(APPEND QRSSPIG_SOURCES QGInputAirSpyHF.cpp)
endif (LibAirSpyHF_FOUND)

if (LibAlsa_FOUND)
	list(APPEND QRSSPIG_SOURCES QGInputAlsa.cpp)
endif (LibAlsa_FOUND)

if (LibGROsmoSdr_FOUND)
	list(APPEND QRSSPIG_SOURCES QGInputGROsmoSdr.cpp)
endif (LibGROsmoSdr_FOUND)

if (LibHackRF_FOUND)
	list(APPEND QRSSPIG_SOURCES QGInputHackRF.cpp)
endif (LibHackRF_FOUND)

if (LibLimeSuite_FOUND)
	list(APPEND QRSSPIG_SOURCES QGInputLime.cpp)
endif (LibLimeSuite_FOUND)

if (LibRtlSdr_FOUND)
	list(APPEND QRSSPIG_SOURCES QGInputRtlSdr.cpp)
endif (LibRtlSdr_FOUND)

if (LibSSH_FOUND)
	list(APPEND QRSSPIG_SOURCES QGUploaderSCP.cpp)
endif (LibSSH_FOUND)

if (LibCurl_FOUND)
	list(APPEND QRSSPIG_SOURCES QGUploaderFTP.cpp)
endif (LibCurl_FOUND)

add_executable (qrsspig ${QRSSPIG_SOURCES})

target_link_libraries(qrsspig pthread)

if (LibBoostProgramOptions_FOUND)
	target_link_libraries(qrsspig ${LibBoostProgramOptions_LIBRARIES})
endif (LibBoostProgramOptions_FOUND)

if (LibYaml_FOUND)
	target_link_libraries(qrsspig ${LibYaml_LIBRARIES})
endif (LibYaml_FOUND)

if (LibFftw3_FOUND)
	target_link_libraries(qrsspig ${LibFftw3_LIBRARIES})
endif (LibFftw3_FOUND)

if (LibGd_FOUND)
	target_link_libraries(qrsspig ${LibGd_LIBRARIES})
endif (LibGd_FOUND)

if (LibAirSpy_FOUND)
	target_link_libraries(qrsspig ${LibAirSpy_LIBRARIES})
	set(HAVE_LIBAIRSPY 1)
endif (LibAirSpy_FOUND)

if (LibAirSpyHF_FOUND)
	target_link_libraries(qrsspig ${LibAirSpyHF_LIBRARIES})
	set(HAVE_LIBAIRSPYHF 1)
endif (LibAirSpyHF_FOUND)

if (LibAlsa_FOUND)
	target_link_libraries(qrsspig ${LibAlsa_LIBRARIES})
	set(HAVE_LIBALSA 1)
endif (LibAlsa_FOUND)

if (LibGROsmoSdr_FOUND)
	target_link_libraries(qrsspig ${LibGROsmoSdr_LIBRARIES})
	set(HAVE_LIBGROSMOSDR 1)
endif (LibGROsmoSdr_FOUND)

if (LibHackRF_FOUND)
	target_link_libraries(qrsspig ${LibHackRF_LIBRARIES})
	set(HAVE_LIBHACKRF 1)
	if (LibHackRF_HAS_hackrf_library_version)
		set(HAVE_LIBHACKRF_LIBRARY_VERSION 1)
	endif (LibHackRF_HAS_hackrf_library_version)
	if (LibHackRF_HAS_hackrf_device_list)
		set(HAVE_LIBHACKRF_DEVICE_LIST 1)
	endif (LibHackRF_HAS_hackrf_device_list)
	if (LibHackRF_HAS_hackrf_set_antenna_enable)
		set(HAVE_LIBHACKRF_ANTENNA_ENABLE 1)
	endif (LibHackRF_HAS_hackrf_set_antenna_enable)
endif (LibHackRF_FOUND)

if (LibLimeSuite_FOUND)
	target_link_libraries(qrsspig ${LibLimeSuite_LIBRARIES})
	set(HAVE_LIBLIMESUITE 1)
endif (LibLimeSuite_FOUND)

if (LibRtlSdr_FOUND)
	target_link_libraries(qrsspig ${LibRtlSdr_LIBRARIES})
	set(HAVE_LIBRTLSDR 1)
	if (LibRtlSdr_HAS_rtlsdr_get_device_count)
		set(HAVE_LIBRTLSDR_DEVICE_LIST 1)
	endif (LibRtlSdr_HAS_rtlsdr_get_device_count)
	if (LibRtlSdr_HAS_rtlsdr_set_tuner_bandwidth)
		set(HAVE_LIBRTLSDR_TUNER_BANDWITH 1)
	endif (LibRtlSdr_HAS_rtlsdr_set_tuner_bandwidth)
endif (LibRtlSdr_FOUND)

if (LibLiquidSDR_FOUND)
	target_link_libraries(qrsspig ${LibLiquidSDR_LIBRARIES})
	set(HAVE_LIBLIQUIDSDR 1)
endif (LibLiquidSDR_FOUND)

if (LibRTFilter_FOUND)
	target_link_libraries(qrsspig ${LibRTFilter_LIBRARIES})
	set(HAVE_LIBRTFILTER 1)
endif (LibRTFilter_FOUND)

if (LibSSH_FOUND)
	target_link_libraries(qrsspig ${LibSSH_LIBRARIES})
	set(HAVE_LIBSSH 1)
endif (LibSSH_FOUND)

if (LibCurl_FOUND)
	target_link_libraries(qrsspig ${LibCurl_LIBRARIES})
	set(HAVE_LIBCURL 1)
endif (LibCurl_FOUND)

# Generate and include Config.h
configure_file ("Config.h.in" "${PROJECT_BINARY_DIR}/Config.h")
include_directories("${PROJECT_BINARY_DIR}")

install(TARGETS qrsspig RUNTIME DESTINATION bin)
