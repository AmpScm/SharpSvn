using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.IO;

namespace SharpSvn.Bonjour
{
    public class SvnBonjourService
    {
        static int _started;
        static readonly object _lock = new object();
        static UdpClient _client;
        static UdpClient _clientV6;

        const string MDnsGroup = "224.0.0.251";
        const string MDnsGroupV6 = "FF02::FB";
        const short MDnsPort = 5353;

        public static void Start()
        {
            bool shouldStart;
            lock (_lock)
            {
                shouldStart = (_started == 0);
                _started++;
            }

            if (shouldStart)
                DoStart();
        }


        private static void DoStart()
        {
            _client = new UdpClient(MDnsPort, AddressFamily.InterNetwork);
            _clientV6 = new UdpClient(MDnsPort, AddressFamily.InterNetworkV6);

            _client.JoinMulticastGroup(IPAddress.Parse(MDnsGroup));
            _clientV6.JoinMulticastGroup(IPAddress.Parse(MDnsGroupV6));

            IPEndPoint from = new IPEndPoint(IPAddress.Any, 0);
            byte[] bytes = _client.Receive(ref from);

            LogBytes(bytes);
            Console.WriteLine("Received {0} bytes", bytes.Length);
            _client.BeginReceive(OnReceive, null);
            _clientV6.BeginReceive(OnReceiveV6, null);
        }

        private static void LogBytes(byte[] bytes)
        {
            string dir = "F:\\pkts";

            for (int i = 0; i < 1000; i++)
            {
                string fn = Path.Combine(dir, string.Format("pkt-{0}.dmp", i));

                if (!File.Exists(fn))
                {
                    File.WriteAllBytes(fn, bytes);
                    return;
                }
            }
        }

        static void OnReceive(IAsyncResult ar)
        {
            IPEndPoint from = new IPEndPoint(IPAddress.Any, 0);
            byte[] data = _client.EndReceive(ar, ref from);
            _client.BeginReceive(OnReceive, ar.AsyncState);

            LogBytes(data);

            Console.WriteLine("Received packet of {0} bytes", data.Length);
        }

        static void OnReceiveV6(IAsyncResult ar)
        {
            IPEndPoint from = new IPEndPoint(IPAddress.IPv6Any, 0);
            byte[] data = _clientV6.EndReceive(ar, ref from);
            _clientV6.BeginReceive(OnReceiveV6, ar.AsyncState);

            LogBytes(data);

            Console.WriteLine("Received v6 packet of {0} bytes", data.Length);
        }

    }
}
