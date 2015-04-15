using System;
using System.IO;
using SharpSvn;
using SharpSvn.UI;

class Program
{
    static void Main(string[] args)
    {
        Uri uri = null;
        if (args.Length < 2
            || !Uri.TryCreate(args[0], UriKind.Absolute, out uri))
        {
            Console.Error.WriteLine("Usage: SvnDumpFileToRepository URL PATH [FILENAME [<True|False>]]");
            Console.Error.WriteLine("Filename defaults to the last component of URL");
            Console.Error.WriteLine("The last argument allows disabling the anonymizing by passing False");
            Environment.ExitCode = 1;
            return;
        }

        string reposPath = args[1];

        using (SvnRepositoryClient repos = new SvnRepositoryClient())
        {
            repos.CreateRepository(reposPath);
        }

        string fileName = SvnTools.GetFileName(uri);

        if (args.Length >= 3)
            fileName = args[2];

        bool anonymize;
        if (args.Length < 4 || !bool.TryParse(args[3], out anonymize))
            anonymize = true;

        Uri reposUri = SvnTools.LocalPathToUri(args[1], true);

        using (SvnClient client = new SvnClient())
        using (SvnClient client2 = new SvnClient())
        {
            SvnUI.Bind(client, new SvnUIBindArgs());

            bool create = true;
            client.FileVersions(uri,
                delegate(object sender, SvnFileVersionEventArgs e)
                {
                    Console.WriteLine("Copying {0} in r{1}", fileName, e.Revision);

                    using (MemoryStream ms = new MemoryStream())
                    {
                        if (anonymize)
                            e.WriteTo(new Anonymizer(ms)); // Write full text to memory stream
                        else
                            e.WriteTo(ms);

                        ms.Position = 0;

                        // And now use 'svnmucc' to update repository
                        client2.RepositoryOperation(reposUri,
                            new SvnRepositoryOperationArgs { LogMessage = string.Format("r{0}", e.Revision) },
                            delegate(SvnMultiCommandClient mucc)
                            {
                                if (create)
                                {
                                    mucc.CreateFile(fileName, ms);
                                    create = false;
                                }
                                else
                                    mucc.UpdateFile(fileName, ms);
                            });
                    }
                });
        }
    }
}
