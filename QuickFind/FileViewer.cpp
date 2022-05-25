#include "FileViewer.h"
#include <windows.h>

void FileViewer::view()
{
	ShellExecute(0, 0, m_fileName.c_str(), 0, 0, SW_SHOW);
}