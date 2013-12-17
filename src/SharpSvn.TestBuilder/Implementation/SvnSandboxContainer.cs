using System;
using System.Collections.Generic;
using System.IO;
using System.Security;
using System.Text;

namespace SharpSvn.TestBuilder.Implementation
{
    class SvnSandboxContainer : IDisposable
    {
        #region Current Container Support
        [ThreadStatic]
        static SvnSandboxContainer _currentContainer;

        public static SvnSandboxContainer Current
        {
            get
            {
                if (_currentContainer == null)
                    _currentContainer = new SvnSandboxContainer();

                return _currentContainer;
            }
        }
        #endregion

        internal SvnSandboxContainer()
        {
            _currentContainer = this;
        }

        List<string> _dirsToClean = new List<string>();

        ~SvnSandboxContainer()
        {
            ClearDirs();
        }

        public void Dispose()
        {
            ClearDirs();
        }

        public void Clear()
        {
            ClearDirs();
            _dirsToClean.Clear();
        }

        private void ClearDirs()
        {
            for (int i = 0; i < _dirsToClean.Count; i++)
            {
                string dir = _dirsToClean[i];

                if (!string.IsNullOrEmpty(dir))
                {
                    try
                    {
                        foreach (FileInfo fif in new DirectoryInfo(dir).GetFiles("*", SearchOption.AllDirectories))
                        {
                            if ((fif.Attributes & FileAttributes.ReadOnly) != 0)
                                fif.Attributes = FileAttributes.Normal;
                        }

                        Directory.Delete(dir, true);
                        _dirsToClean[i] = null;
                    }
                    catch
                    { }
                }
            }
        }

        string _tempPath;
        public string TempPath
        {
            get
            {
                if (_tempPath == null)
                {
                    _tempPath = SvnTools.GetNormalizedFullPath(
                                    Environment.GetEnvironmentVariable("SVNTEST_TEMP")
                                    ?? Environment.GetEnvironmentVariable("TMP")
                                    ?? Environment.GetEnvironmentVariable("TEMP")
                                    ?? System.IO.Path.GetTempPath());

                    _tempPath = Path.Combine(_tempPath, "SandBox");

                    if (!Directory.Exists(_tempPath))
                        Directory.CreateDirectory(_tempPath);
                }
                return _tempPath;
            }
        }

        public string GetTempDir(string basename)
        {
            string basedir = TempPath;

            string dir;
            if (!string.IsNullOrEmpty(basename))
                dir = Path.Combine(basedir, basename + "_" + Path.GetRandomFileName().Substring(0,6));
            else
                dir = Path.Combine(basedir, Path.GetRandomFileName());

            Directory.CreateDirectory(dir);

            _dirsToClean.Add(dir);
            return dir;
        }

        string _filesDir;
        public string GetTempFile(string basename)
        {
            if (_filesDir == null)
                _filesDir = GetTempDir(string.IsNullOrEmpty(basename) ? "files" : (basename + "_files"));

            return Path.Combine(_filesDir, Path.GetRandomFileName());
        }
    }
}
