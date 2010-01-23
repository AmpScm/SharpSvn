// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
	int nNwArgv = 0;
	char** pNwArgv;
	
	_set_printf_count_output(true); // Required for disconnect message and HTTP Proxy of plink

	// Subversion 1.7+ will pass -q to ssh.. just ignore this
	pNwArgv = (char**)malloc(sizeof(char**) * __argc);

	pNwArgv[nNwArgv++] = __argv[0];

	for(int i = 1; i < __argc; i++)
	{
		if (strcmp(__argv[i], "-q"))
			pNwArgv[nNwArgv++] = __argv[i];
	}

	return main(nNwArgv, pNwArgv);
}

