#pragma once

#include "ConfigReader.h"
#include <string>
#include <windows.h>

class FileViewer
{
public:
	// Initialize FileViewer
	FileViewer(ConfigReader::configinfo* configInfo, std::wstring  fileName)
		: m_configInfo(configInfo), m_fileName(fileName)
	{}

	// view method
	int view();

private:
	ConfigReader::configinfo* m_configInfo;
	std::wstring  m_fileName;

	HWND find_scite();
};

