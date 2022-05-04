#include <stdlib.h>

#include <chrono>
#include <ctime>
#include <iostream>

#include "httplib.h"
#include "nlohmann/json.hpp"

using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

// HTTP
httplib::Server svr;

std::string execCommand(const std::string cmd, int& out_exitStatus)
{
    out_exitStatus = 0;
    auto pPipe = ::popen(cmd.c_str(), "r");
    if(pPipe == nullptr)
    {
        throw std::runtime_error("Cannot open pipe");
    }

    std::array<char, 256> buffer;

    std::string result;

    while(not std::feof(pPipe))
    {
        auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
        result.append(buffer.data(), bytes);
    }

    auto rc = ::pclose(pPipe);

    if(WIFEXITED(rc))
    {
        out_exitStatus = WEXITSTATUS(rc);
    }

    return result;
}

int64_t now() {
	const auto time = std::chrono::system_clock::now();
	const int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
							time.time_since_epoch())
							.count();
  return now;
}

int main(int argc, char ** argv) {
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

  svr.Get("/window", [argv](const httplib::Request&, httplib::Response& res) {
    int exit_status;
    char pBuf[256];
    size_t len = sizeof(pBuf); 
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    int bytes = GetModuleFileName(NULL, pBuf, len);
    string result = execCommand("powershell.exe -ExecutionPolicy ByPass -File windows.ps1", exit_status);
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
	svr.listen("0.0.0.0", 8080);
}