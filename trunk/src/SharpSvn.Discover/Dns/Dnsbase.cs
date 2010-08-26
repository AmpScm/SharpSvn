using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace SharpSvn.Discover.Dns
{
    class DnsBase
    {
        protected string ReadName(BinaryReader br, byte[] fullPacket)
        {
            int len;

            StringBuilder sb = new StringBuilder();

            while (0 != (len = br.ReadByte()))
            {
                switch (len & (int)DnsLabel.LABEL_MASK)
                {
                    case (int)DnsLabel.Standard:
                        break;
                    case (int)DnsLabel.Compressed:
                        // Pointer to different location
                        int pos = ((len & (int)DnsLabel.LABEL_NOT_MASK) << 8) | br.ReadByte();
                        ReadPtrName(sb, fullPacket, pos);
                        return sb.ToString();
                    case (int)DnsLabel.Extended:
                    default:
                        throw new NotImplementedException();
                }

                byte[] txt = br.ReadBytes(len);
                string part = Encoding.UTF8.GetString(txt);

                sb.Append(part);
                sb.Append('.');
            }

            return sb.ToString();
        }

        private void ReadPtrName(StringBuilder sb, byte[] fullPacket, int pos)
        {
            byte[] map = new byte[fullPacket.Length];

            while (map[pos] == 0)
            {
                map[pos] = 1; // Avoid loops

                int len = fullPacket[pos];

                if (len == 0)
                    return;

                int lbl = len & (int)DnsLabel.LABEL_MASK;
                len = len & (int)DnsLabel.LABEL_NOT_MASK;

                switch (lbl)
                {
                    case (int)DnsLabel.Standard:
                        {
                            pos++;
                            byte[] nameBytes = new byte[len];
                            Array.Copy(fullPacket, pos, nameBytes, 0, len);

                            sb.Append(Encoding.UTF8.GetString(nameBytes));
                            sb.Append('.');
                        }
                        pos += len;
                        break;
                    case (int)DnsLabel.Compressed:
                        pos = (len << 8) | fullPacket[pos + 1];
                        break;
                    case (int)DnsLabel.Extended:
                    default:
                        throw new NotImplementedException();
                }
            }

            if (map[pos] != 0)
                throw new InvalidOperationException("Name loop detected");
        }
    }
}
