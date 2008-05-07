using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;
using System.Xml;
using System.Web;
using QQn.TurtleUtils.IO;

namespace CollabUpload
{
    public class Program
    {
        class Args
        {
            public string Site;
            public int Folder;

            public string UserName;
            public string Password;
            public string Name;
            public string Status;
            public string Description;
            public readonly List<string> Files = new List<string>();
        }

        static void Main(string[] args)
        {
            Args a = Parse(args);
            if (a != null)
                Run(a);
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
                        case "--site":
                            if (!argAvailable)
                                return ArgError("--host requires argument");
                            aa.Site = value.Contains(".") ? value : string.Format("{0}.open.collab.net", value);
                            i++;
                            break;
                        case "--folder":
                            if (!argAvailable || !int.TryParse(value, out aa.Folder))
                                return ArgError("--folder requires integer argument");
                            i++;
                            break;
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
                        case "--name":
                            if (!argAvailable)
                                return ArgError("--name requires argument");
                            aa.Name = value;
                            i++;
                            break;
                        case "--description":
                            if (!argAvailable)
                                return ArgError("--description requires argument");
                            aa.Description = value;
                            i++;
                            break;
                        case "--status":
                            if (!argAvailable)
                                return ArgError("--status requires argument");
                            aa.Status = value;
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

        static private Args ArgError(string error)
        {
            Console.Error.WriteLine("Error: {0}", error);
            return null;
        }

        static Args Run(Args args)
        {
            CookieContainer cookBox = new CookieContainer();

            string requestUri = string.Format("http://{0}/servlets/ProjectDocumentAdd?folderID={1}", args.Site, args.Folder);
            HttpWebRequest wr = (HttpWebRequest)WebRequest.Create(requestUri);
            wr.UserAgent = "Mozilla/8.0 (compatible; Collab Uploader C#)";
            wr.CookieContainer = cookBox;
            string responseUri;
            string text;

            using (WebResponse response = wr.GetResponse())
            {
                responseUri = response.ResponseUri.ToString();
                text = GetText(response);
            }

            if (responseUri != requestUri && responseUri.Contains("/Login"))
            {
                // Standard Collabnet login on secondary host..
                XmlDocument doc = new XmlDocument();
                doc.LoadXml(text);

                XmlNamespaceManager nsmgr = new XmlNamespaceManager(doc.NameTable);
                nsmgr.AddNamespace("html", "http://www.w3.org/1999/xhtml");
                XmlElement form = (XmlElement)doc.SelectSingleNode("//html:form[@id='loginform']", nsmgr);
                StringBuilder sb = new StringBuilder();

                wr = (HttpWebRequest)WebRequest.Create(new Uri(new Uri(responseUri), new Uri(form.GetAttribute("action"))));
                wr.UserAgent = "Mozilla/8.0 (compatible; Collab Uploader C#)";
                wr.CookieContainer = cookBox;
                using(WebFormWriter wfw = new WebFormWriter(wr, WebRequestPostDataEncoding.WwwFormUrlEncoded, "POST"))
                {
                    foreach (XmlElement el in form.SelectNodes(".//html:input[@name]", nsmgr))
                    {
                        string name = el.GetAttribute("name");

                        switch (name.ToUpperInvariant())
                        {
                            case "LOGINID":
                                wfw.AddValue(name, args.UserName);
                                break;
                            case "PASSWORD":
                                wfw.AddValue(name, args.Password);
                                break;
                            default:
                                string v = el.GetAttribute("value");
                                wfw.AddValue(name, v ?? "");
                                break;
                        }
                    }
                }

                using (WebResponse response = wr.GetResponse())
                {
                    responseUri = response.ResponseUri.ToString();
                    text = GetText(response);
                }

                if (responseUri != requestUri)
                    return ArgError("Invalid credentials specified");
            }

            // Ok, we are logged in; let's upload the files

            foreach (string file in args.Files)
            {
                Console.WriteLine("Uploading {0}", Path.GetFileName(file));
                wr = (HttpWebRequest)WebRequest.Create(requestUri + "&action=Add%20document");
                wr.UserAgent = "Mozilla/8.0 (compatible; Collab Uploader C#)";
                wr.CookieContainer = cookBox;
                wr.Timeout = 1800 * 1000; // Half an hour should be enough 
                using (WebFormWriter wfw = new WebFormWriter(wr, WebRequestPostDataEncoding.MultipartFormData, "POST"))
                {
                    wfw.AddValue("name", args.Name ?? Path.GetFileName(file));
                    wfw.AddValue("status", args.Status ?? "Draft");
                    wfw.AddValue("description", args.Description ?? "Description");
                    wfw.AddValue("initiallylocked", "");
                    wfw.AddValue("type", "file");                    
                    wfw.AddValue("textFormat", "pre");
                    wfw.AddFile("file", file);                    
                    wfw.AddValue("docUrl", "");
                    wfw.AddValue("submit", "submit");
                    wfw.AddValue("maxDepth", "");
                }

                using (WebResponse response = wr.GetResponse())
                {
                    text = GetText(response);

                    if (text.Contains("\"error"))
                    {
                        Console.WriteLine("Upload failed");
                        return null;
                    }
                }
            }
            return null;
        }

        static string GetText(WebResponse response)
        {
            using (StreamReader sr = new StreamReader(response.GetResponseStream()))
            {
                return sr.ReadToEnd();
            }
        }
    }
}
