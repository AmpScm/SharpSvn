// This is the main DLL file.

#include "stdafx.h"
#include "DnsAll.h"

#include <isc/app.h>
#include <isc/hash.h>

using namespace SharpDns;
using namespace SharpDns::Implementation;

static SharpDnsBase::SharpDnsBase()
{
	InitializeResolver();

	if (!isc_net_probeipv4())
		m_bHaveIpv4 = true;

	if (!isc_net_probeipv6())
		m_bHaveIpv6 = true;

	isc_mem_t *pMem = nullptr;
	if(!isc_mem_create(0, 0, &pMem) && pMem)
		g_pMem = pMem;

	isc_entropy_t *pEnt = nullptr;
	if(!isc_entropy_create(pMem, &pEnt))
		g_pEnt = pEnt;

	isc_hash_create(pMem, pEnt, 255);
	isc_hash_init();
}



static bool _initialized = false;

void SharpDnsBase::InitializeResolver()
{
	// Class resolver. Is locked by the framework
	if(_initialized)
		return;
}