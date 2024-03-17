#define _CRT_SECURE_NO_WARNINGS

#include "CPU-X.h"

WNDCLASSW CreateWindowClassW(LPWSTR Name, LRESULT ClassProc, COLORREF BackgroundColor, LPCWSTR Cursor, LPWSTR Icon) {
	WNDCLASSW wcl;
	memset(&wcl, 0, sizeof(wcl));

	wcl.lpszClassName = Name;
	wcl.lpfnWndProc = ClassProc;
	wcl.hbrBackground = CreateSolidBrush(BackgroundColor);
	wcl.hCursor = LoadCursorW(0, Cursor);
	wcl.cbWndExtra = 0;
	wcl.hIcon = LoadIconW(GetModuleHandle(0), MAKEINTRESOURCEW(Icon));

	RegisterClassW(&wcl);

	return wcl;
}

void ShowAllWindow(int Style, HWND Windows, ...) {
	HWND(*Window) = &Windows;
	while (*Window) {
		ShowWindow(*Window, Style);
		Window++;
	}
}

LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	RECT rcWindow;
	GetWindowRect(hWnd, &rcWindow);

	static MouseY;
	MouseY = HIWORD(lParam);

	USHORT uRow = 1;

	if (MouseY >= rcWindow.top && MouseY < rcWindow.top + 40) uRow = 0;

	return uRow == 1 ? HTNOWHERE : HTCAPTION;
}

HFONT GetFont(int Height, int Width, int Weight, int italic, int underline, int strike) {
	return CreateFontW(Height, Width, 0, 0, Weight, italic, underline, strike, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL);
}

HFONT LoadFontFromFile(wchar_t* File, const char* FontName, int SizeX, int SizeY) {
	if (File != NULL) AddFontResourceW(File);

	LOGFONTA lf;
	memset(&lf, 0, sizeof(LOGFONTA));

	lf.lfHeight = SizeY;
	lf.lfWidth = SizeX;
	lf.lfCharSet = DEFAULT_CHARSET;
	strcpy(lf.lfFaceName, FontName);

	return CreateFontIndirectA(&lf);
}

void InitPC(This_PC* PC) {
	VariantInit(&PC->cpu.Name);
	VariantInit(&PC->cpu.Socket);
	VariantInit(&PC->cpu.BaseSpeedWMI);
	VariantInit(&PC->cpu.BusSpeedWMI);
	VariantInit(&PC->cpu.Stepping);
	VariantInit(&PC->cpu.NumOfCoreWMI);
	VariantInit(&PC->cpu.NumOfLogCoreWMI);
	VariantInit(&PC->cpu.CacheL2SizeWMI);
	VariantInit(&PC->cpu.CacheL3SizeWMI);
	VariantInit(&PC->cpu.DataSize);

	VariantInit(&PC->motherboard.BIOSName);
	VariantInit(&PC->motherboard.BIOSVersion);
	VariantInit(&PC->motherboard.Manufactured);
	VariantInit(&PC->motherboard.Model);

	VariantInit(&PC->mem.Speed);
	/*VariantInit(&PC->mem.Manufacture);
	VariantInit(&PC->mem.PartNum);
	VariantInit(&PC->mem.SerialNum);*/
	VariantInit(&PC->mem.Type);

	VariantInit(&PC->gpu.Name);
	VariantInit(&PC->gpu.RAM);

	PC->cpu.InstructionSet = malloc(sizeof(char));
	if(PC->cpu.InstructionSet != NULL) PC->cpu.InstructionSet[0] = '\0';
}
