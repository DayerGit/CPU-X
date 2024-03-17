#define _CRT_SECURE_NO_WARNINGS

#include "CPU-X.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int ActiveTab = TAB_CPU;
This_PC PC; BOOL LoadComplete = FALSE;

int getLen(wchar_t* str) {
	int i = 0;
	for (; str[i] != L'\0' && !(str[i] == ' ' && str[i + 1] == ' '); i++);
	return i;
}

wchar_t* strcut(wchar_t* str, wchar_t c) {
	wchar_t* tmp = malloc(sizeof(wchar_t));
	int i = 0;
	for (; str[i] != L'\0' && str[i] != c; tmp[i] = str[i], i++, tmp = realloc(tmp, (i + 1) * sizeof(wchar_t)));
	tmp[i] = L'\0';
	return tmp;
}

BOOL strcm(wchar_t* strOne, wchar_t* strTwo) {
	for (int i = 0; strOne[i] != L'\0' && strTwo[i] != '\0'; i++) if (strOne[i] != strTwo[i]) return 0;
	return 1;
}

LRESULT WINAPI CPUWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static LRESULT lRet;
	lRet = 0;
	DwmDefWindowProc(hwnd, msg, wparam, lparam, &lRet);

	switch (msg) {
	case WM_ACTIVATE: {
		MARGINS margins = { 0,0, 0, 400 };
		if (IsWindows11OrGreater()) DwmExtendFrameIntoClientArea(hwnd, &margins);

		const auto DWMSBT_AUTO = 0;
		const auto DWMSBT_DISABLE = 1; // Default
		const auto DWMSBT_MAINWINDOW = 2; // Mica
		const auto DWMSBT_TRANSIENTWINDOW = 3; // Acrylic
		const auto DWMSBT_TABBEDWINDOW = 4; // Tabbed
		DwmSetWindowAttribute(hwnd, (unsigned long)38, &DWMSBT_TABBEDWINDOW, sizeof(int));

		const auto DARK_DISABLE = 1;
		DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &DARK_DISABLE, sizeof(int));
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT PS; RECT TextRect;
		HBRUSH Brush = CreateSolidBrush(RGB(40, 40, 40));
		HDC WindowDC = BeginPaint(hwnd, &PS);
		FillRect(WindowDC, &PS.rcPaint, GetStockObject(BLACK_BRUSH));
		SelectObject(WindowDC, Brush);
		Rectangle(WindowDC, 4, TAB_SIZE_Y + 9, 396, 395);

		SetBkMode(WindowDC, TRANSPARENT); SetTextColor(WindowDC, RGB(255, 255, 255));
		static unsigned int NameLen, CacheL2Len, CacheL3Len;

		switch (ActiveTab) {
		case TAB_CPU: {
			NameLen = getLen(PC.cpu.Name.bstrVal);
			SelectObject(WindowDC, LoadFontFromFile(0, "Segoe UI Semibold", 400 / NameLen, 1000 / NameLen));
			TextRect = (RECT){ 15, 60, 385, 400 };
			DrawTextW(WindowDC, PC.cpu.Name.bstrVal, -1, &TextRect, DT_LEFT);

			SelectObject(WindowDC, GetFont(0, 0, 0, 0, 0, 0));
			TextRect.top += 1000 / NameLen + 5;
			DrawTextW(WindowDC, PC.cpu.NumOfCoreAndLogCore, -1, &TextRect, DT_RIGHT);
			DrawTextW(WindowDC, PC.cpu.Socket.bstrVal, -1, &TextRect, DT_LEFT);

			SelectObject(WindowDC, LoadFontFromFile(0, "Segoe UI Semibold", 400 / NameLen, 1000 / NameLen));
			SelectObject(WindowDC, CreateSolidBrush(RGB(50, 50, 50)));
			SelectObject(WindowDC, CreatePen(PS_SOLID, 1, RGB(70, 70, 70)));
			TextRect.top += 1500 / NameLen + 5;
			RoundRect(WindowDC, 5, TextRect.top, 395, TextRect.top + 80, 5, 5);
			DrawTextA(WindowDC, "Поддерживаемые инструкции\0", -1, &TextRect, DT_LEFT);
			TextRect.top += 1200 / NameLen + 5;
			SelectObject(WindowDC, GetFont(0, 0, 0, 0, 0, 0));
			DrawTextA(WindowDC, PC.cpu.InstructionSet, -1, &TextRect, DT_LEFT);

			CacheL2Len = getLen(PC.cpu.CacheL2Size);
			CacheL3Len = getLen(PC.cpu.CacheL3Size);
			TextRect.top += 2000 / NameLen + 5;
			RoundRect(WindowDC, 10, TextRect.top - 5, 220, TextRect.top + 60, 5, 5);
			RoundRect(WindowDC, 390 - (max(CacheL2Len, CacheL3Len) * 9), TextRect.top - 5, 390, TextRect.top + 60, 5, 5);
			DrawTextW(WindowDC, PC.cpu.BaseSpeed, -1, &TextRect, DT_LEFT);
			DrawTextW(WindowDC, PC.cpu.CacheL2Size, -1, &TextRect, DT_RIGHT);

			TextRect.top += 1000 / NameLen + 5;
			DrawTextW(WindowDC, PC.cpu.BusSpeed, -1, &TextRect, DT_LEFT);
			DrawTextW(WindowDC, PC.cpu.CacheL3Size, -1, &TextRect, DT_RIGHT);

			TextRect.top += 1500 / NameLen + 5;
			DrawTextA(WindowDC, PC.cpu.CPUDataSizeText, -1, &TextRect, DT_RIGHT);
			DrawTextW(WindowDC, PC.cpu.Stepping.bstrVal, -1, &TextRect, DT_LEFT);

			break;
		}
		case TAB_MAINBOARD: {
			NameLen = getLen(PC.motherboard.Model.bstrVal);
			SelectObject(WindowDC, LoadFontFromFile(0, "Segoe UI Semibold", 200 / NameLen, 500 / NameLen));
			TextRect = (RECT){ 15, 60, 385, 400 };
			DrawTextW(WindowDC, PC.motherboard.Model.bstrVal, -1, &TextRect, DT_LEFT);

			SelectObject(WindowDC, GetFont(0, 0, 0, 0, 0, 0));
			TextRect.top += 600 / NameLen + 5;
			DrawTextW(WindowDC, PC.motherboard.Manufactured.bstrVal, -1, &TextRect, DT_LEFT);

			SelectObject(WindowDC, LoadFontFromFile(0, "Segoe UI Semibold", 200 / NameLen, 500 / NameLen));
			SelectObject(WindowDC, CreateSolidBrush(RGB(50, 50, 50)));
			SelectObject(WindowDC, CreatePen(PS_SOLID, 1, RGB(70, 70, 70)));
			TextRect.top += 800 / NameLen + 5;
			RoundRect(WindowDC, 5, TextRect.top, 395, TextRect.top + (1500 / NameLen + 10), 5, 5);
			DrawTextA(WindowDC, "BIOS\0", -1, &TextRect, DT_LEFT);
			TextRect.top += 600 / NameLen + 5;
			SelectObject(WindowDC, GetFont(0, 0, 0, 0, 0, 0));
			DrawTextW(WindowDC, PC.motherboard.BIOSName.bstrVal, -1, &TextRect, DT_LEFT);
			TextRect.top += 500 / NameLen + 5;
			DrawTextW(WindowDC, PC.motherboard.BIOSVersion.bstrVal, -1, &TextRect, DT_LEFT);
			break;
		}
		case TAB_MEMORY: {
			SelectObject(WindowDC, LoadFontFromFile(0, "Segoe UI Semibold", 0, 40));
			TextRect = (RECT){ 15, 60, 385, 400 };
			DrawTextA(WindowDC, PC.mem.TypeText, -1, &TextRect, DT_LEFT);

			SelectObject(WindowDC, GetFont(0, 0, 700, 0, 0, 0));
			TextRect.top += 50;
			DrawTextA(WindowDC, PC.mem.size, -1, &TextRect, DT_LEFT);
			DrawTextA(WindowDC, PC.mem.SpeedText, -1, &TextRect, DT_RIGHT);
			break;
		}
		case TAB_VIDEOCONTROLLER: {
			NameLen = getLen(PC.gpu.Name.bstrVal);
			SelectObject(WindowDC, LoadFontFromFile(0, "Segoe UI Semibold", 200 / NameLen, 500 / NameLen));
			TextRect = (RECT){ 15, 60, 385, 400 };
			DrawTextW(WindowDC, PC.gpu.Name.bstrVal, -1, &TextRect, DT_LEFT);
			break;
		}
		}

		EndPaint(hwnd, &PS);
		DeleteObject(Brush);
		DeleteDC(WindowDC);
		break;
	}
	case WM_GETMINMAXINFO: {
		MINMAXINFO* pInfo = (MINMAXINFO*)lparam;
		pInfo->ptMinTrackSize.x = 400;
		pInfo->ptMinTrackSize.y = 400;
		pInfo->ptMaxTrackSize.x = 400;
		pInfo->ptMaxTrackSize.y = 400;
		break;
	}

	case WM_NCCALCSIZE: {
		NCCALCSIZE_PARAMS* pncsp = (NCCALCSIZE_PARAMS*)(lparam);
		break;
	}
	case WM_NCHITTEST: {
		if (lRet == 0) lRet = HitTestNCA(hwnd, wparam, lparam);
		break;
	}

	case WM_COMMAND: {
		if (TAB_CPU <= wparam && wparam <= TAB_VIDEOCONTROLLER) {
			ActiveTab = wparam; HWND ChildAfter = 0;
			for (int i = TAB_CPU; i <= TAB_VIDEOCONTROLLER + 1; i++) {
				ChildAfter = FindWindowExW(hwnd, ChildAfter, L"NewButton", 0);
				InvalidateRect(ChildAfter, 0, 0);
				InvalidateRect(hwnd, 0, 0);
			}
		}
		switch (wparam) {
		case BUTTON_CLOSE: PostQuitMessage(0);
		}
		break;
	}
	case WM_SHOWWINDOW: {
		if (GetForegroundWindow != hwnd) SetForegroundWindow(hwnd);
		break;
	}
	case WM_DESTROY: PostQuitMessage(0);
	default: return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
	return lRet;
}

