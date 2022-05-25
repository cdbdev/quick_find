#pragma once
#include <string>

class FileViewer
{
public:
	// Initialize FileViewer
	FileViewer(std::wstring  fileName)
		: m_fileName(fileName)
	{}

	// view method
	void view();

private:
	std::wstring  m_fileName;
};

