// SharpSvn-Plink.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SharpPlink.h"
#include <stdio.h>

extern "C" {

	extern int main(int argc, char** argv);
};


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	_set_printf_count_output(true); // Required for disconnect message and HTTP Proxy of plink

	return main(__argc, __argv);
}

