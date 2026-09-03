#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <filesystem>
#include "Converters.h"
#include "FileNames.h"

#pragma comment(lib, "comctl32.lib")

namespace {

	// コントロールID
	enum ControlId {
		ID_EDIT_INPUT = 1001,
		ID_BUTTON_INPUT_BROWSE,
		ID_EDIT_OUTPUT,
		ID_BUTTON_OUTPUT_BROWSE,
		ID_RADIO_ISEKAI,
		ID_RADIO_BAKIN,
		ID_RADIO_ISEKAI_FACE,
		ID_BUTTON_CONVERT,
		ID_LIST_LOG,
	};

	HWND g_hEditInput = nullptr;
	HWND g_hEditOutput = nullptr;
	HWND g_hRadioIsekai = nullptr;
	HWND g_hRadioBakin = nullptr;
	HWND g_hRadioIsekaiFace = nullptr;
	HWND g_hListLog = nullptr;
	HWND g_hButtonConvert = nullptr;

	std::wstring Utf8ToWide(const std::string& s) {
		if (s.empty()) return std::wstring();
		int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
		std::wstring result(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &result[0], len);
		return result;
	}

	std::string WideToUtf8(const std::wstring& s) {
		if (s.empty()) return std::string();
		int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0, nullptr, nullptr);
		std::string result(len, '\0');
		WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), &result[0], len, nullptr, nullptr);
		return result;
	}

	std::string GetWindowTextUtf8(HWND hwnd) {
		int len = GetWindowTextLengthW(hwnd);
		std::wstring buf(len, L'\0');
		if (len > 0) {
			GetWindowTextW(hwnd, &buf[0], len + 1);
		}
		return WideToUtf8(buf);
	}

	void AppendLog(const std::string& message) {
		std::wstring wmessage = Utf8ToWide(message);
		int index = (int)SendMessageW(g_hListLog, LB_ADDSTRING, 0, (LPARAM)wmessage.c_str());
		SendMessageW(g_hListLog, LB_SETTOPINDEX, index, 0);
	}

	// フォルダ選択ダイアログを表示し、選択されたパス(UTF-8)を返す。キャンセル時は空文字。
	std::string BrowseForFolder(HWND owner, const wchar_t* title) {
		std::string result;
		BROWSEINFOW bi = { 0 };
		bi.hwndOwner = owner;
		bi.lpszTitle = title;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

		LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
		if (pidl != nullptr) {
			wchar_t path[MAX_PATH] = { 0 };
			if (SHGetPathFromIDListW(pidl, path)) {
				result = WideToUtf8(path);
			}
			CoTaskMemFree(pidl);
		}
		return result;
	}

	void DoConvert(HWND hwnd) {
		std::string inputDir = GetWindowTextUtf8(g_hEditInput);
		std::string outputDir = GetWindowTextUtf8(g_hEditOutput);

		if (inputDir.empty() || outputDir.empty()) {
			MessageBoxW(hwnd, L"入力フォルダと出力フォルダを指定してください。", L"CharachipConverter", MB_OK | MB_ICONWARNING);
			return;
		}

		std::error_code ec;
		std::filesystem::create_directories(outputDir, ec);

		std::string mode;
		if (SendMessageW(g_hRadioIsekai, BM_GETCHECK, 0, 0) == BST_CHECKED) mode = "i";
		else if (SendMessageW(g_hRadioBakin, BM_GETCHECK, 0, 0) == BST_CHECKED) mode = "b";
		else if (SendMessageW(g_hRadioIsekaiFace, BM_GETCHECK, 0, 0) == BST_CHECKED) mode = "f";

		if (mode.empty()) {
			MessageBoxW(hwnd, L"変換モードを選択してください。", L"CharachipConverter", MB_OK | MB_ICONWARNING);
			return;
		}

		std::vector<std::string> inputs;
		if (!getFileNames(inputDir, inputs) || inputs.empty()) {
			MessageBoxW(hwnd, L"入力フォルダに変換元ファイルがありません。", L"CharachipConverter", MB_OK | MB_ICONWARNING);
			return;
		}

		EnableWindow(g_hButtonConvert, FALSE);
		SendMessageW(g_hListLog, LB_RESETCONTENT, 0, 0);

		LogFunc log = AppendLog;
		int converted = 0;
		for (const std::string& filename : inputs) {
			if (!std::filesystem::is_regular_file(filename, ec)) {
				continue;
			}
			if (mode == "i") {
				convert_isekai(filename, outputDir, 3, 4, log);
			}
			else if (mode == "b") {
				convert_bakin(filename, outputDir, 1, 4, log);
			}
			else if (mode == "f") {
				convert_isekai_face(filename, outputDir, 4, 2, log);
			}
			converted++;
		}

		EnableWindow(g_hButtonConvert, TRUE);
		AppendLog("--- 変換完了 (" + std::to_string(converted) + " 件) ---");
		MessageBoxW(hwnd, L"変換が完了しました。", L"CharachipConverter", MB_OK | MB_ICONINFORMATION);
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_CREATE: {
			HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

			auto createLabel = [&](const wchar_t* text, int x, int y, int w, int h) {
				HWND h_ = CreateWindowW(L"STATIC", text, WS_CHILD | WS_VISIBLE, x, y, w, h, hwnd, nullptr, nullptr, nullptr);
				SendMessageW(h_, WM_SETFONT, (WPARAM)hFont, TRUE);
				return h_;
			};

			createLabel(L"入力フォルダ:", 10, 15, 100, 20);
			g_hEditInput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L".\\input", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
				120, 12, 340, 22, hwnd, (HMENU)ID_EDIT_INPUT, nullptr, nullptr);
			SendMessageW(g_hEditInput, WM_SETFONT, (WPARAM)hFont, TRUE);
			HWND btnInBrowse = CreateWindowW(L"BUTTON", L"参照...", WS_CHILD | WS_VISIBLE,
				470, 11, 80, 24, hwnd, (HMENU)ID_BUTTON_INPUT_BROWSE, nullptr, nullptr);
			SendMessageW(btnInBrowse, WM_SETFONT, (WPARAM)hFont, TRUE);

			createLabel(L"出力フォルダ:", 10, 50, 100, 20);
			g_hEditOutput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L".\\output", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
				120, 47, 340, 22, hwnd, (HMENU)ID_EDIT_OUTPUT, nullptr, nullptr);
			SendMessageW(g_hEditOutput, WM_SETFONT, (WPARAM)hFont, TRUE);
			HWND btnOutBrowse = CreateWindowW(L"BUTTON", L"参照...", WS_CHILD | WS_VISIBLE,
				470, 46, 80, 24, hwnd, (HMENU)ID_BUTTON_OUTPUT_BROWSE, nullptr, nullptr);
			SendMessageW(btnOutBrowse, WM_SETFONT, (WPARAM)hFont, TRUE);

			HWND grp = CreateWindowW(L"BUTTON", L"変換モード", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
				10, 85, 540, 55, hwnd, nullptr, nullptr, nullptr);
			SendMessageW(grp, WM_SETFONT, (WPARAM)hFont, TRUE);

			g_hRadioIsekai = CreateWindowW(L"BUTTON", L"異世界の創造者", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
				25, 108, 150, 22, hwnd, (HMENU)ID_RADIO_ISEKAI, nullptr, nullptr);
			SendMessageW(g_hRadioIsekai, WM_SETFONT, (WPARAM)hFont, TRUE);

			g_hRadioBakin = CreateWindowW(L"BUTTON", L"RPG Developer Bakin", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
				185, 108, 180, 22, hwnd, (HMENU)ID_RADIO_BAKIN, nullptr, nullptr);
			SendMessageW(g_hRadioBakin, WM_SETFONT, (WPARAM)hFont, TRUE);

			g_hRadioIsekaiFace = CreateWindowW(L"BUTTON", L"異世界の創造者 フェイス", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
				375, 108, 165, 22, hwnd, (HMENU)ID_RADIO_ISEKAI_FACE, nullptr, nullptr);
			SendMessageW(g_hRadioIsekaiFace, WM_SETFONT, (WPARAM)hFont, TRUE);

			SendMessageW(g_hRadioIsekai, BM_SETCHECK, BST_CHECKED, 0);

			g_hButtonConvert = CreateWindowW(L"BUTTON", L"変換開始", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
				230, 150, 120, 32, hwnd, (HMENU)ID_BUTTON_CONVERT, nullptr, nullptr);
			SendMessageW(g_hButtonConvert, WM_SETFONT, (WPARAM)hFont, TRUE);

			g_hListLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", nullptr,
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOSEL,
				10, 195, 540, 180, hwnd, (HMENU)ID_LIST_LOG, nullptr, nullptr);
			SendMessageW(g_hListLog, WM_SETFONT, (WPARAM)hFont, TRUE);

			return 0;
		}
		case WM_COMMAND: {
			int id = LOWORD(wParam);
			if (id == ID_BUTTON_INPUT_BROWSE) {
				std::string dir = BrowseForFolder(hwnd, L"入力フォルダを選択してください");
				if (!dir.empty()) {
					SetWindowTextW(g_hEditInput, Utf8ToWide(dir).c_str());
				}
			}
			else if (id == ID_BUTTON_OUTPUT_BROWSE) {
				std::string dir = BrowseForFolder(hwnd, L"出力フォルダを選択してください");
				if (!dir.empty()) {
					SetWindowTextW(g_hEditOutput, Utf8ToWide(dir).c_str());
				}
			}
			else if (id == ID_BUTTON_CONVERT) {
				DoConvert(hwnd);
			}
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}

} // namespace

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
	INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES };
	InitCommonControlsEx(&icc);

	const wchar_t* CLASS_NAME = L"CharachipConverterWindow";

	WNDCLASSW wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	RegisterClassW(&wc);

	HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"CharachipConverter",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 580, 430,
		nullptr, nullptr, hInstance, nullptr);

	if (hwnd == nullptr) {
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg = { };
	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}
