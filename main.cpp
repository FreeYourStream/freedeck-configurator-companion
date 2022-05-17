#define IS_WINDOWS false
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define IS_WINDOWS true
#endif

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
fs::path location;


bool is_windows() {
	return IS_WINDOWS;
}

std::string execCommand(const std::string cmd, int& out_exitStatus) {
	#if IS_WINDOWS
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

	auto rc = _pclose(pPipe);
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
	char pBuf[256];
	size_t len = sizeof(pBuf);
#if IS_WINDOWS
	//ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
	int bytes = GetModuleFileName(NULL, pBuf, len);
#else
	int bytes = MIN(readlink("/proc/self/exe", pBuf, len), len - 1);
#endif
	if(bytes >= 0)
		pBuf[bytes] = '\0';
	location = pBuf;
	location.remove_filename();
#if IS_WINDOWS
	Tray::Tray tray("My Tray", location.string() + "/icon.ico");
#else
	Tray::Tray tray("My Tray", location.string() + "/icon.png");
#endif
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
#if IS_WINDOWS
		TCHAR buff[255];
		GetWindowText(GetForegroundWindow(), buff, 255);
		string result = buff;
#else
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


#if IS_WINDOWS
// pretty ghetto, but it's windows, so i don't care for now :)
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR ipCmdLine, int nCmdShow) {
	main(0, NULL);
}
#endif