#include <stdlib.h>
#include <stdio.h>

#include <chrono>
#include <filesystem>
#include <ctime>
#include <iostream>

#include "httplib.h"
#include "nlohmann/json.hpp"

#include "tray.hpp"


using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

// HTTP
httplib::Server svr;

std::string execCommand(const std::string cmd, int& out_exitStatus) {
	#ifndef popen
	#define popen _popen
	#define pclose _pclose
	#endif
	out_exitStatus = 0;
	auto pPipe = ::popen(cmd.c_str(), "r");
	if (pPipe == nullptr) {
		throw std::runtime_error("Cannot open pipe");
	}

	std::array<char, 256> buffer;

	std::string result;

	while (not std::feof(pPipe)) {
		auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
		result.append(buffer.data(), bytes);
	}

	auto rc = ::pclose(pPipe);
	if (rc == -1) {
		throw std::runtime_error("Cannot close pipe");
	}
	return result;
}

int64_t now() {
	const auto time = std::chrono::system_clock::now();
	const int64_t now =
		std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
	return now;
}

void add_cors(const httplib::Request& req, httplib::Response& res) {
	res.set_header("Access-Control-Allow-Origin", req.get_header_value("Origin").c_str());
	res.set_header("Allow", "GET, POST, HEAD, OPTIONS");
	res.set_header("Access-Control-Allow-Headers",
					 "X-Requested-With, Content-Type, Accept, Origin, Authorization");
	res.set_header("Access-Control-Allow-Methods", "OPTIONS, GET, POST, HEAD");
}

int main(int argc, char** argv) {
	Tray::Tray tray("My Tray", "icon.ico");
	tray.addEntry(Tray::Button("Exit", [&]{
		svr.stop();
		tray.exit();
	}));
	svr.set_error_handler([](const auto& req, auto& res) {
		auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "text/html");
	});

	svr.Get("/stats", [](const httplib::Request&, httplib::Response& res) {
		json j;
		j["epoch"] = now();
		res.set_content(j.dump(), "application/json");
	});
	svr.set_pre_routing_handler([](const auto& req, auto& res) {
		add_cors(req, res);
		return httplib::Server::HandlerResponse::Unhandled;
	});

	svr.Get("/window", [argv](const httplib::Request& req, httplib::Response& res) {
		int exit_status;
		char pBuf[256];
		size_t len = sizeof(pBuf);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		int bytes = GetModuleFileName(NULL, pBuf, len);
		if (bytes >= 0)
			pBuf[bytes] = '\0';
		fs::path location = pBuf;
		location.remove_filename();
		string result =
			execCommand("powershell.exe -ExecutionPolicy ByPass -File " + location.string() + "scripts/windows.ps1", exit_status);
#else
		int bytes = MIN(readlink("/proc/self/exe", pBuf, len), len - 1);
		if(bytes >= 0)
			pBuf[bytes] = '\0';
		fs::path location = pBuf;
		location.remove_filename();
		string result = execCommand("bash " + location.string() + "/scripts/linux.sh", exit_status);
#endif
		res.set_content(result, "text/plain");
	});

	cout << "Server listening on 8080\n";

	std::thread server_thread([]() {
		svr.listen("0.0.0.0", 8080);
		return 0;
	});

	tray.run();
	server_thread.join();
}