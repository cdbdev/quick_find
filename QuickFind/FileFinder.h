#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <set>

class FileFinder
{
public:
	struct fileinfo {
		std::wstring directory;
		std::wstring fileName;
	};

	// Initialize FileFinder
	FileFinder(std::wstring  fileName, std::set<std::string> fileTypes)
		: m_fileName(fileName), m_filetypes(fileTypes)
	{}

	// find method
	std::vector<fileinfo> find(std::wstring dir);

private:
	std::wstring  m_fileName;
	std::set<std::string> m_filetypes;

	bool endsWith(std::wstring const& str, std::string const& suffix);
};

