using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.IO;
using System.Xml;
using System.Web;
using QQn.TurtleUtils.IO;
using System.Text.RegularExpressions;
using System.Globalization;

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

            public string Result;
            public int MaxUploads;
            public TimeSpan Keep;

            public readonly List<string> Files = new List<string>();
        }

        class Document
        {
            public Uri DownloadUri;
            public string Name;
            public string Who;
            public string Status;
            public DateTime Date;
            public Uri ReserveUri;
            public string Description;
            public Uri EditUri;
            public int FileId;
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
                        case "--result":
                            if (!argAvailable)
                                return ArgError("--result requires argument");
                            aa.Result = value;
                            i++;
                            break;
                        case "--max-uploads":
                            if (!argAvailable || !int.TryParse(value, out aa.MaxUploads))
                                return ArgError("--max-uploads requires integer argument");
                            i++;
                            break;
                        case "--keep":
                            if (!argAvailable || !TimeSpan.TryParse(value, out aa.Keep))
                                return ArgError("--keep requires timespan argument DD.hh:mm");
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

        const string UserAgentName = "Mozilla/8.0 (compatible; Collab Uploader C#)";

        static Args Run(Args args)
        {
            CookieContainer cookBox = new CookieContainer();

            string requestUri = string.Format("http://{0}/servlets/ProjectDocumentAdd?folderID={1}", args.Site, args.Folder);
            HttpWebRequest wr = (HttpWebRequest)WebRequest.Create(requestUri);
            wr.UserAgent = UserAgentName;
            wr.CookieContainer = cookBox;
            string responseUri;
            string text;

            using (WebResponse response = wr.GetResponse())
            {
                responseUri = response.ResponseUri.ToString();
                text = GetText(response);
            }

            MaybeLogin(args, cookBox, requestUri, responseUri, ref text);

            // Ok, we are logged in; let's upload the files

            if (!string.IsNullOrEmpty(args.Result) && File.Exists(args.Result))
                File.Delete(args.Result);

            foreach (string file in args.Files)
            {
                Console.WriteLine("Uploading {0}", Path.GetFileName(file));
                wr = (HttpWebRequest)WebRequest.Create(requestUri + "&action=Add%20document");
                wr.UserAgent = UserAgentName;
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

                    // This page is currently not valid Xml; use regular expressions instead
                    Regex re = new Regex("\\<a[^>]*href=\"(?<url>[^\"]*" + Regex.Escape(Path.GetFileName(file)) + ")\"[^>]*\\>"
                        + "\\s*" + Regex.Escape(args.Name ?? Path.GetFileName(file)) + "\\s*</a>");

                    Match m = re.Match(text);

                    if (m.Success)
                    {
                        if (!string.IsNullOrEmpty(args.Result))
                            File.AppendAllText(args.Result, new Uri(new Uri(requestUri), m.Groups["url"].Value).AbsoluteUri + Environment.NewLine);
                    }
                }
            }

            if (args.MaxUploads > 0 || args.Keep != TimeSpan.Zero)
            {
                List<Document> docs = GetDocumentList(args, cookBox);

                RemoveOnStatus(args, docs); // Filter all not interesting documents (status, locks)
                SortDocs(docs);

                if (args.MaxUploads > 0 && docs.Count > args.MaxUploads)
                {
                    int from = docs.Count - args.MaxUploads;
                    docs.RemoveRange(from, docs.Count - from);
                }

                if (args.Keep > TimeSpan.Zero)
                {
                    FilterOnDate(docs, DateTime.Now.Date - args.Keep);
                }                

                GC.KeepAlive(docs);
                foreach (Document doc in docs)
                {
                    Console.WriteLine("Deleting {0}", doc.Name);

                    DeleteFile(args, cookBox, doc.FileId);
                }
            }
            return null;
        }

        private static void RemoveOnStatus(Args args, List<Document> docs)
        {
            string filterStatus = args.Status ?? "Draft";
            docs.RemoveAll(delegate(Document d)
            {
                if (d.ReserveUri == null)
                    return true; // Document is reserved                    
                else if (!string.Equals(filterStatus, d.Status, StringComparison.OrdinalIgnoreCase))
                    return true; // Status doesn't match

                return false;
            });                
        }

        private static void SortDocs(List<Document> docs)
        {
            docs.Sort(
                delegate(Document d1, Document d2)
                {
                    return StringComparer.OrdinalIgnoreCase.Compare(d1.Name, d2.Name);
                });
        }

        private static void FilterOnDate(List<Document> docs, DateTime keepAfter)
        {
            docs.RemoveAll(
                delegate(Document d)
                {
                    if (d.Date != DateTime.MinValue && d.Date >= keepAfter)
                        return true;
                    else
                        return false;
                });
        }

        private static void MaybeLogin(Args args, CookieContainer cookBox, string requestUri, string responseUri, ref string text)
        {
            HttpWebRequest wr;
            if (responseUri != requestUri && responseUri.Contains("/Login"))
            {
                // Standard Collabnet login on secondary host..
                XmlDocument doc = new XmlDocument();

                XmlReaderSettings xs = new XmlReaderSettings();
                xs.ValidationFlags = System.Xml.Schema.XmlSchemaValidationFlags.None;
                xs.ValidationType = ValidationType.None;
                xs.ProhibitDtd = false;

                int nS = text.IndexOf("<html");

                if (nS > 0)
                    text = text.Substring(nS).Replace("&nbsp;", "&#160;").Replace("&copy;", "&#169;");

                doc.Load(XmlReader.Create(new StringReader(text), xs));

                XmlNamespaceManager nsmgr = new XmlNamespaceManager(doc.NameTable);
                nsmgr.AddNamespace("html", "http://www.w3.org/1999/xhtml");
                XmlElement form = (XmlElement)doc.SelectSingleNode("//html:form[@id='loginform']", nsmgr);
                StringBuilder sb = new StringBuilder();

                wr = (HttpWebRequest)WebRequest.Create(new Uri(new Uri(responseUri), new Uri(form.GetAttribute("action"))));
                wr.UserAgent = UserAgentName;
                wr.CookieContainer = cookBox;
                using (WebFormWriter wfw = new WebFormWriter(wr, WebRequestPostDataEncoding.WwwFormUrlEncoded, "POST"))
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
                {
                    throw new InvalidOperationException("Invalid credentials specified");
                }
            }
        }

        private static List<Document> GetDocumentList(Args args, CookieContainer cookBox)
        {
            HttpWebRequest wr;
            string responseUri;
            string text;

            string requestUri = string.Format("http://{0}/servlets/ProjectDocumentList?folderID={1}", args.Site, args.Folder);

            wr = (HttpWebRequest)WebRequest.Create(requestUri);
            wr.UserAgent = "Mozilla/8.0 (compatible; Collab Uploader C#)";
            wr.CookieContainer = cookBox;

            using (WebResponse response = wr.GetResponse())
            {
                responseUri = response.ResponseUri.ToString();
                text = GetText(response);
            }

            string body = "([^<]|(\\<(a|div)[^>]*>[^<]*\\<\\/(a|div)>))*";

            Regex re = new Regex("\\<tr[^>]*>\\s*\\<td[^>]*>\\s*\\<div[^>]+class=\"leaf\"[^>]*\\>\\s*" +
                "\\<a href=\"(?<url>[^\"]*)\"[^>]*>\\s*(?<name>[^<]*)\\<\\/a\\>\\s*\\<\\/div\\>\\s*\\<\\/td>\\s*" +
                "\\<td[^>]*>\\s*(?<status>[^<]*)\\</td>\\s*" +
                "\\<td[^>]*>\\s*\\<(a)[^>]*>(?<who>[^<]*)<\\/a>\\s*on\\s(?<date>[^<]*)\\<\\/td>\\s*" +
                "\\<td[^>]*>" + body + "\\<\\/td>\\s*" +
                "\\<td[^>]*>\\s*[^<]*(\\<a[^>]*href=\"(?<reserveUrl>[^\"]*)\"[^>]*>\\s*Reserve[^<]*\\<\\/a>)?" + body + "\\<\\/td>\\s*" +
                "\\<td[^>]*>\\s*(?<desc>[^<]*)\\<\\/td>\\s*" +
                "\\<td[^>]*>\\s*\\<a[^>]*href=\"(?<editUrl>[^\"]*)\"[^>]*>\\s*Edit\\s*\\<\\/a>\\s*<\\/td>\\s*" +                
                "\\<\\/tr\\>"
                , RegexOptions.Singleline | RegexOptions.ExplicitCapture | RegexOptions.IgnoreCase);

            List<Document> docs = new List<Document>();

            Uri baseUri = new Uri(string.Format("http://{0}/servlets/qq", args.Site));
            foreach (Match m in re.Matches(text))
            {
                string url = m.Groups["url"].Value.Trim();
                string name = m.Groups["name"].Value.Trim();
                string status = m.Groups["status"].Value.Trim();
                string who = m.Groups["who"].Value.Trim();
                string date = m.Groups["date"].Value.Trim();
                string reserveUrl = m.Groups["reserveUrl"].Value.Trim();
                string desc = m.Groups["desc"].Value.Trim();
                string editUrl = m.Groups["editUrl"].Value.Trim();

                Document d = new Document();
                d.DownloadUri = new Uri(baseUri, url);
                d.Name = name;
                d.Who = who;
                d.Status = status;
                d.ReserveUri = string.IsNullOrEmpty(reserveUrl) ? null : new Uri(baseUri, reserveUrl);
                d.Description = desc;
                d.EditUri = new Uri(baseUri, editUrl);
                d.FileId = int.Parse(editUrl.Substring(editUrl.IndexOf("documentID=") + 11));

                DateTime when;
                if (!string.IsNullOrEmpty(date) && DateTime.TryParse(date.Replace(" at "," "), out when))
                {
                    d.Date = when;
                }

                docs.Add(d);
            }

            return docs;
        }

        private static void DeleteFile(Args args, CookieContainer cookBox, int fileId)
        {
            string responseUri;
            string text;
            string requestUri = string.Format("http://{0}/servlets/ProjectDocumentDelete", args.Site);


            HttpWebRequest wr = (HttpWebRequest)WebRequest.Create(requestUri);
            wr.UserAgent = UserAgentName;
            wr.CookieContainer = cookBox;
            using (WebFormWriter wfw = new WebFormWriter(wr, WebRequestPostDataEncoding.WwwFormUrlEncoded, "POST"))
            {
                wfw.AddValue("documentID", fileId.ToString(CultureInfo.InvariantCulture));
                wfw.AddValue("Button", "Confirm delete");
                wfw.AddValue("maxDepth", "");
            }

            using (WebResponse response = wr.GetResponse())
            {
                responseUri = response.ResponseUri.ToString();

                if (!response.ResponseUri.AbsolutePath.EndsWith("/ProjectDocumentList", StringComparison.OrdinalIgnoreCase))
                    throw new InvalidOperationException("Delete failed");
            }
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
