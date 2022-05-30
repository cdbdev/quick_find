#pragma once

#include <string>
#include <set>

class ConfigReader
{
public:
	struct configinfo {
		std::wstring workspace;
		std::set<std::string> filetypes;
		int action;
	};

	// Constructor
	ConfigReader() {}

	// read method
	configinfo read();
};

