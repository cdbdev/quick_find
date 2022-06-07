#include <locale>
#include <codecvt>
#include "FileViewer.h"

int FileViewer::view()
{
	if (m_configInfo->action != 2) {// System editor
		ShellExecute(0, 0, m_fileName.c_str(), 0, 0, SW_SHOW);
        return 0;
	}
	else {// SciTE editor
        HWND w = find_scite();
        if (w == 0) {
            // Couldn't find running instance of SciTE
            return -1;
        }
        else {
            // Open file with SciTE
            std::wstring cmd = L"open:";
            cmd = cmd + m_fileName;

            COPYDATASTRUCT cds{};
            cds.dwData = 0;
            cds.cbData = (DWORD)cmd.size();

            // First convert to string (necessary to pass to SendMessage)
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::string cmdString = converter.to_bytes(cmd);
            cds.lpData = (void*)cmdString.c_str();

            // Pass the command to SciTE
            SendMessage(w, WM_COPYDATA, 0, (LPARAM)(LPVOID)&cds);
            return 0;
        }
	}
}

HWND FileViewer::find_scite() {
    HWND target = 0;

    unsigned int SDI = RegisterWindowMessage(L"SciTEDirectorInterface");

    HWND w = GetWindow(GetDesktopWindow(), GW_CHILD);

    while (w) {
        DWORD_PTR res = 0;
        SendMessageTimeout(w, SDI, 0, 0, SMTO_NORMAL, 1000, &res);
        if (res == static_cast<DWORD>(SDI)) {
            return w;
        }

        w = GetWindow(w, GW_HWNDNEXT);
    }

    return 0;
}