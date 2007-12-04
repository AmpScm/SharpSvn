#include "stdafx.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Resources;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("SharpDns")];
[assembly:AssemblyDescriptionAttribute("")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("SharpSvn Project")];
[assembly:AssemblyProductAttribute("SharpDns")];
[assembly:AssemblyCopyrightAttribute("Copyright © SharpSvn Project 2007")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly:AssemblyVersionAttribute("1.1.*")];
[assembly:NeutralResourcesLanguageAttribute("en")];

[assembly:ComVisible(false)];
[assembly:CLSCompliantAttribute(true)];
[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
[assembly:RuntimeCompatibility(WrapNonExceptionThrows = true)];

#pragma comment(lib, "libdns.lib")
#pragma comment(lib, "libisc.lib")

#pragma comment(lib, "libeay32.lib")

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
