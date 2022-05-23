#pragma once
#include <windows.h>
#include <vector>
#include <string>

class FileFinder
{
public:
	struct fileinfo {
		std::wstring directory;
		std::wstring fileName;
	};

	// Initialize FileFinder
	FileFinder(std::wstring  fileName)
		: m_fileName(fileName)
	{}

	// find method
	std::vector<fileinfo> find(std::wstring dir);

private:
	std::wstring  m_fileName;
};

