using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.PdbAnnotate.Framework;
using System.IO;
using System.Diagnostics;
using Microsoft.Build.Utilities;
using SharpSvn;

namespace SharpSvn.PdbAnnotate.Providers
{
    /// <summary>
    /// Implements the <see cref="SourceProvider"/> class for subversion (http://subversion.tigris.org/)
    /// </summary>
    class SubversionResolver : SourceResolver, ISourceProviderDetector
    {
        /// <summary>
        /// 
        /// </summary>
        public SubversionResolver(IndexerState state)
            : base(state, "SVN")
        {
            //GC.KeepAlive(null);
        }

        #region ### Availability
        /// <summary>
        /// 
        /// </summary>
        public override bool Available
        {
            get { return true; }
        }

        #region #### ISourceProviderDetector Members

        /// <summary>
        /// SourceIndex detector which tries to find valid providers
        /// </summary>
        /// <param name="state"></param>
        /// <returns>true if one or more files might be managed in subversion, otherwise false</returns>
        public bool CanProvideSources(SharpSvn.PdbAnnotate.Framework.IndexerState state)
        {
            SortedList<string, string> directories = new SortedList<string, string>(StringComparer.InvariantCultureIgnoreCase);

            foreach (SourceFile file in state.SourceFiles.Values)
            {
                string dir = file.File.DirectoryName;

                if (directories.ContainsKey(dir))
                    continue;

                directories.Add(dir, dir);

                if (SvnTools.IsBelowManagedPath(dir))
                    return true;
            }

            return false;
        }

        #endregion #### ISourceProviderDetector Members
        #endregion ### Availability


        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override bool ResolveFiles()
        {
            SvnClient client = new SvnClient();
            SvnInfoArgs infoArgs = new SvnInfoArgs();

            infoArgs.ThrowOnError = false;
            infoArgs.Depth = SvnDepth.Files;

            foreach (SourceFile file in State.SourceFiles.Values)
            {
                if (file.IsResolved)
                    continue;

                string dirName = SvnTools.GetTruePath(SvnTools.GetNormalizedDirectoryName(file.FullName), true);

                client.Info(dirName, infoArgs,
                    delegate(object sender, SvnInfoEventArgs e)
                    {
                        SourceFile infoFile;

                        string path = e.FullPath;

                        if (State.SourceFiles.TryGetValue(path, out infoFile)
                            && !infoFile.IsResolved)
                        {
                            infoFile.SourceReference = new SubversionSourceReference(this, infoFile, e.RepositoryRoot, 
                                                                                      e.RepositoryRoot.MakeRelativeUri(e.Uri), e.LastChangeRevision, e.Revision);
                        }
                    });
            }

            return true;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        public override void WriteEnvironment(StreamWriter writer)
        {
            writer.Write(Id);
            writer.WriteLine(@"__TRG=%targ%\%var7%\%fnbksl%(%var4%)\%var5%\%fnfile%(%var4%)");
            writer.Write(Id);
            writer.Write("__CMD=svn.exe export \"%var3%%var4%@%var6%\" \"%");
            writer.Write(Id);
            writer.WriteLine("__TRG%\" --non-interactive --quiet");
        }

        /// <summary>
        /// 
        /// </summary>
        public override int SourceEntryVariableCount
        {
            get { return 5; }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    class SubversionSourceReference : SourceReference
    {
        readonly Uri _reposRoot;
        readonly Uri _itemPath;
        readonly long _commitRev;
        readonly long _wcRev;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="resolver"></param>
        /// <param name="sourceFile"></param>
        /// <param name="reposRoot"></param>
        /// <param name="itemPath"></param>
        /// <param name="commitRev"></param>
        /// <param name="wcRev"></param>
        public SubversionSourceReference(SourceProvider resolver, SourceFile sourceFile, Uri reposRoot, Uri itemPath, long commitRev, long wcRev)
            : base(resolver, sourceFile)
        {
            if (reposRoot == null)
                throw new ArgumentNullException("reposRoot");
            else if (itemPath == null)
                throw new ArgumentNullException("itemPath");

            _reposRoot = reposRoot;
            _itemPath = itemPath;

            _commitRev = commitRev;
            _wcRev = wcRev;
        }

        static string ReposSubDir(Uri reposUri)
        {
            StringBuilder sb = new StringBuilder();

            sb.Append("svn-");
            sb.Append(reposUri.Scheme);
            sb.Append("\\");
            if (!string.IsNullOrEmpty(reposUri.Host))
            {
                foreach (char c in reposUri.Host)
                    if (char.IsLetterOrDigit(c) || (".-".IndexOf(c) >= 0))
                        sb.Append(c);
                    else
                        sb.Append('_');

                if (reposUri.Port >= 1)
                    sb.AppendFormat("_{0}", reposUri.Port);
            }

            if (!string.IsNullOrEmpty(reposUri.AbsolutePath))
            {
                sb.Append(reposUri.AbsolutePath);
            }

            sb.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
            sb.Replace(":", null);
            sb.Replace("<", null);
            sb.Replace(">", null);
            sb.Replace(@"\\", @"\");
            sb.Replace("//", "/");

            while (sb.Length > 0 && sb[sb.Length - 1] == Path.DirectorySeparatorChar)
                sb.Length -= 1;

            return sb.ToString();
        }

        /// <summary>
        /// Gets a list of entries for the sourcefiles
        /// </summary>
        /// <returns></returns>
        public override string[] GetSourceEntries()
        {
            return new string[]
            {
                _reposRoot.ToString(),
                _itemPath.ToString(),
                _commitRev.ToString(),
                _wcRev.ToString(),
                ReposSubDir(_reposRoot)
            };
        }
    }
}
