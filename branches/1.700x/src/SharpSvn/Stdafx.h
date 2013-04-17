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

#define SHARPSVN_INFO 1
#define _WIN32_WINNT 0x0500 // 2000 and higher
#pragma warning(push)
#pragma warning(disable: 4635)
#pragma warning(disable: 4634) // XML document comment: cannot be applied:  Discarding XML document comment for invalid target.
#include <apr-1/apr.h>
#include <apr-1/apr_strings.h>
#include <apr-1/apr_file_io.h>

#define SVN_DEPRECATED __declspec(deprecated)

#include <svn_client.h>
#include <svn_compat.h>
#include <svn_dirent_uri.h>
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
//#pragma warning(error: 4996) // Deprecated warnings
using System::Runtime::InteropServices::OutAttribute;
using System::Diagnostics::CodeAnalysis::SuppressMessageAttribute;
using System::Diagnostics::DebuggerStepThroughAttribute;
using System::Diagnostics::DebuggerDisplayAttribute;
#define SecurityPermissionAttribute(x,y) System::ComponentModel::DescriptionAttribute("Importing SharpSvnStrings")
#include "SharpSvnStrings.h"
#undef SecurityPermissionAttribute
#define SVN_THROW(expr)                         \
  do {                                          \
    svn_error_t *svn_err__temp = (expr);        \
    if (svn_err__temp)                          \
	  throw SvnException::Create(expr);         \
  } while (0)

#define SVN_HANDLE(expr)                                                  \
  do {                                                                    \
    svn_error_t *svn_err__temp = (expr);                                  \
	if (svn_err__temp)                                                    \
	  return this->CurrentCommandArgs->HandleResult(this, svn_err__temp); \
  } while (0)


#define DECLARE_EVENT_X(type, name, scope)			\
	scope:											\
		event EventHandler<type>^ name				\
		{											\
		scope:										\
[System::Runtime::CompilerServices::MethodImpl(		\
	System::Runtime::CompilerServices::				\
		MethodImplOptions::Synchronized)]			\
			void add(EventHandler<type>^ value)		\
			{										\
				event_##name += value;				\
			}										\
[System::Runtime::CompilerServices::MethodImpl(		\
	System::Runtime::CompilerServices::				\
		MethodImplOptions::Synchronized)]			\
			void remove(EventHandler<type>^ value)	\
			{										\
				event_##name -= value;				\
			}										\
		private:									\
			void raise(Object^ sender, type e)		\
			{										\
				EventHandler<type>^ ev_handler =	\
					event_##name;					\
				if (ev_handler)						\
					ev_handler(sender, e);			\
			}										\
		}											\
	private:										\
		EventHandler<type>^ event_##name;


#define DECLARE_EVENT(type, name) DECLARE_EVENT_X(type, name, public)

#pragma warning(disable: 4127) // warning C4127: conditional expression is constant