LRESULT WINAPI ButtonWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static color = 0, block = 0;
	switch (msg) {
	case WM_PAINT: {
		static wchar_t WindowText[256];
		GetWindowTextW(hwnd, WindowText, 256);
		RECT WinRect;
		GetClientRect(hwnd, &WinRect);

		PAINTSTRUCT PS;
		HBRUSH Brush; HPEN Pen;
		if (ActiveTab == GetMenu(hwnd)) {
			Brush = CreateSolidBrush(RGB(40, 40, 40));
			Pen = CreatePen(PS_SOLID, 0, RGB(40, 40, 40));
		}
		else {
			Brush = CreateSolidBrush(color == 0 ? RGB(0, 0, 0) : RGB(10, 10, 10));
			Pen = CreatePen(PS_SOLID, 0, color == 0 ? RGB(0, 0, 0) : RGB(10, 10, 10));
		}

		HDC WindowDC = BeginPaint(hwnd, &PS);
		FillRect(WindowDC, &PS.rcPaint, GetStockObject(BLACK_BRUSH));
		SelectObject(WindowDC, Brush);
		SelectObject(WindowDC, Pen);

		RoundRect(WindowDC, PS.rcPaint.left, PS.rcPaint.top, PS.rcPaint.right, PS.rcPaint.bottom, 10, 10);
		Rectangle(WindowDC, PS.rcPaint.left, PS.rcPaint.top + 10, PS.rcPaint.right, PS.rcPaint.bottom);

		SetTextColor(WindowDC, RGB(255, 255, 255)); SetBkMode(WindowDC, TRANSPARENT);
		SelectObject(WindowDC, GetFont(WinRect.bottom / 2, 0, 100, 0, 0, 0));
		DrawTextW(WindowDC, WindowText, -1, &WinRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &PS);

		DeleteObject(Brush);
		DeleteObject(Pen);
		DeleteDC(WindowDC);
		break;
	}
	case WM_MOUSEMOVE: {
		if (block == 0) {
			color = 1;
			InvalidateRect(hwnd, 0, 0);
			TRACKMOUSEEVENT TME;
			TME.cbSize = sizeof(TME); TME.dwFlags = TME_LEAVE; TME.dwHoverTime = HOVER_DEFAULT; TME.hwndTrack = hwnd;
			TrackMouseEvent(&TME);
			block = 1;
		}
		break;
	}
	case WM_MOUSELEAVE: {
		color = 0; block = 0;
		InvalidateRect(hwnd, 0, 0);
		break;
	}
	case WM_LBUTTONDOWN: {
		color = 0;
		SendMessageW(GetParent(hwnd), WM_COMMAND, GetMenu(hwnd), 0);
		break;
	}
	default: return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
}

