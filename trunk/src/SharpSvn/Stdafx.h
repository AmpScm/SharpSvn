// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
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

#pragma once

#define _WIN32_WINNT 0x0500 // 2000 and higher
#pragma warning(push)
#pragma warning(disable: 4635)
#pragma warning(disable: 4634) // XML document comment: cannot be applied:  Discarding XML document comment for invalid target.
#include <apr-1/apr.h>
#include <apr-1/apr_strings.h>
#include <apr-1/apr_file_io.h>

#define SVN_DEPRECATED __declspec(deprecated)

#include <svn_client.h>
#include <svn_io.h>
#include <svn_path.h>
#include <svn_props.h>
#include <svn_repos.h>
#include <svn_time.h>

#ifndef UNUSED_ALWAYS
#define UNUSED_ALWAYS(x) (x)
#endif

#pragma warning(pop)

#include <vcclr.h>
#include <crtdbg.h>

#pragma warning(disable: 4706) // assignment within conditional expression
#pragma warning(error: 4996) // Deprecated warnings
using System::Runtime::InteropServices::OutAttribute;
using System::Diagnostics::CodeAnalysis::SuppressMessageAttribute;
using System::Diagnostics::DebuggerStepThroughAttribute;
using System::Diagnostics::DebuggerDisplayAttribute;
#include "SharpSvnStrings.h"
