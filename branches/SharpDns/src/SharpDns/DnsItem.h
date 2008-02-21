// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details
#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


namespace SharpDns 
{
	public enum class DnsDataClass
	{
		Reserved =dns_rdataclass_reserved0,
		In = dns_rdataclass_in,
		Chaos = dns_rdataclass_chaos,
		Hs = dns_rdataclass_hs,
		None = dns_rdataclass_none,
		Any = dns_rdataclass_any,		
	};

	public enum class DnsDataType
	{
		// See http://www.dns.net/dnsrd/rr.html
		None = dns_rdatatype_none,
		A = dns_rdatatype_a,
		Ns = dns_rdatatype_ns,
		CName = dns_rdatatype_cname,
		Soa = dns_rdatatype_soa,
		AAAA = dns_rdatatype_aaaa,
		Any = dns_rdatatype_any,
		Ptr = dns_rdatatype_ptr,
		Mx = dns_rdatatype_mx,
		Txt = dns_rdatatype_txt,
		Srv = dns_rdatatype_srv,

		// Never used these
		Wks = dns_rdatatype_wks,
		Rp = dns_rdatatype_rp,
		Sig = dns_rdatatype_sig,
		Key = dns_rdatatype_key,
		Loc = dns_rdatatype_loc,
		Naptr = dns_rdatatype_naptr,

		// Special
		Unspecified = dns_rdatatype_unspec,
		Ixfr = dns_rdatatype_ixfr,
		Axfr = dns_rdatatype_axfr,


		// Signatures
		TKey = dns_rdatatype_tkey,
		TSig = dns_rdatatype_tsig,


		// Deprecated
		Md = dns_rdatatype_md,
		Mf = dns_rdatatype_mf,
		Mb = dns_rdatatype_mb,
		Mg = dns_rdatatype_mg,
		Mr = dns_rdatatype_mr,
		Null = dns_rdatatype_null,
		HInfo = dns_rdatatype_hinfo,
		MInfo = dns_rdatatype_minfo,
		AfsDb = dns_rdatatype_afsdb,
		X25 = dns_rdatatype_x25,
		Isdn = dns_rdatatype_isdn,
		Rt = dns_rdatatype_rt,
		Nsap = dns_rdatatype_nsap,
		NsapPtr = dns_rdatatype_nsap_ptr,
		Px = dns_rdatatype_px,
		Gpos = dns_rdatatype_gpos,	
		Nxt = dns_rdatatype_nxt,
		Spf = dns_rdatatype_spf,
			
		Kx = dns_rdatatype_kx,
		Cert = dns_rdatatype_cert,
		A6 = dns_rdatatype_a6,
		DName = dns_rdatatype_dname,
		Opt = dns_rdatatype_opt,
		Apl = dns_rdatatype_apl,
		Ds = dns_rdatatype_ds,
		Sshfp = dns_rdatatype_sshfp,
		IpSecKey = dns_rdatatype_ipseckey,
		RrSig = dns_rdatatype_rrsig,
		NSec = dns_rdatatype_nsec,
		DnsKey = dns_rdatatype_dnskey,
		Dlv = dns_rdatatype_dlv,

		

		MailB = dns_rdatatype_mailb,
		MailA = dns_rdatatype_maila,
	};

	public ref class DnsItem sealed
	{
		initonly String^ _name;
		initonly String^ _value;
		initonly String^ _suffix;
		initonly DnsDataClass _class;
		initonly DnsDataType _type;
		initonly int _ttl;

	public:
		DnsItem(String^ name, DnsDataClass dataClass, DnsDataType dataType, int ttl, String^ value)
		{
			if(!name)
				throw gcnew ArgumentNullException("name");
			else if(!value)
				throw gcnew ArgumentNullException("value");
			else if(ttl < 0)
				throw gcnew ArgumentOutOfRangeException("ttl");

			_name = name;
			_value = value;
			_class = dataClass;
			_type = dataType;
			_ttl = ttl;
			_suffix = name;
		}

		DnsItem(String^ name, DnsDataClass dataClass, DnsDataType dataType, int ttl, String^ value, String^ suffix)
		{
			if(!name)
				throw gcnew ArgumentNullException("name");
			else if(!value)
				throw gcnew ArgumentNullException("value");
			else if(ttl < 0)
				throw gcnew ArgumentOutOfRangeException("ttl");

			_name = name;
			_value = value;
			_class = dataClass;
			_type = dataType;
			_ttl = ttl;
			_suffix = suffix;
		}

	public:
		property String^ Name
		{
			String^ get()
			{
				return _name;
			}
		}

		property String^ Value
		{
			String^ get()
			{
				return _value;
			}
		}

		property String^ FullValue
		{
			String^ get()
			{
				switch(DataType)
				{
				case DnsDataType::Mx:
				case DnsDataType::CName:
					if(_suffix && !Value->EndsWith("."))
						return Value + "." + _suffix;
					else
						return Value;
				default:
					return Value;
				}
			}
		}

		property DnsDataClass DataClass
		{
			DnsDataClass get()
			{
				return _class;
			}
		}

		property DnsDataType DataType
		{
			DnsDataType get()
			{
				return _type;
			}
		}

		property TimeSpan Ttl
		{
			TimeSpan get()
			{
				return TimeSpan(0, 0, _ttl);
			}
		}

		property int TtlSecs
		{
			int get()
			{
				return _ttl;
			}
		}

		virtual String^ ToString() override
		{
			return Name;
		}
	};
};