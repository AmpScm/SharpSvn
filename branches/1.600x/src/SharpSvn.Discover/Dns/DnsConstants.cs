using System;
using System.Collections.Generic;
using System.Text;

namespace SharpSvn.Discover.Dns
{
    static class DnsConstants
    {
        // http://www.iana.org/assignments/dns-parameters

        // changed to final class - jeffs
        const String MDNS_GROUP = "224.0.0.251";
        const String MDNS_GROUP_IPV6 = "FF02::FB";
        const int MDNS_PORT = 5353;
        const int DNS_PORT = 53;
        const int DNS_TTL = 60 * 60; // default one hour TTL
        // const int DNS_TTL = 120 * 60; // two hour TTL (draft-cheshire-dnsext-multicastdns.txt ch 13)

        const int MAX_MSG_TYPICAL = 1460;
        const int MAX_MSG_ABSOLUTE = 8972;

        const int FLAGS_QR_MASK = 0x8000; // Query response mask
        const int FLAGS_QR_QUERY = 0x0000; // Query
        const int FLAGS_QR_RESPONSE = 0x8000; // Response

        const int FLAGS_AA = 0x0400; // Authorative answer
        const int FLAGS_TC = 0x0200; // Truncated
        const int FLAGS_RD = 0x0100; // Recursion desired
        const int FLAGS_RA = 0x8000; // Recursion available

        const int FLAGS_Z = 0x0040; // Zero
        const int FLAGS_AD = 0x0020; // Authentic data
        const int FLAGS_CD = 0x0010; // Checking disabled

        // Time Intervals for various functions

        const int SHARED_QUERY_TIME = 20; // milliseconds before send shared query
        const int QUERY_WAIT_INTERVAL = 225; // milliseconds between query loops.
        const int PROBE_WAIT_INTERVAL = 250; // milliseconds between probe loops.
        const int RESPONSE_MIN_WAIT_INTERVAL = 20; // minimal wait interval for response.
        const int RESPONSE_MAX_WAIT_INTERVAL = 115; // maximal wait interval for response
        const int PROBE_CONFLICT_INTERVAL = 1000; // milliseconds to wait after conflict.
        const int PROBE_THROTTLE_COUNT = 10; // After x tries go 1 time a sec. on probes.
        const int PROBE_THROTTLE_COUNT_INTERVAL = 5000; // We only increment the throttle count, if the previous increment is inside this interval.
        const int ANNOUNCE_WAIT_INTERVAL = 1000; // milliseconds between Announce loops.
        const int RECORD_REAPER_INTERVAL = 10000; // milliseconds between cache cleanups.
        const int KNOWN_ANSWER_TTL = 120;
        const int ANNOUNCED_RENEWAL_TTL_INTERVAL = DNS_TTL * 500; // 50% of the TTL in milliseconds

        const long CLOSE_TIMEOUT = ANNOUNCE_WAIT_INTERVAL * 5L;
        const long SERVICE_INFO_TIMEOUT = ANNOUNCE_WAIT_INTERVAL * 6L;
    }

    [Flags]
    enum DnsLabel : byte
    {
        Standard = 0,
        Extended = 0x40,
        Unknown = 0x80,
        Compressed = 0xC0,

        LABEL_MASK = 0xC0,
        LABEL_NOT_MASK = 0x3F,
    }

    enum DnsQuery : byte
    {
        /// <summary>
        /// * Query [RFC1035]
        /// </summary>
        Query = 0,

        /// <summary>
        /// * IQuery (Inverse Query, Obsolete) [RFC3425]
        /// </summary>
        InverseQuery = 1,

        /// <summary>
        /// Status [RFC1035]
        /// </summary>
        Status = 2,


        /// <summary>
        /// Unassigned
        /// </summary>
        Unassigned3 = 3,

        /// <summary>
        /// * Notify [RFC1996]
        /// </summary>
        Notify = 4,

        /// <summary>
        /// * Update [RFC2136]
        /// </summary>
        Update = 5,
    }

    enum DnsOptionCode
    {
        None = 0,

        /// <summary>
        /// * Long-Lived Queries Option [http://files.dns-sd.org/draft-sekar-dns-llq.txt]
        /// </summary>
        LLQ = 1,

        /// <summary>
        /// * Update Leases Option [http://files.dns-sd.org/draft-sekar-dns-ul.txt]
        /// </summary>
        UL = 2,

        /// <summary>
        /// * Name Server Identifier Option [RFC5001]
        /// </summary>
        NSID = 3,

        /// <summary>
        /// * Owner Option [draft-cheshire-edns0-owner-option]
        /// </summary>
        Owner = 4,
    }

    enum DnsRecordClass : ushort
    {
        /// <summary>
        /// ?
        /// </summary>
        Unknown = 0,

        /// <summary>
        /// in
        /// </summary>
        IN = 1,

        /// <summary>
        /// cs
        /// </summary>
        CS = 2,

        /// <summary>
        /// ch
        /// </summary>
        Chaos = 3,

        /// <summary>
        /// hs
        /// </summary>
        Hesiod = 4,


        /// <summary>
        /// Used in DNS UPDATE [RFC 2136]
        /// </summary>
        None = 254,