LRESULT WINAPI WaitWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	static unsigned int i = 0;
	switch (msg) {
	case WM_CREATE: {
		SetTimer(hwnd, 1, 150, 0);
		break;
	}
	case WM_ACTIVATE: {
		MARGINS margins = { 0,0,0, 200 };
		if (IsWindows11OrGreater()) DwmExtendFrameIntoClientArea(hwnd, &margins);

		const auto DWMSBT_MAINWINDOW = 2; // Mica
		DwmSetWindowAttribute(hwnd, (unsigned long)38, &DWMSBT_MAINWINDOW, sizeof(int));

		const auto DARK_DISABLE = 1;
		DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &DARK_DISABLE, sizeof(int));
		break;
	}
	case WM_NCCALCSIZE: {
		NCCALCSIZE_PARAMS* pncsp = (NCCALCSIZE_PARAMS*)(lparam);
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT PS;
		HDC WindowDC = BeginPaint(hwnd, &PS);
		FillRect(WindowDC, &PS.rcPaint, GetStockObject(BLACK_BRUSH));
		SetBkMode(WindowDC, TRANSPARENT); SetTextColor(WindowDC, RGB(255, 255, 255));
		SelectObject(WindowDC, LoadFontFromFile(L"./boot.ttf", "Segoe Boot Semilight", 0, 0));
		static wchar_t t[] = L"  Сканирую устройства\0";
		t[0] += i; i++;
		if (i > 16) {
			i = 0; t[0] = L'';
		}
		DrawTextW(WindowDC, t, -1, &PS.rcPaint, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &PS);
		DeleteDC(WindowDC);
		break;
	}
	case WM_TIMER: {
		InvalidateRect(hwnd, 0, 0);
		if (LoadComplete) PostQuitMessage(0);
		break;
	}
	default: return DefWindowProcW(hwnd, msg, wparam, lparam);
	}
}

