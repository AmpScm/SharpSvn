// SharpSvn-Plink.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SharpSvn-Plink.h"

extern "C" {

	extern int main(int argc, char** argv);
};

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


extern "C" void sPlinkConfirm();

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	int result;
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	_set_printf_count_output(true); // Required for disconnect message and HTTP Proxy of plink

	return main(__argc, __argv);
}

