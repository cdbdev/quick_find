#include <Windows.h>
#include <commctrl.h>
#include <tchar.h>
#include "resource.h"
#include <string>

#include "ConfigReader.h"
#include "FileFinder.h"
#include "FileViewer.h"

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")


INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buff[1024];
	HWND editControl;
	HWND listControl;

	LVCOLUMN lvc{};
	LVITEM lvi{};

	int iSlected = 0;
	wchar_t colValue[255]{};

	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;
		case IDOK:
			// Get Edit Control
			editControl = GetDlgItem(hDlg, IDC_EDIT1);
			//GetWindowText(editControl, buff, 1024);
			GetWindowText(editControl, buff, sizeof(buff) / sizeof(TCHAR));
			auto inputLength = _tcslen(buff);
			if (inputLength < 3) {
				MessageBox(hDlg, TEXT("Please enter a search string of at least 3 characters."), TEXT("Warning"), MB_OK);
				return TRUE;
			}

			// Get List Control
			listControl = GetDlgItem(hDlg, IDC_LIST1);
			ListView_DeleteAllItems(listControl);

			// Initialize items

			lvi.mask = LVIF_TEXT | LVIF_STATE;
			lvi.stateMask = 0;
			lvi.state = 0;

			// BEGIN TEST FileFinder
			FileFinder ff(buff);
			auto filesFound = ff.find(L"c:/tmp/new/");

			for (auto it = std::begin(filesFound); it != std::end(filesFound); ++it) {
				int index = std::distance(filesFound.begin(), it);

				FileFinder::fileinfo fileInfo = *it;
				std::wstring fileName = fileInfo.fileName;
				std::wstring directory = fileInfo.directory;

				// First column
				lvi.iItem = index;
				lvi.iSubItem = 0;
				lvi.pszText = LPWSTR(fileName.c_str());

				if (ListView_InsertItem(listControl, &lvi) == -1)
					return FALSE;

				// Second column
				lvi.iItem = index;
				lvi.iSubItem = 1;
				lvi.pszText = LPWSTR(directory.c_str());

				if (ListView_SetItem(listControl, &lvi) == -1)
					return FALSE;
			}

			// Autosize columns
			ListView_SetColumnWidth(listControl, 0, LVSCW_AUTOSIZE);
			ListView_SetColumnWidth(listControl, 1, LVSCW_AUTOSIZE);

			// END TEST

			return TRUE;
		}
		break;

	case WM_CLOSE:
		if (MessageBox(hDlg, TEXT("Close the program?"), TEXT("Close"),
			MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			DestroyWindow(hDlg);
		}
		return TRUE;

	case WM_INITDIALOG:
	{
		// Get config info		
		ConfigReader cr;
		ConfigReader::configinfo configInfo = cr.read();

		memset(&lvc, 0, sizeof(2));

		listControl = GetDlgItem(hDlg, IDC_LIST1);
		SendMessage(listControl, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

		/********************************************************************
		*   DEFINE THE COLUMNS
		*****************************************************************/

		// The mask specifies that the format, width, text,
		// and subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

		// First column
		lvc.iSubItem = 0;
		lvc.pszText = LPWSTR(L"FileName");;
		lvc.cx = 100; // width
		lvc.fmt = LVCFMT_LEFT;

		if (ListView_InsertColumn(listControl, 0, &lvc) == -1)
			return FALSE;

		// Second column
		lvc.iSubItem = 0;
		lvc.pszText = LPWSTR(L"Directory");
		lvc.cx = 120; // width
		lvc.fmt = LVCFMT_LEFT;

		if (ListView_InsertColumn(listControl, 1, &lvc) == -1)
			return FALSE;

		return TRUE;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_DBLCLK:
			listControl = GetDlgItem(hDlg, IDC_LIST1);
			iSlected = SendMessage(listControl, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
			if (iSlected == -1)
			{
				MessageBox(hDlg, TEXT("No Items in List"),TEXT("Warning"), MB_OK | MB_ICONINFORMATION);
				break;
			}

			// Get selected line directory;
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 1;
			lvi.pszText = colValue;
			lvi.cchTextMax = 256;
			lvi.iItem = iSlected;
			SendMessage(listControl, LVM_GETITEMTEXT, iSlected, (LPARAM)&lvi);
			std::wstring dirView = lvi.pszText;

			// Get selected line file
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem = 0;
			lvi.pszText = colValue;
			lvi.cchTextMax = 256;
			lvi.iItem = iSlected;
			SendMessage(listControl, LVM_GETITEMTEXT, iSlected, (LPARAM)&lvi);
			std::wstring fileView = lvi.pszText;

			//MessageBox(hDlg, lvi.pszText, TEXT("Info"), MB_OK);
			FileViewer fv(dirView+fileView);
			fv.view();
		}
		return TRUE;
	}

	return FALSE;
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
	HWND hDlg;
	MSG msg;
	BOOL ret;

	InitCommonControls();
	hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
	ShowWindow(hDlg, nCmdShow);

	while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if (ret == -1)
			return -1;

		if (!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}