void WaitWindow() {
	CreateWindowClassW(L"Wait", WaitWindowProc, RGB(0, 0, 0), IDC_ARROW, IDI_ICON1);
	RECT DeskWindow;
	GetWindowRect(GetDesktopWindow(), &DeskWindow);

	HWND Window = CreateWindowExW(0, L"Wait", L"Init", WS_OVERLAPPEDWINDOW, (DeskWindow.right - 300) / 2, (DeskWindow.bottom - 150) / 2, 300, 150, 0, 0, 0, 0);

	ShowWindow(Window, SW_SHOWNORMAL);

	MSG msg;
	while (GetMessageW(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	ExitThread(0);
}

int main(int argc, char* argv[]) {

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WaitWindow, 0, 0, 0);

	InitPC(&PC);
	int size = 1, count = 0;
	for (int i = PF_MMX_INSTRUCTIONS_AVAILABLE; i <= PF_AVX512F_INSTRUCTIONS_AVAILABLE; i++) {
		if (IsProcessorFeaturePresent(i)) {
			if (count == 20) {
				size++;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%s\n", PC.cpu.InstructionSet);
			}
			switch (i) {
			case PF_MMX_INSTRUCTIONS_AVAILABLE: {
				size += 6;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sMMX, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_XMMI_INSTRUCTIONS_AVAILABLE: {
				size += 7;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sSSE, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_3DNOW_INSTRUCTIONS_AVAILABLE: {
				size += 10;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%s3DNow!, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_XMMI64_INSTRUCTIONS_AVAILABLE: {
				size += 8;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sSSE2, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_SSE3_INSTRUCTIONS_AVAILABLE: {
				size += 8;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sSSE3, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_SSSE3_INSTRUCTIONS_AVAILABLE: {
				size += 9;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sSSSE3, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_SSE4_1_INSTRUCTIONS_AVAILABLE: {
				size += 10;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sSSE4.1, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_SSE4_2_INSTRUCTIONS_AVAILABLE: {
				size += 10;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sSSE4.2, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_AVX_INSTRUCTIONS_AVAILABLE: {
				size += 7;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sAVX, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_AVX2_INSTRUCTIONS_AVAILABLE: {
				size += 8;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sAVX2, \0", PC.cpu.InstructionSet);
				break;
			}
			case PF_AVX512F_INSTRUCTIONS_AVAILABLE: {
				size += 10;
				PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
				if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sAVX512, \0", PC.cpu.InstructionSet);
				break;
			}
			}
			count++;
		}
	}

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	IWbemLocator* pLoc = NULL;
	IWbemServices* pSvc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	BSTR inf;

	if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) return -1;
	if (FAILED(CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL))) return -1;
	if (FAILED(CoCreateInstance(&CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, &IID_IWbemLocator, (LPVOID*)&pLoc))) return -1;
	if (FAILED(pLoc->lpVtbl->ConnectServer(pLoc, L"ROOT\\CIMV2", NULL, NULL, 0, NULL, 0, 0, &pSvc))) return -1;
	if (FAILED(CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE))) return -1;

	pSvc->lpVtbl->ExecQuery(pSvc, L"WQL", L"SELECT * FROM Win32_Processor", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn);
	pclsObj->lpVtbl->Get(pclsObj, L"Name", 0, &PC.cpu.Name, 0, 0);
	wchar_t* test = strcut(PC.cpu.Name.bstrVal, L' ');
	if (IsProcessorFeaturePresent(PF_VIRT_FIRMWARE_ENABLED)) {
		if (strcm(test, L"AMD")) {
			size += 9;
			PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
			if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sAMD-V \0", PC.cpu.InstructionSet);
		}
		else if (strcm(test, L"Intel")) {
			size += 12;
			PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
			if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sIntel VT \0", PC.cpu.InstructionSet);
		}
		else {
			size += 18;
			PC.cpu.InstructionSet = realloc(PC.cpu.InstructionSet, size * sizeof(char));
			if (PC.cpu.InstructionSet != NULL) sprintf(PC.cpu.InstructionSet, "%sVirtualization \0", PC.cpu.InstructionSet);
		}
	}

	pclsObj->lpVtbl->Get(pclsObj, L"SocketDesignation", 0, &PC.cpu.Socket, 0, 0);
	inf = SysAllocString(L"Сокет ");
	VarBstrCat(inf, PC.cpu.Socket.bstrVal, &PC.cpu.Socket.bstrVal);
	SysFreeString(inf);

	pclsObj->lpVtbl->Get(pclsObj, L"CurrentClockSpeed", 0, &PC.cpu.BaseSpeedWMI, 0, 0);
	wsprintfW(&PC.cpu.BaseSpeed, L"Базовая частота: %i МГц\0", PC.cpu.BaseSpeedWMI.intVal);

	pclsObj->lpVtbl->Get(pclsObj, L"Stepping", 0, &PC.cpu.Stepping, 0, 0);
	inf = SysAllocString(L"Степпинг: ");
	VarBstrCat(inf, PC.cpu.Stepping.bstrVal, &PC.cpu.Stepping.bstrVal);
	SysFreeString(inf);

	pclsObj->lpVtbl->Get(pclsObj, L"NumberOfCores", 0, &PC.cpu.NumOfCoreWMI, 0, 0);
	pclsObj->lpVtbl->Get(pclsObj, L"NumberOfLogicalProcessors", 0, &PC.cpu.NumOfLogCoreWMI, 0, 0);
	wsprintfW(&PC.cpu.NumOfCoreAndLogCore, L"%i ядер / %i потоков\0", PC.cpu.NumOfCoreWMI.intVal, PC.cpu.NumOfLogCoreWMI.intVal);

	pclsObj->lpVtbl->Get(pclsObj, L"L2CacheSize", 0, &PC.cpu.CacheL2SizeWMI, 0, 0);
	if (PC.cpu.CacheL2SizeWMI.intVal > 1024) wsprintfW(&PC.cpu.CacheL2Size, L"Кэш L2: %i МБ\0", PC.cpu.CacheL2SizeWMI.intVal / 1024);
	else wsprintfW(&PC.cpu.CacheL2Size, L"Кэш L2: %i КБ\0", PC.cpu.CacheL2SizeWMI.intVal);


	pclsObj->lpVtbl->Get(pclsObj, L"L3CacheSize", 0, &PC.cpu.CacheL3SizeWMI, 0, 0);
	if (PC.cpu.CacheL3SizeWMI.intVal > 1024) wsprintfW(&PC.cpu.CacheL3Size, L"Кэш L3: %i МБ\0", PC.cpu.CacheL3SizeWMI.intVal / 1024);
	else wsprintfW(&PC.cpu.CacheL3Size, L"Кэш L3: %i МБ\0", PC.cpu.CacheL3SizeWMI.intVal);

	pclsObj->lpVtbl->Get(pclsObj, L"DataWidth", 0, &PC.cpu.DataSize, 0, 0);
	PC.cpu.CPUDataSizeText = malloc(10 * sizeof(char));
	if (PC.cpu.CPUDataSizeText != NULL) sprintf_s(PC.cpu.CPUDataSizeText, 10, "%i бит\0", PC.cpu.DataSize.intVal);

	pclsObj->lpVtbl->Get(pclsObj, L"ExtClock", 0, &PC.cpu.BusSpeedWMI, 0, 0);
	wsprintfW(&PC.cpu.BusSpeed, L"Частота шины:      %i МГц\0", PC.cpu.BusSpeedWMI.intVal);

	pSvc->lpVtbl->ExecQuery(pSvc, L"WQL", L"SELECT * FROM Win32_BIOS", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn);
	pclsObj->lpVtbl->Get(pclsObj, L"Manufacturer", 0, &PC.motherboard.BIOSName, 0, 0);
	inf = SysAllocString(L"Бренд: ");
	VarBstrCat(inf, PC.motherboard.BIOSName.bstrVal, &PC.motherboard.BIOSName.bstrVal);
	SysFreeString(inf);
	pclsObj->lpVtbl->Get(pclsObj, L"SMBIOSBIOSVersion", 0, &PC.motherboard.BIOSVersion, 0, 0);
	inf = SysAllocString(L"Версия: ");
	VarBstrCat(inf, PC.motherboard.BIOSVersion.bstrVal, &PC.motherboard.BIOSVersion.bstrVal);
	SysFreeString(inf);

	pSvc->lpVtbl->ExecQuery(pSvc, L"WQL", L"SELECT * FROM Win32_BaseBoard", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn);
	pclsObj->lpVtbl->Get(pclsObj, L"Manufacturer", 0, &PC.motherboard.Manufactured, 0, 0);
	inf = SysAllocString(L"Производитель: ");
	VarBstrCat(inf, PC.motherboard.Manufactured.bstrVal, &PC.motherboard.Manufactured.bstrVal);
	SysFreeString(inf);
	pclsObj->lpVtbl->Get(pclsObj, L"Product", 0, &PC.motherboard.Model, 0, 0);

	pSvc->lpVtbl->ExecQuery(pSvc, L"WQL", L"SELECT * FROM Win32_PhysicalMemory", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn);
	pclsObj->lpVtbl->Get(pclsObj, L"Speed", 0, &PC.mem.Speed, 0, 0);
	sprintf(&PC.mem.SpeedText, "Частота: %i МГц\0", PC.mem.Speed.intVal);
	pclsObj->lpVtbl->Get(pclsObj, L"SMBIOSMemoryType", 0, &PC.mem.Type, 0, 0);

	if (PC.mem.Type.intVal == 26) sprintf_s(&PC.mem.TypeText, 10, "DDR 4\0");
	else if (PC.mem.Type.intVal == 24) sprintf_s(&PC.mem.TypeText, 10, "DDR 3\0");
	else if (21 <= PC.mem.Type.intVal && 23 >= PC.mem.Type.intVal) sprintf_s(&PC.mem.TypeText, 10, "DDR 2\0");
	else if (PC.mem.Type.intVal == 20) sprintf_s(&PC.mem.TypeText, 10, "DDR\0");
	else if (PC.mem.Type.intVal == 17) sprintf_s(&PC.mem.TypeText, 10, "SDRAM\0");
	else sprintf_s(&PC.mem.TypeText, 10, "Unknown\0");

	if ((statex.ullTotalPhys / 1024 / 1024) < 1024) sprintf(&PC.mem.size, "Объём: %llu МБ\0", statex.ullTotalPhys / 1024 / 1024);
	else sprintf(&PC.mem.size, "Объём: %llu ГБ\0", statex.ullTotalPhys / 1024 / 1024 / 1000);

	pSvc->lpVtbl->ExecQuery(pSvc, L"WQL", L"SELECT * FROM Win32_VideoController", WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
	pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn);
	pclsObj->lpVtbl->Get(pclsObj, L"Name", 0, &PC.gpu.Name, 0, 0);
	//pclsObj->lpVtbl->Get(pclsObj, L"AdapterRAM", 0, &PC.gpu.RAM, 0, 0);
	//wprintf(L"%llu\n", PC.gpu.RAM.ullVal/1024/1024);

	CreateWindowClassW(L"CPU-X", CPUWindowProc, RGB(0, 0, 0), IDC_ARROW, IDI_ICON1);
	CreateWindowClassW(L"NewButton", ButtonWindowProc, RGB(0, 0, 0), IDC_ARROW, NULL);

	RECT DeskWindow;
	GetWindowRect(GetDesktopWindow(), &DeskWindow);

	HWND Window = CreateWindowExW(0, L"CPU-X", L"CPU-X", WS_OVERLAPPEDWINDOW, (DeskWindow.right - 400) / 2, (DeskWindow.bottom - 400) / 2, 400, 400, 0, 0, 0, 0);
	HWND Close = CreateWindowExW(0, L"newButton", L"✕", WS_CHILD, 360, 0, 40, 40, Window, BUTTON_CLOSE, 0, 0);

	HWND CPU_TAB = CreateWindowExW(0, L"newButton", L"ЦП", WS_CHILD, 5, 10, 60, TAB_SIZE_Y, Window, TAB_CPU, 0, 0);
	HWND MAINBOARD_TAB = CreateWindowExW(0, L"newButton", L"Плата", WS_CHILD, 68, 10, 100, TAB_SIZE_Y, Window, TAB_MAINBOARD, 0, 0);
	HWND MEMORY_TAB = CreateWindowExW(0, L"newButton", L"Память", WS_CHILD, 171, 10, 80, TAB_SIZE_Y, Window, TAB_MEMORY, 0, 0);
	HWND VIDEOCONTROLLER_TAB = CreateWindowExW(0, L"newButton", L"Видеокарта", WS_CHILD, 254, 10, 100, TAB_SIZE_Y, Window, TAB_VIDEOCONTROLLER, 0, 0);

	ShowAllWindow(SW_SHOWNORMAL, Window, Close, CPU_TAB, MAINBOARD_TAB, MEMORY_TAB, VIDEOCONTROLLER_TAB, 0);
	LoadComplete = TRUE;

	MSG message;
	while (GetMessageW(&message, 0, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
	return 0;
}