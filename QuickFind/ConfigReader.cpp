#include <fstream>
#include <windows.h>
#include <sstream>

#include "ConfigReader.h"

ConfigReader::configinfo ConfigReader::read()
{
    configinfo configInfo;

    const std::wstring configPath = L"QuickFindGlobal.properties";

    // Get current location based on exe file
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring exePath = std::wstring(path);

    // Get position of path without executable
    std::size_t sepPos = exePath.rfind('\\') + 1;

    // Check if properties file exists
    std::wstring configFile = exePath.substr(0, sepPos) + configPath;
    std::ifstream f(configFile);
    if (!f.good())
        return configInfo; // file not found, maybe deleted?

    // Read properties
    if (f.is_open()) {
        std::string line;

        while (std::getline(f,line)) {
            // Remove spaces if present
            line.erase(std::remove_if(line.begin(), line.end(), std::isspace), line.end());

            // check for comment
            if (line[0] == '#' || line.empty())
                continue;

            // Process name/value pairs
            size_t delimiterPos = line.find("=");
            std::string configName = line.substr(0, delimiterPos);
            std::string configValue = line.substr(delimiterPos + 1);

            if (configName == "workspace") {
                std::wstring value = std::wstring(configValue.begin(), configValue.end());
                configInfo.workspace = value;
            }
            else if (configName == "filetypes") {
                // construct a stream from the string
                std::string token;
                std::stringstream ss(configValue);

                // Add filetypes
                while (std::getline(ss, token, ',')) {
                    configInfo.filetypes.insert(token);
                }
            }
            else if (configName == "action") {
                configInfo.action = std::stoi(configValue);
            }
            else {
                continue;
            }
        }
    }

    return configInfo;
}