        /// <summary>
        /// * Not a DNS class, but a DNS query class, meaning "all classes"
        /// </summary>
        Any = 255,

        /**
         * Multicast DNS uses the bottom 15 bits to identify the record class...
         */
        CLASS_MASK = 0x7FFF,

        /**
         * ... and the top bit indicates that all other cached records are now invalid
         */
        CLASS_UNIQUE = 0x8000,
    }

    enum DnsRecordType
    {
        /**
     * Address
     */
        Ignore = 0,
        /**
         * Address
         */
        A = 1,
        /**
         * Name Server
         */
        NS = 2,
        /**
         * Mail Destination
         */
        MD = 3,
        /**
         * Mail Forwarder
         */
        MF = 4,
        /**
         * Canonical Name
         */
        CNAME = 5,
        /**
         * Start of Authority
         */
        SOA = 6,
        /**
         * Mailbox
         */
        MB = 7,
        /**
         * Mail Group
         */
        MG = 8,
        /**
         * Mail Rename
         */
        MR = 9,
        /**
         * NULL RR
         */
        NULL = 10,
        /**
         * Well-known-service
         */
        WKS = 11,
        /**
         * Domain Name pointer
         */
        PTR = 12,
        /**
         * Host information
         */
        HINFO = 13,
        /**
         * Mailbox information
         */
        MINFO = 14,
        /**
         * Mail exchanger
         */
        MX = 15,
        /**
         * Arbitrary text string
         */
        TXT = 16,
        /**
         * for Responsible Person [RFC1183]
         */
        RP = 17,
        /**
         * for AFS Data Base location [RFC1183]
         */
        AFSDB = 18,
        /**
         * for X.25 PSDN address [RFC1183]
         */
        X25 = 19,
        /**
         * for ISDN address [RFC1183]
         */
        ISDN = 20,
        /**
         * for Route Through [RFC1183]
         */
        RT = 21,
        /**
         * for NSAP address, NSAP style A record [RFC1706]
         */
        NSAP = 22,
        /**
         *
         */
        NSAP_OTR = 23,
        /**
         * for security signature [RFC2931]
         */
        SIG = 24,
        /**
         * for security key [RFC2535]
         */
        KEY = 25,
        /**
         * X.400 mail mapping information [RFC2163]
         */
        PX = 26,
        /**
         * Geographical Position [RFC1712]
         */
        GPOS = 27,
        /**
         * IP6 Address [Thomson]
         */
        AAAA = 28,
        /**
         * Location Information [Vixie]
         */
        LOC = 29,
        /**
         * Next Domain - OBSOLETE [RFC2535, RFC3755]
         */
        NXT = 30,
        /**
         * Endpoint Identifier [Patton]
         */
        EID = 31,
        /**
         * Nimrod Locator [Patton]
         */
        NIMLOC = 32,
        /**
         * Server Selection [RFC2782]
         */
        SRV = 33,
        /**
         * ATM Address [Dobrowski]
         */
        ATM = 34,
        /**
         * Naming Authority Pointer [RFC2168, RFC2915]
         */
        NAPTR = 35,
        /**
         * Key Exchanger [RFC2230]
         */
        KX = 36,
        /**
         * CERT [RFC2538]
         */
        CERT = 37,
        /**
         * A6 [RFC2874]
         */
        A6 = 38,
        /**
         * DNAME [RFC2672]
         */
        DNAME = 39,
        /**
         * SINK [Eastlake]
         */
        SINK = 40,
        /**
         * OPT [RFC2671]
         */
        OPT = 41,
        /**
         * APL [RFC3123]
         */
        APL = 42,
        /**
         * Delegation Signer [RFC3658]
         */
        DS = 43,
        /**
         * SSH Key Fingerprint [RFC-ietf-secsh-dns-05.txt]
         */
        SSHFP = 44,
        /**
         * RRSIG [RFC3755]
         */
        RRSIG = 46,
        /**
         * NSEC [RFC3755]
         */
        NSEC = 47,
        /**
         * DNSKEY [RFC3755]
         */
        DNSKEY = 48,
        /**
         * [IANA-Reserved]
         */
        UINFO = 100,
        /**
         * [IANA-Reserved]
         */
        UID = 101,
        /**
         * [IANA-Reserved]
         */
        GID = 102,
        /**
         * [IANA-Reserved]
         */
        UNSPEC = 103,
        /**
         * Transaction Key [RFC2930]
         */
        TKEY = 249,
        /**
         * Transaction Signature [RFC2845]
         */
        TSIG = 250,
        /**
         * Incremental transfer [RFC1995]
         */
        IXFR = 251,
        /**
         * Transfer of an entire zone [RFC1035]
         */
        AXFR = 252,
        /**
         * Mailbox-related records (MB, MG or MR) [RFC1035]
         */
        MAILA = 253,
        /**
         * Mail agent RRs (Obsolete - see MX) [RFC1035]
         */
        MAILB = 254,
        /**
         * Request for all records [RFC1035]
         */
        ANY = 255,
    }

    public enum Fields
    {
        Domain, Protocol, Application, Instance, Subtype
    }
}
