using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Reflection.Emit;
using Microsoft.Build.Framework;
using Microsoft.Win32;

namespace SharpSvn.MSBuild
{
    public class UpdateNativeVersionResource : ITask
    {
                public Microsoft.Build.Framework.IBuildEngine BuildEngine
                {
                        get; set;
                }

                [Required]
                public ITaskItem Source
                {
                        get; set;
                }

                [Required]
                public string TempDir
                {
                        get; set;
                }

                public string KeyContainer
                {
                        get; set;
                }

                public ITaskItem KeyFile
                {
                        get; set;
                }

                [Output]
                public bool SourceUpdated
                {
                        get;
                        set;
                }

                public Microsoft.Build.Framework.ITaskHost HostObject
                {
                        get; set;
                }

                public bool Execute()
                {
                        if (NativeMethods.GetFileVersionInfoSize(Source.ItemSpec) > 0)
                                return true; // Nothing to do

                        try
                        {

                                if (Directory.Exists(TempDir))
                                        Directory.Delete(TempDir, true);

                                Directory.CreateDirectory(TempDir);
                                string srcFile = Source.ItemSpec;
                                string tmpFile = Path.Combine(TempDir, Path.GetFileName(Source.ItemSpec));

                                Assembly myAssembly = typeof(UpdateNativeVersionResource).Assembly;
                                AppDomainSetup setup = new AppDomainSetup();
                                setup.ApplicationName = "GenerateVersionInfoViaAssembly";
                                setup.ApplicationBase = Path.GetDirectoryName(new Uri(myAssembly.CodeBase).LocalPath);
                                setup.AppDomainInitializer = new AppDomainInitializer(OnRefreshVersionInfo);
                                setup.AppDomainInitializerArguments = new string[] { Source.ItemSpec, TempDir, tmpFile };

                                AppDomain dom = AppDomain.CreateDomain("AttributeRefresher", myAssembly.Evidence, setup);
                                AppDomain.Unload(dom); // Remove locks

                                byte[] versionInfo;
                                int size = NativeMethods.GetFileVersionInfoSize(tmpFile);
                                if (size >= 0 && NativeMethods.GetFileVersionInfo(tmpFile, size, out versionInfo))
                                {
                                        File.Delete(tmpFile);

                                        BuildEngine.LogMessageEvent(new BuildMessageEventArgs("Updating version resources", null, "UpdateNativeVersionResouce", MessageImportance.Normal));
                                        using (ResourceUpdateHandle resHandle = NativeMethods.BeginUpdateResource(srcFile, false))
                                        {
                                                if (resHandle != null)
                                                {
                                                        bool ok = NativeMethods.UpdateResource(resHandle, (IntPtr)16, (IntPtr)1, 0, versionInfo, size);
                                                        ok = resHandle.Commit() && ok;

                                                        if (ok)
                                                        {
                                                                if (!string.IsNullOrEmpty(KeyContainer) || (KeyFile != null && !string.IsNullOrEmpty(KeyFile.ItemSpec)))
                                                                {
                                                                        BuildEngine.LogMessageEvent(new BuildMessageEventArgs("Resigning assembly", null, "UpdateNativeVersionResouce", MessageImportance.Normal));

                                                                        if (!ResignAssemblyWithFileOrContainer(srcFile, (KeyFile != null) ? KeyFile.ItemSpec : null, KeyContainer))
                                                                        {
                                                                                BuildEngine.LogMessageEvent(new BuildMessageEventArgs("Resigning assembly failed", null, "UpdateNativeVersionResouce", MessageImportance.High));
                                                                                return false;
                                                                        }
                                                                }

                                                                SourceUpdated = true;
                                                                return true;
                                                        }
                                                }
                                        }
                                }
                                else
                                        File.Delete(tmpFile);

                                BuildEngine.LogMessageEvent(new BuildMessageEventArgs("Updating version resources failed", null, "UpdateNativeVersionResouce", MessageImportance.Normal));

                                return false;
                        }
                        finally
                        {
                                if (!SourceUpdated && File.Exists(Source.ItemSpec))
                                        File.Delete(Source.ItemSpec); // Ensure relinking in next build
                        }
                }

