using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;

namespace SharpSvn.Discover.Dns
{
    class DnsRecord : DnsBase
    {
        readonly string _name;
        readonly DnsRecordType _type;
        readonly DnsRecordClass _class;
        readonly bool _unique;
        byte[] _body;

        public DnsRecord(string name, DnsRecordType type, DnsRecordClass cls, bool unique, byte[] body)
        {
            if (string.IsNullOrEmpty(name))
                throw new ArgumentNullException("name");

            _name = name;
            _type = type;
            _class = cls;
            _unique = unique;
            _body = body;
        }

        public string Name
        {
            get { return _name; }
        }

        public DnsRecordType Type
        {
            get { return _type; }
        }

        public DnsRecordClass Class
        {
            get { return _class; }
        }

        public bool Unique
        {
            get { return _unique; }
        }

        internal void ToString(StringBuilder sb)
        {
            sb.Append(Name);
            sb.Append(' ');
            sb.Append(Class);
            sb.Append(' ');
            sb.Append(Type);
            sb.Append(' ');
            //sb.Append(Unique ? " UNIQUE " : " ");

            ToStringDetails(sb);
            sb.AppendLine();
        }

        protected virtual void ToStringDetails(StringBuilder sb)
        {
            sb.AppendFormat(" ({0} bytes)", _body.Length);
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            ToString(sb);
            return sb.ToString().TrimEnd();
        }


        internal static DnsRecord Create(string name, DnsRecordType type, DnsRecordClass cls, bool unique, byte[] body, byte[] fullPacket)
        {
            switch(type)
            {
                case DnsRecordType.A:
                case DnsRecordType.AAAA:
                    return new DnsRecordAddress(name, type, cls, unique, body);

                case DnsRecordType.TXT:
                    return new DnsRecordText(name, type, cls, unique, body);

                case DnsRecordType.CNAME:
                case DnsRecordType.PTR:
                    return new DnsRecordAlias(name, type, cls, unique, body, fullPacket);

                case DnsRecordType.SRV:
                    return new DnsRecordServer(name, type, cls, unique, body, fullPacket);

                default:
                    return new DnsRecord(name, type, cls, unique, body);
            }
        }

        sealed class DnsRecordAddress : DnsRecord
        {
            readonly IPAddress _addr;
            public DnsRecordAddress(string name, DnsRecordType type, DnsRecordClass cls, bool unique, byte[] body)
                : base(name, type, cls, unique, body)
            {
                if (type == DnsRecordType.A && body.Length == 4)
                    _addr = new IPAddress(body);
                else if (type == DnsRecordType.AAAA && body.Length == 16)
                    _addr = new IPAddress(body);
                else
                    throw new InvalidOperationException();
            }

            protected override void ToStringDetails(StringBuilder sb)
            {
                sb.Append(_addr.ToString());
            }
        }

        sealed class DnsRecordText : DnsRecord
        {
            readonly string _text;

            public DnsRecordText(string name, DnsRecordType type, DnsRecordClass cls, bool unique, byte[] body)
                : base(name, type, cls, unique, body)
            {
                _text = Encoding.UTF8.GetString(body);
            }

            protected override void ToStringDetails(StringBuilder sb)
            {
                sb.AppendFormat("\"{0}\"", _text);
            }
        }

        sealed class DnsRecordAlias : DnsRecord
        {
            readonly string _alias;

            public DnsRecordAlias(string name, DnsRecordType type, DnsRecordClass cls, bool unique, byte[] body, byte[] fullPacket)
                : base(name, type, cls, unique, body)
            {
                using(MemoryStream ms = new MemoryStream(body, false))
                using (BinaryReader br = new BinaryReader(ms))
                {
                    _alias = ReadName(br, fullPacket);
                }
            }

            protected override void ToStringDetails(StringBuilder sb)
            {
                sb.Append(_alias);
            }
        }

        sealed class DnsRecordServer : DnsRecord
        {
            readonly int _prio;
            readonly int _weight;
            readonly int _port;
            readonly string _name;

            public DnsRecordServer(string name, DnsRecordType type, DnsRecordClass cls, bool unique, byte[] body, byte[] fullPacket)
                : base(name, type, cls, unique, body)
            {
                using (MemoryStream ms = new MemoryStream(body, false))
                using (BinaryReader br = new BinaryReader(ms))
                {
                    _prio = unchecked((ushort)IPAddress.NetworkToHostOrder(br.ReadInt16()));
                    _weight = unchecked((ushort)IPAddress.NetworkToHostOrder(br.ReadInt16()));
                    _port = unchecked((ushort)IPAddress.NetworkToHostOrder(br.ReadInt16()));
                    _name = ReadName(br, fullPacket);
                }
            }

            protected override void ToStringDetails(StringBuilder sb)
            {
                sb.Append(_prio);
                sb.Append(' ');
                sb.Append(_weight);
                sb.Append(' ');
                sb.Append(_port);
                sb.Append(' ');
                sb.Append(_name);
            }
        }
    }
}
