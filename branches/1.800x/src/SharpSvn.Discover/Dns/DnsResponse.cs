using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;
using System.Diagnostics;

namespace SharpSvn.Discover.Dns
{
    class DnsResponse : DnsBase
    {
        readonly IPAddress _origin;
        readonly IList<DnsQuestion> _questions;
        readonly IList<DnsRecord> _answers;
        readonly IList<DnsRecord> _authorities;
        readonly IList<DnsRecord> _additional;

        public DnsResponse(IPAddress origin, byte[] payload)
            : this(origin, new MemoryStream(payload, false), payload)
        {
        }

        DnsResponse(IPAddress origin, Stream payload, byte[] fullPacket)
        {
            _origin = origin;
            using (BinaryReader br = new BinaryReader(payload))
            {
                ushort id = unchecked((ushort)IPAddress.NetworkToHostOrder(br.ReadInt16()));
                ushort flags = unchecked((ushort)IPAddress.NetworkToHostOrder(br.ReadInt16()));
                int nQuestions = IPAddress.NetworkToHostOrder(br.ReadInt16());
                int nAnswers = IPAddress.NetworkToHostOrder(br.ReadInt16());
                int nAuthorities = IPAddress.NetworkToHostOrder(br.ReadInt16());
                int nAdditional = IPAddress.NetworkToHostOrder(br.ReadInt16());

                if (nQuestions > 0)
                {
                    List<DnsQuestion> questions = new List<DnsQuestion>(nQuestions);
                    for (int i = 0; i < nQuestions; i++)
                        questions.Add(ReadQuestion(br, fullPacket));

                    _questions = questions.AsReadOnly();
                }

                if (nAnswers > 0)
                {
                    List<DnsRecord> answers = new List<DnsRecord>(nAnswers);
                    for (int i = 0; i < nAnswers; i++)
                        answers.Add(ReadAnswer(br, fullPacket));

                    _answers = answers.AsReadOnly();
                }

                if (nAuthorities > 0)
                {
                    List<DnsRecord> authorities = new List<DnsRecord>(nAuthorities);
                    for (int i = 0; i < nAuthorities; i++)
                        authorities.Add(ReadAnswer(br, fullPacket));

                    _authorities = authorities.AsReadOnly();
                }

                if (nAdditional > 0)
                {
                    List<DnsRecord> additional = new List<DnsRecord>(nAdditional);
                    for (int i = 0; i < nAdditional; i++)
                        additional.Add(ReadAnswer(br, fullPacket));

                    _additional = additional.AsReadOnly();
                }
            }
        }

        public IList<DnsQuestion> Questions
        {
            get { return _questions; }
        }

        public IList<DnsRecord> Answers
        {
            get { return _answers; }
        }

        public IList<DnsRecord> Authorities
        {
            get { return _authorities; }
        }

        public IList<DnsRecord> Additional
        {
            get { return _additional; }
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            if (Questions != null)
            {
                sb.AppendLine("== Questions ==");

                foreach (DnsQuestion q in Questions)
                    q.ToString(sb);
            }

            if (Answers != null)
            {
                sb.AppendLine("== Answers ==");

                foreach (DnsRecord a in Answers)
                    a.ToString(sb);
            }

            if (Authorities != null)
            {
                sb.AppendLine("== Authorities ==");

                foreach (DnsRecord a in Authorities)
                    a.ToString(sb);
            }

            return sb.ToString();
        }      

        private DnsQuestion ReadQuestion(BinaryReader br, byte[] fullPacket)
        {
            string name = ReadName(br, fullPacket);
            DnsRecordType type = (DnsRecordType)IPAddress.NetworkToHostOrder(br.ReadInt16());
            DnsRecordClass cls = (DnsRecordClass)IPAddress.NetworkToHostOrder(br.ReadInt16());
            bool unique = (cls & DnsRecordClass.CLASS_UNIQUE) != 0;
            cls = cls & DnsRecordClass.CLASS_MASK;

            return new DnsQuestion(name, type, cls, unique);
        }

        private DnsRecord ReadAnswer(BinaryReader br, byte[] fullPacket)
        {
            string name = ReadName(br, fullPacket);
            DnsRecordType type = (DnsRecordType)IPAddress.NetworkToHostOrder(br.ReadInt16());
            DnsRecordClass cls = (DnsRecordClass)IPAddress.NetworkToHostOrder(br.ReadInt16());
            bool unique = (cls & DnsRecordClass.CLASS_UNIQUE) != 0;
            cls = cls & DnsRecordClass.CLASS_MASK;

            int ttl = IPAddress.NetworkToHostOrder(br.ReadInt32());
            int len = unchecked((ushort)IPAddress.NetworkToHostOrder(br.ReadInt16()));

            byte[] body = br.ReadBytes(len);

            return DnsRecord.Create(name, type, cls, unique, body, fullPacket);
        }

        


        static void TestExisting()
        {
            foreach (FileInfo f in new DirectoryInfo("F:\\pkts").GetFiles("*.dmp"))
            {
                DnsResponse rs = new DnsResponse(IPAddress.Any, File.ReadAllBytes(f.FullName));

                Console.WriteLine(rs.ToString());
                Debug.WriteLine(rs.ToString());
            }
        }
    }
}