                internal static bool ResignAssemblyWithFileOrContainer(string assembly, string keyFile, string keyContainer)
                {
                        if (!string.IsNullOrEmpty(keyContainer))
                        {
                                if(ResignAssemblyWithContainer(assembly, keyContainer))
                                        return true;
                        }

                        if (!string.IsNullOrEmpty(keyFile))
                                return ResignAssemblyWithFile(assembly, keyFile);
                        else
                                return string.IsNullOrEmpty(keyContainer);
                }

                /// <summary>
                /// Res the sign assembly.
                /// </summary>
                /// <param name="assembly">The assembly.</param>
                /// <param name="strongNameFile">The strong name file.</param>
                /// <returns></returns>
                public static bool ResignAssemblyWithFile(string assembly, string strongNameFile)
                {
                        if (string.IsNullOrEmpty(assembly))
                                throw new ArgumentNullException("assembly");
                        else if (string.IsNullOrEmpty(strongNameFile))
                                throw new ArgumentNullException("strongNameFile");
                        else if (!File.Exists(assembly))
                                throw new FileNotFoundException("Assembly not found", assembly);
                        else if (!File.Exists(strongNameFile))
                                throw new FileNotFoundException(string.Format(CultureInfo.InvariantCulture, "StrongNameFile not found: {0}", strongNameFile), strongNameFile);

                        ProcessStartInfo psi = new ProcessStartInfo(StrongNameToolPath, string.Format(CultureInfo.InvariantCulture, "-q -Ra \"{0}\" \"{1}\"", assembly, strongNameFile));
                        psi.UseShellExecute = false;
                        psi.WindowStyle = ProcessWindowStyle.Hidden;
                        psi.CreateNoWindow = true;
                        using (Process p = Process.Start(psi))
                        {
                                p.WaitForExit();

                                return p.ExitCode == 0;
                        }
                }

                static string _sdkPath;
                /// <summary>
                /// Gets the framework SDK dir.
                /// </summary>
                /// <value>The framework SDK dir.</value>
                static string FrameworkSdkDir
                {
                        get
                        {
                                if (_sdkPath == null)
                                {
                                        using (RegistryKey rk = Registry.LocalMachine.OpenSubKey("SOFTWARE\\Microsoft\\.NETFramework", false))
                                        {
                                                if (rk != null)
                                                {
                                                        string primary = null;
                                                        string fallback = null;
                                                        string myVersion = ("sdkInstallRoot" + typeof(UpdateNativeVersionResource).Assembly.ImageRuntimeVersion);

                                                        foreach (string name in rk.GetValueNames())
                                                        {
                                                                if (name.StartsWith("sdkinstallroot", StringComparison.OrdinalIgnoreCase))
                                                                {
                                                                        if (fallback == null)
                                                                        {
                                                                                string value = (string)rk.GetValue(name);

                                                                                if (Directory.Exists(value))
                                                                                {
                                                                                        fallback = name;
                                                                                }
                                                                        }

                                                                        if (myVersion.StartsWith(name, StringComparison.OrdinalIgnoreCase))
                                                                        {
                                                                                string value = (string)rk.GetValue(name);

                                                                                if (Directory.Exists(value))
                                                                                {
                                                                                        if ((primary == null) || (name.Length > primary.Length))
                                                                                                primary = name;
                                                                                }
                                                                        }
                                                                }
                                                        }

                                                        if (primary == null)
                                                                primary = fallback;

                                                        if (primary != null)
                                                        {
                                                                _sdkPath = (string)rk.GetValue(primary);
                                                        }
                                                }
                                        }
                                }
                                return _sdkPath;
                        }
                }

