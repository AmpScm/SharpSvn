// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <ws2tcpip.h>


// TODO: Patch in import sequence
#define in6_addr q_in6_addr
#define in6addr_any q_in6addr_any
#define in6addr_loopback q_in6addr_lookback
#define sockaddr_in6 q_sockaddr_in6

#include <isc/mem.h>
#include <isc/buffer.h>
#include <isc/timer.h>
#include <isc/socket.h>
#include <isc/task.h>
#include <isc/entropy.h>

#include <dns/dispatch.h>
#include <dns/message.h>
#include <dns/name.h>
#include <dns/request.h>
#include <dns/resolver.h>
#include <dns/rdataset.h>

#include <dns/view.h>
} // Missing in view.h

#include <vcclr.h>

#undef in6_addr
#undef in6addr_any
#undef in6addr_loopback
#undef sockaddr_in6



 

