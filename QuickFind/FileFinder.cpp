#include "FileFinder.h"
#include <algorithm>
#include <tchar.h>
#include <strsafe.h>

std::vector<FileFinder::fileinfo> FileFinder::find(std::wstring dir) {
	std::vector<FileFinder::fileinfo> fileList;
	TCHAR directory[MAX_PATH]{};
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	DWORD dwError = 0;

	// Check if dir ends with '/'	
	if (dir.size() > 0 && dir.back() != '/') {
		dir.push_back('/');
	}

	// Check directory does not exceed max length
	// Three characters are for the "\*" plus NULL appended below.
	if (dir.size() >= MAX_PATH - 3) {
		return {};
	}

	// Copy string to TCHAR for further win32 api processing
	copy(dir.begin(), dir.end(), &directory[0]);
	directory[dir.size()] = _T('\0');

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.
	StringCchCopy(szDir, MAX_PATH, directory);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return {};
	}

	wchar_t* fileName = ffd.cFileName;

	// List all the files in the directory with some info about them.
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			fileName = ffd.cFileName; // fileName in this case is the directoryName :)

			// Do not recurse in case of '.' or '..'
			if (wcscmp(fileName, L".") != 0 && wcscmp(fileName, L"..") != 0) {
				std::wstring dirTmp = dir;
				auto subFolderResults = find(dirTmp.append(fileName));
				fileList.insert(fileList.end(), subFolderResults.begin(), subFolderResults.end());
			}
		}
		else
		{
			fileName = ffd.cFileName;

			// check if extension works out
			bool extFound = false;
			std::set<std::string>::iterator itr;

			for (itr = m_filetypes.begin(); itr != m_filetypes.end(); itr++)
			{
				if (endsWith(std::wstring(fileName), *itr)) {
					extFound = true;
					break;
				}
			}

			if (!extFound)
				continue;

			// check if input is part of the filename
			if (wcsstr(fileName, m_fileName.c_str()) != 0 || dir.find(m_fileName) != std::string::npos) {
				fileList.push_back(FileFinder::fileinfo{ dir, fileName });
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		//Last file detected
	}
	FindClose(hFind);

	return fileList;
}

bool FileFinder::endsWith(std::wstring const& str, std::string const& suffix) {
	//std::string fn = std::string(str.begin(), str.end());
	
	size_t extLength = suffix.size();
	return str.substr(str.size() - extLength, str.size()-1) == std::wstring(suffix.begin(), suffix.end());
}