                static string _snExe;
                /// <summary>
                /// Gets the sn.exe path.
                /// </summary>
                /// <value>The sn exe path.</value>
                static string StrongNameToolPath
                {
                        get
                        {
                                if (_snExe == null)
                                {
                                        string sn = Path.Combine(FrameworkSdkDir ?? ".", "bin\\sn.exe");

                                        if (File.Exists(sn))
                                                _snExe = sn;
                                        else
                                                _snExe = FindFileInPath("sn.exe");

                                }
                                return _snExe;
                        }
                }

                static string FindFileInPath(string file, string pathList)
                {
                        if (string.IsNullOrEmpty(file))
                                throw new ArgumentNullException("file");
                        else if (string.IsNullOrEmpty(pathList))
                                throw new ArgumentNullException("pathList");

                        string[] paths = pathList.Split(Path.PathSeparator);

                        foreach (string i in paths)
                        {
                                if (string.IsNullOrEmpty(i))
                                        continue;

                                string fullPath = Path.GetFullPath(Path.Combine(i, file));

                                if (File.Exists(fullPath))
                                        return fullPath;
                        }

                        return null;
                }

                /// <summary>
                /// Finds the file in the system environment variable path, the current directory, or the directory containing the current application.
                /// </summary>
                /// <param name="file">the filename of the file to search</param>
                /// <returns>The full path of the file or <c>null</c> if the file is not found</returns>
                static string FindFileInPath(string file)
                {
                        if (string.IsNullOrEmpty(file))
                                throw new ArgumentNullException("file");

                        string path = Environment.GetEnvironmentVariable("PATH");

                        string result;
                        if (!string.IsNullOrEmpty(path))
                        {
                                result = FindFileInPath(file, path);

                                if (!string.IsNullOrEmpty(result))
                                        return result;
                        }

                        result = FindFileInPath(file, ".");

                        if (!string.IsNullOrEmpty(result))
                                return result;

                        Assembly asm = Assembly.GetEntryAssembly();

                        if (asm == null)
                                asm = Assembly.GetCallingAssembly();

                        if (asm != null)
                                result = FindFileNextToAssembly(file, asm);

                        return result;
                }

                private static string FindFileNextToAssembly(string file, Assembly assembly)
                {
                        if (string.IsNullOrEmpty(file))
                                throw new ArgumentNullException("file");
                        else if (assembly == null)
                                throw new ArgumentNullException("assembly");

                        if (assembly.CodeBase == null)
                                return null;

                        Uri uri = new Uri(assembly.CodeBase);

                        if (uri.IsFile || uri.IsUnc)
                                return FindFileInPath(file, Path.GetDirectoryName(uri.LocalPath));

                        return null;
                }

                /// <summary>
                /// Res the sign assembly.
                /// </summary>
                /// <param name="assembly">The assembly.</param>
                /// <param name="container">The container.</param>
                /// <returns></returns>
                public static bool ResignAssemblyWithContainer(string assembly, string container)
                {
                        if (string.IsNullOrEmpty(assembly))
                                throw new ArgumentNullException("assembly");
                        else if (string.IsNullOrEmpty(container))
                                throw new ArgumentNullException("container");
                        else if (!File.Exists(assembly))
                                throw new FileNotFoundException("Assembly not found", assembly);

                        ProcessStartInfo psi = new ProcessStartInfo(StrongNameToolPath, string.Format(CultureInfo.InvariantCulture, "-q -Rca \"{0}\" \"{1}\"", assembly, container));
                        psi.UseShellExecute = false;
                        psi.WindowStyle = ProcessWindowStyle.Hidden;
                        psi.CreateNoWindow = true;
                        using (Process p = Process.Start(psi))
                        {
                                p.WaitForExit();

                                return p.ExitCode == 0;
                        }
                }

                // Called in it's own appdomain
                static void OnRefreshVersionInfo(string[] args)
                {
                        string fromFile = args[0];
                        string toDir = args[1];
                        string toFile = args[2];

                        Assembly asm = Assembly.ReflectionOnlyLoad(File.ReadAllBytes(fromFile));

                        string result = GenerateAttributeAssembly(asm, toDir);
                }

