#include "QGUploader.h"
#include "Config.h"

#include <cstring>
#include <functional>
#include <iostream>
#include <syslog.h>
#include <thread>

#include "QGUtils.h"

#include "QGUploaderLocal.h"

#ifdef HAVE_LIBCURL
#include "QGUploaderFTP.h"
#endif // HAVE_LIBCURL

#ifdef HAVE_LIBSSH
#include "QGUploaderSCP.h"
#endif // HAVE_LIBSSH

std::vector<std::string> QGUploader::listModules() {
    std::vector<std::string> modules;

    modules.push_back("File");
#ifdef HAVE_LIBCURL
    modules.push_back("FTP");
#endif //HAVE_LIBCURL
#ifdef HAVE_LIBSSH
    modules.push_back("SCP");
#endif //HAVE_LIBSSH

    return modules;
}

QGUploader::QGUploader(const YAML::Node &config) {
    _fileNameTmpl = "";
    _verbose = false;
    _pushIntermediate = false;

    if (config["filename"]) _fileNameTmpl = config["filename"].as<std::string>();
    if (config["verbose"]) _verbose = config["verbose"].as<bool>();
    if (config["intermediate"]) _pushIntermediate = config["intermediate"].as<bool>();
}

// Push is done in a thread on a copy of the data.
// Parent class handles copying of the data, creation of the thread and finally free the data
// wait param can be set to true to block until pushed. Used on program exit

void QGUploader::push(const std::string &fileNameTmpl, const std::string &fileExt, long int freq, std::chrono::milliseconds frameStart, const char *data, int dataSize, bool intermediate, bool wait) {
    if (intermediate && !_pushIntermediate) return;

    char *d = new char[dataSize];

    if (!d) throw std::runtime_error("Out of memory");

    std::memcpy(d, data, dataSize);

    std::string fileName;
	QGUtils::formatFilename(_fileNameTmpl.length() > 0 ? _fileNameTmpl : fileNameTmpl, fileName, freq, frameStart);
    fileName += "." + fileExt;

    if (wait)
        std::thread(std::bind(&QGUploader::_pushThread, this, fileName, d, dataSize)).join();
    else
        std::thread(std::bind(&QGUploader::_pushThread, this, fileName, d, dataSize)).detach();
}

std::unique_ptr<QGUploader> QGUploader::CreateUploader(const YAML::Node &config) {
    if (!config["type"]) throw std::runtime_error("YAML: uploader must have a type");

    if (config["type"].as<std::string>().compare("local") == 0) {
        return std::unique_ptr<QGUploader>(new QGUploaderLocal(config));
    } else if (config["type"].as<std::string>().compare("scp") == 0) {
#ifdef HAVE_LIBSSH
        return std::unique_ptr<QGUploader>(new QGUploaderSCP(config));
#else
        throw std::runtime_error("QGUploader: scp uploader support not builtin into this build");
#endif //  HAVE_LIBSSH
    } else if ((config["type"].as<std::string>().compare("ftp") == 0) || (config["type"].as<std::string>().compare("ftps") == 0)) {
#ifdef HAVE_LIBCURL
        return std::unique_ptr<QGUploader>(new QGUploaderFTP(config));
#else
        throw std::runtime_error("QGUploader: ftp uploader support not builtin into this build");
#endif //  HAVE_LIBCURL
    } else {
        throw std::runtime_error(std::string("QGUploader: unknown type ") + config["type"].as<std::string>());
    }
}

void QGUploader::_pushThread(std::string fileName, const char *data, int dataSize) {
    std::string uri;

    try {
        _pushThreadImpl(fileName, data, dataSize, uri);
		syslog(LOG_ERR, "pushed %s", uri.c_str());
        std::cout << "pushed " << uri << std::endl;
    } catch (const std::exception &e) {
		syslog(LOG_ERR, "pushing %s failed: %s", uri.c_str(), e.what());
        std::cout << "pushing " << uri << " failed: " << e.what() << std::endl;
    }
    // Delete data allocated above so specific implementations don't have to worry
    delete [] data;
}
