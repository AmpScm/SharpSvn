using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Whos.Shared.Client.LiveID;
using System.Security;
using System.Net;
using System.Xml;
using System.Xml.Serialization;
using System.IO;

namespace WerRegister
{
    class Program
    {
        class Args
        {
            public string UserName;
            public string Password;

            public readonly List<string> Files = new List<string>();
        }

        static int Main(string[] args)
        {
            Args a = Parse(args);
            if (a == null)
                return 1;

            return Run(a);
        }

        static Args Parse(string[] args)
        {
            Args aa = new Args();
            int i;

            for (i = 0; i < args.Length; i++)
            {
                string key = args[i];
                if (!string.IsNullOrEmpty(key) && key[0] == '-')
                {
                    if (args[i] == "--")
                    {
                        i++;
                        break;
                    }

                    bool argAvailable = i + 1 < args.Length;

                    string value = argAvailable ? args[i + 1] : null;
                    switch (key)
                    {
                        case "--username":
                            if (!argAvailable)
                                return ArgError("--username requires argument");
                            aa.UserName = value;
                            i++;
                            break;
                        case "--password":
                            if (!argAvailable)
                                return ArgError("--password requires argument");
                            aa.Password = value;
                            i++;
                            break;

                        default:
                            return ArgError(string.Format("Unknown argument: {0}", value));
                    }
                }
                else
                    aa.Files.Add(key);
            }

            while (i < args.Length)
                aa.Files.Add(args[i++]);

            return aa;
        }

        private static int Run(Args args)
        {
            LiveIdAuthentication auth = new LiveIdAuthentication("", Guid.Empty, typeof(Program).Assembly.FullName, "winqual.microsoft.com", "MBI_SSL", true);

            SecureString ss = new SecureString();

            foreach (char c in args.Password)
                ss.AppendChar(c);

            if (!auth.Authenticate(args.UserName, ss))
            {
                Console.Error.WriteLine("Error: Authentication failed");
                return 1;
            }

            string uploadUrl = "https://winqual.microsoft.com/services/wer/user/fileupload.aspx";
            string challenge = LiveIdAuthentication.GetChallengeFromServiceRequest(uploadUrl);
            CookieContainer container = new CookieContainer();
            foreach (string file in args.Files)
            {
                WebClient wc = new WebClient();
                LiveIdAuthentication.AddAuthorizationHeaderToRequestHeaders(wc.Headers, auth.GetResponseForHttpChallenge(challenge));

                byte[] response = wc.UploadFile(uploadUrl, file);

                AtomFeed feed = ParseUploadResponse(response);

                if (feed.Status != "ok")
                {
                    if (feed.Entries.Length > 0)
                    {
                        foreach (FeedEntry entry in feed.Entries)
                            Console.Error.WriteLine(entry.Title);
                    }

                    return 1;
                }
            }

            return 0;
        }

        static AtomFeed ParseUploadResponse(byte[] response)
        {
            using (MemoryStream stream = new MemoryStream(response))
            {
                XmlSerializer s = new XmlSerializer(typeof(AtomFeed));
                return (AtomFeed)s.Deserialize(stream);
            }
        }

        static private Args ArgError(string error)
        {
            Console.Error.WriteLine("Error: {0}", error);
            return null;
        }
    }

    [XmlRoot("feed", Namespace = "http://www.w3.org/2005/Atom")]
    public class AtomFeed
    {
        [XmlAttribute("status", Namespace = "http://schemas.microsoft.com/windowserrorreporting")]
        public string Status;
        [XmlElement("entry")]
        public FeedEntry[] Entries;
    }
    public class FeedEntry
    {
        [XmlElement("title")]
        public string Title;
    }
}
