#include <stdio.h>
#include <Windows.h>
#include <Wbemidl.h>
#include <dwmapi.h>
#include <Powrprof.h>
#include <VersionHelpers.h>
#include "resource.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "PowrProf.lib")

#define BUTTON_CLOSE 1

#define TAB_CPU 2
#define TAB_MAINBOARD 3
#define TAB_MEMORY 4
#define TAB_VIDEOCONTROLLER 5

#define TAB_SIZE_Y 35

typedef struct _CPU {
	VARIANT Name, Socket, Stepping, BaseSpeedWMI, BusSpeedWMI, NumOfCoreWMI, NumOfLogCoreWMI, CacheL2SizeWMI, CacheL3SizeWMI, DataSize;
	char* CPUDataSizeText, * InstructionSet;
	wchar_t BaseSpeed[30], BusSpeed[30], NumOfCoreAndLogCore[60], CacheL2Size[30], CacheL3Size[30];
} CPU;

typedef struct _MainBoard {
	VARIANT BIOSName, BIOSVersion, Manufactured, Model;
} MainBoard;

typedef struct _Memory {
	VARIANT Speed, Type;
	char size[30], SpeedText[30], TypeText[10];
} Memory;

typedef struct _GPU {
	VARIANT Name, RAM;
} GPU;

typedef struct _PC {
	CPU cpu;
	MainBoard motherboard;
	Memory mem;
	GPU gpu;
} This_PC;


WNDCLASSW CreateWindowClassW(LPWSTR Name, LRESULT ClassProc, COLORREF BackgroundColor, LPCWSTR Cursor, LPWSTR Icon);
void ShowAllWindow(int Style, HWND Windows, ...);
LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam);
HFONT GetFont(int Height, int Width, int Weight, int italic, int underline, int strike);
HFONT LoadFontFromFile(wchar_t* File, const char* FontName, int SizeX, int SizeY);

void InitPC(This_PC* PC);

VERSIONHELPERAPI IsWindows11OrGreater() {
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	DWORDLONG const dwlConditionMask = VerSetConditionMask(VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL), VER_BUILDNUMBER, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = HIBYTE(_WIN32_WINNT_WIN10);
	osvi.dwMinorVersion = LOBYTE(_WIN32_WINNT_WIN10);
	osvi.dwBuildNumber = 22000;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, dwlConditionMask) != FALSE;
}