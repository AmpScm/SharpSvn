using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace SharpSvn.Discover.Dns
{
    abstract class DnsEntry : IComparable<DnsEntry>
    {
        // private static Logger logger = Logger.getLogger(DNSEntry.class.getName());
        readonly String _key;

        readonly String _name;

        readonly DnsRecordType _type;

        readonly DnsRecordClass _dnsClass;

        readonly bool _unique;

        readonly Dictionary<Fields, String> _qualifiedNameMap;

        /**
         * Create an entry.
         */
        protected DnsEntry(String name, DnsRecordType type, DnsRecordClass recordClass, bool unique)
        {
            _name = name;
            // _key = (name != null ? name.trim().toLowerCase() : null);
            _type = type;
            _dnsClass = recordClass;
            _unique = unique;
            _qualifiedNameMap = Impl.DecodeQualifiedNameMapForType(this.Name);
            String domain = _qualifiedNameMap[Fields.Domain];
            String protocol = _qualifiedNameMap[Fields.Protocol];
            String application = _qualifiedNameMap[Fields.Application];
            String instance = _qualifiedNameMap[Fields.Instance].ToLowerInvariant();
            _key = (instance.Length > 0 ? instance + "." : "") + (application.Length > 0 ? "_" + application + "." : "") + (protocol.Length > 0 ? "_" + protocol + "." : "") + domain + ".";
        }

        /*
         * (non-Javadoc)
         *
         * @see java.lang.Object#equals(java.lang.Object)
         */
        public override bool Equals(object obj)
        {
            bool result = false;
            DnsEntry entry = obj as DnsEntry;
            if (entry != null)
            {
                if (GetHashCode() == entry.GetHashCode())
                    result =
                        RecordType == entry.RecordType
                        && RecordClass == entry.RecordClass;
            }
            return result;
        }

        /**
         * Check if two entries have exactly the same name, type, and class.
         *
         * @param entry
         *
         * @return <code>true</code> if the two entries have are for the same record, <code>false</code> otherwise
         */
        public bool IsSameEntry(DnsEntry entry)
        {
            return
                string.Equals(Key, entry.Key, StringComparison.OrdinalIgnoreCase)
                && RecordType == entry.RecordType
                && (entry.RecordClass == DnsRecordClass.Any
                    || entry.RecordClass == RecordClass);
        }

        public bool SameSubtype(DnsEntry other)
        {
            return SubType == other.SubType;
        }

        /**
         * Returns the subtype of this entry
         *
         * @return subtype of this entry
         */
        public String SubType
        {
            get
            {
                String subtype = _qualifiedNameMap[Fields.Subtype];
                return (subtype != null ? subtype : "");
            }
        }

        /**
         * Returns the name of this entry
         *
         * @return name of this entry
         */
        public String Name
        {
            get { return _name ?? ""; }
        }

        /**
         * Returns the key for this entry. The key is the lower case name.
         *
         * @return key for this entry
         */
        public String Key
        {
            get { return _key ?? ""; }
        }

        /**
         * @return record type
         */
        public DnsRecordType RecordType
        {
            get { return _type; }
        }

        /**
         * @return record class
         */
        public DnsRecordClass RecordClass
        {
            get { return _dnsClass; }
        }

        /**
         * @return true if unique
         */
        public bool Unique
        {
            get { return _unique; }
        }

        public IDictionary<Fields, String> getQualifiedNameMap()
        {
            return new Dictionary<Fields, String>(_qualifiedNameMap);
        }

        public bool IsServicesDiscoveryMetaQuery()
        {
            return _qualifiedNameMap[Fields.Application] == "dns-sd"
                && _qualifiedNameMap[Fields.Instance] == "_services";
        }

        public bool IsDomainDiscoveryQuery()
        {
            // b._dns-sd._udp.<domain>.
            // db._dns-sd._udp.<domain>.
            // r._dns-sd._udp.<domain>.
            // dr._dns-sd._udp.<domain>.
            // lb._dns-sd._udp.<domain>.

            if (_qualifiedNameMap[Fields.Application] == "dns-sd")
            {
                String name = _qualifiedNameMap[Fields.Instance];
                return name == "b" || name == "db" || name == "r" || name == "dr" || name == "lb";
            }
            return false;
        }

        public bool IsReverseLookup()
        {
            return _qualifiedNameMap[Fields.Domain].EndsWith(".in-addr.arpa") || _qualifiedNameMap[Fields.Domain].EndsWith(".ip6.arpa");
        }

        /**
         * Check if the record is stale, i.e. it has outlived more than half of its TTL.
         *
         * @param now
         *            update date
         * @return <code>true</code> is the record is stale, <code>false</code> otherwise.
         */
        public abstract bool IsStale(long now);

        /**
         * Check if the record is expired.
         *
         * @param now
         *            update date
         * @return <code>true</code> is the record is expired, <code>false</code> otherwise.
         */
        public abstract bool IsExpired(DateTime now);

        /**
         * Check that 2 entries are of the same class.
         *
         * @param entry
         * @return <code>true</code> is the two class are the same, <code>false</code> otherwise.
         */
        public bool IsSameRecordClass(DnsEntry entry)
        {
            return (entry != null) && (entry.RecordClass == this.RecordClass);
        }

        /**
         * Check that 2 entries are of the same type.
         *
         * @param entry
         * @return <code>true</code> is the two type are the same, <code>false</code> otherwise.
         */
        public bool IsSameType(DnsEntry entry)
        {
            return (entry != null) && (entry.RecordType == RecordType);
        }

        /**
         * @param dout
         * @throws IOException
         */
        protected void ToByteArray(BinaryWriter dout)
        {
            dout.Write(Encoding.UTF8.GetBytes(Name));
            dout.Write((short)RecordType);
            dout.Write((short)RecordClass);
        }

        /**
         * Creates a byte array representation of this record. This is needed for tie-break tests according to draft-cheshire-dnsext-multicastdns-04.txt chapter 9.2.
         *
         * @return byte array representation
         */
        protected byte[] ToByteArray()
        {
            using (MemoryStream ms = new MemoryStream())
            using (BinaryWriter bw = new BinaryWriter(ms))
            {
                ToByteArray(bw);
                bw.Flush();

                return ms.ToArray();
            }
        }

        /**
         * Does a lexicographic comparison of the byte array representation of this record and that record. This is needed for tie-break tests according to draft-cheshire-dnsext-multicastdns-04.txt chapter 9.2.
         *
         * @param that
         * @return a negative integer, zero, or a positive integer as this object is less than, equal to, or greater than the specified object.
         */
        public int CompareTo(DnsEntry that)
        {
            byte[] thisBytes = this.ToByteArray();
            byte[] thatBytes = that.ToByteArray();

            for (int i = 0, n = Math.Min(thisBytes.Length, thatBytes.Length); i < n; i++)
            {
                int nn = thisBytes[i] - thisBytes[i];

                if (nn != 0)
                    return nn;
            }

            return thisBytes.Length - thatBytes.Length;
        }

        /**
         * Overriden, to return a value which is consistent with the value returned by equals(Object).
         */
        public override int GetHashCode()
        {
            return Name.GetHashCode() + RecordType.GetHashCode() + RecordClass.GetHashCode();
        }

        /*
         * (non-Javadoc)
         *
         * @see java.lang.Object#toString()
         */
        public override String ToString()
        {
            StringBuilder aLog = new StringBuilder();
            aLog.Append("[" + GetType().Name + "@" + GetHashCode());
            aLog.Append(" type: " + this.RecordType);
            aLog.Append(", class: " + this.RecordClass);
            aLog.Append((Unique ? "-unique," : ","));
            aLog.Append(" name: " + Name);
            this.ToString(aLog);
            aLog.Append("]");
            return aLog.ToString();
        }

        protected virtual void ToString(StringBuilder aLog)
        {
            // Stub
        }
    }
}
