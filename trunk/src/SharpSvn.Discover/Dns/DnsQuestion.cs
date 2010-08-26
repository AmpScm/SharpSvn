using System;
using System.Collections.Generic;
using System.Text;

namespace SharpSvn.Discover.Dns
{
    class DnsQuestion
    {
        readonly string _name;
        readonly DnsRecordType _type;
        readonly DnsRecordClass _class;
        readonly bool _unique;

        public DnsQuestion(string name, DnsRecordType type, DnsRecordClass cls, bool unique)
        {
            if (string.IsNullOrEmpty(name))
                throw new ArgumentNullException("name");

            _name = name;
            _type = type;
            _class = cls;
            _unique = unique;
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
            sb.Append(Type);
            sb.Append(' ');
            sb.Append(Class);
            sb.AppendLine(Unique ? " UNIQUE" : "");
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            ToString(sb);
            return sb.ToString().TrimEnd();
        }
    }
}