                private static string GenerateAttributeAssembly(Assembly assembly, string outputDirectory)
                {
                        AssemblyName srcName = new AssemblyName(assembly.FullName);

                        if (srcName == null || string.IsNullOrEmpty(srcName.Name))
                                return null;

                        try
                        {
                                // Prepare dynamic assembly for resources
                                AssemblyName asmName = new AssemblyName(srcName.FullName);

                                // Only create an on-disk assembly. We never have to execute anything
                                AssemblyBuilder newAssembly = AppDomain.CurrentDomain.DefineDynamicAssembly(asmName, AssemblyBuilderAccess.ReflectionOnly, outputDirectory);

                                string tmpFile = srcName.Name + ".dll";
                                newAssembly.DefineDynamicModule(asmName.Name, tmpFile);

                                AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve += new ResolveEventHandler(OnReflectionOnlyAssemblyResolve);

                                try
                                {
                                        Assembly mscorlib = Assembly.ReflectionOnlyLoad(typeof(int).Assembly.FullName);
                                        Assembly system = Assembly.ReflectionOnlyLoad(typeof(Uri).Assembly.FullName);
                                        bool hasInformationalVersion = false;
                                        bool hasVersion = false;

                                        foreach (CustomAttributeData attr in CustomAttributeData.GetCustomAttributes(assembly))
                                        {
                                                if ((attr.NamedArguments.Count > 0) || (attr.Constructor == null))
                                                {
                                                        // We don't use named arguments at this time; not needed for the version resources
                                                        continue;
                                                }

                                                Type type = attr.Constructor.ReflectedType;

                                                if (type.Assembly != mscorlib && type.Assembly != system)
                                                {
                                                        continue;
                                                }

                                                if (type.Assembly == mscorlib)
                                                        switch (type.Name)
                                                        {
                                                                case "System.Reflection.AssemblyInformationalVersionAttribute":
                                    hasInformationalVersion = true;
                                    break;
                                case "System.Reflection.AssemblyVersionAttribute":
                                    hasVersion = true;
                                    break;
                            }

                        List<object> values = new List<object>();
                        foreach (CustomAttributeTypedArgument arg in attr.ConstructorArguments)
                        {
                            values.Add(arg.Value);
                        }

                        CustomAttributeBuilder cb = new CustomAttributeBuilder(attr.Constructor, values.ToArray());

                        newAssembly.SetCustomAttribute(cb);
                    }

                    if (!hasVersion)
                        newAssembly.SetCustomAttribute(
                                        new CustomAttributeBuilder(typeof(AssemblyVersionAttribute).GetConstructor(new Type[] { typeof(String) }),
                                                                                           new object[] { srcName.Version.ToString() }));
                    if (!hasInformationalVersion)
                        newAssembly.SetCustomAttribute(
                                        new CustomAttributeBuilder(typeof(AssemblyInformationalVersionAttribute).GetConstructor(new Type[] { typeof(String) }),
                                                                                           new object[] { srcName.Version.ToString() }));

                    newAssembly.SetCustomAttribute(
                                            new CustomAttributeBuilder(typeof(AssemblyCultureAttribute).GetConstructor(new Type[] { typeof(String) }),
                                                                                                    new object[] { "" }));
                }
                finally
                {
                    AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve -= new ResolveEventHandler(OnReflectionOnlyAssemblyResolve);
                }

                newAssembly.DefineVersionInfoResource();
                newAssembly.Save(tmpFile);

                return Path.Combine(outputDirectory, tmpFile);
            }
            catch (FileLoadException)
            {
                return null;
            }
            catch (IOException)
            {
                return null;
            }
        }

        /// <summary>
        /// Called when [reflection only assembly resolve].
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="args">The <see cref="System.ResolveEventArgs"/> instance containing the event data.</param>
        /// <returns></returns>
        static Assembly OnReflectionOnlyAssemblyResolve(object sender, ResolveEventArgs args)
        {
            return Assembly.ReflectionOnlyLoad(args.Name);
        }
    }
}
