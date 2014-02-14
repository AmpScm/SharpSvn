using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;
using SharpSvn.MSBuild.FileParsers;
using ComCompatibleVersionAttribute = System.Runtime.InteropServices.ComCompatibleVersionAttribute;
using ComVisibleAttribute = System.Runtime.InteropServices.ComVisibleAttribute;
using NeutralResourcesLanguageAttribute = System.Resources.NeutralResourcesLanguageAttribute;
using SatelliteContractVersionAttribute = System.Resources.SatelliteContractVersionAttribute;

namespace SharpSvn.MSBuild
{
    public enum Language
    {
        Unknown,
        cpp,
        csharp,
        vb
    }

    public class TransformAssemblyInfo : ITask
    {
        readonly List<string> _filesWritten = new List<string>();
        Language _language;

        public Microsoft.Build.Framework.IBuildEngine BuildEngine
        {
            get;
            set;
        }

        [Required]
        public string Language
        {
            get { return _language.ToString(); }
            set { _language = (Language)Enum.Parse(typeof(Language), value, true); }
        }

        [Required]
        public ITaskItem[] Sources
        {
            get;
            set;
        }

        public string SetVersion
        { get; set; }

        public string SetOnlyAssemblyVersion
        { get; set; }
        public string SetFileVersion
        { get; set; }
        public string SetInformationalVersion
        { get; set; }
        public string SetSatelliteContractVersion
        { get; set; }
        public string SetComCompatibleVersion
        { get; set; }

        public string SetCopyright
        { get; set; }
        public string SetCompany
        { get; set; }
        public string SetTrademark
        { get; set; }
        public string SetConfiguration
        { get; set; }
        public string SetDescription
        { get; set; }
        public string SetProduct
        { get; set; }
        public string SetTitle
        { get; set; }

        public string SetClsCompliant
        { get; set; }

        public string SetComVisible
        { get; set; }

        public string SetNeutralResourceLanguage
        { get; set; }

        [Output]
        public ITaskItem[] FilesWritten
        {
            get
            {
                ITaskItem[] items = new ITaskItem[_filesWritten.Count];
                int n = 0;
                foreach (string f in _filesWritten)
                {
                    items[n++] = new TaskItem(f);
                }
                return items;
            }
        }

        public Microsoft.Build.Framework.ITaskHost HostObject
        { get; set; }

        public bool Execute()
        {
            foreach (ITaskItem ti in Sources)
            {
                string src = ti.GetMetadata("InfoSource");

                if (!File.Exists(src))
                    continue;

                string dest = ti.ItemSpec;
                string dir = Path.GetDirectoryName(dest);

                Directory.CreateDirectory(dir);

                _filesWritten.Add(dest);

                string tmp = dest + ".new";
                using (StreamWriter sw = File.CreateText(tmp))
                using (StreamReader sr = File.OpenText(src))
                {
                    string line;

                    LanguageParser parser = CreateParser();

                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetOnlyAssemblyVersion))
                        parser.AddAttribute<AssemblyVersionAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetFileVersion))
                        parser.AddAttribute<AssemblyFileVersionAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetInformationalVersion))
                        parser.AddAttribute<AssemblyInformationalVersionAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetSatelliteContractVersion))
                        parser.AddAttribute<System.Resources.SatelliteContractVersionAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetComCompatibleVersion))
                        parser.AddAttribute<System.Runtime.InteropServices.ComCompatibleVersionAttribute>();

                    if (!string.IsNullOrWhiteSpace(SetCopyright))
                        parser.AddAttribute<AssemblyCopyrightAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetCompany))
                        parser.AddAttribute<AssemblyCompanyAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetTrademark))
                        parser.AddAttribute<AssemblyTrademarkAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetConfiguration))
                        parser.AddAttribute<AssemblyConfigurationAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetDescription))
                        parser.AddAttribute<AssemblyDescriptionAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetProduct))
                        parser.AddAttribute<AssemblyProductAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetTitle))
                        parser.AddAttribute<AssemblyTitleAttribute>();

                    if (!string.IsNullOrWhiteSpace(SetClsCompliant))
                        parser.AddAttribute<CLSCompliantAttribute>(SetClsCompliant.StartsWith("?"));
                    if (!string.IsNullOrWhiteSpace(SetComVisible))
                        parser.AddAttribute<ComVisibleAttribute>();
                    if (!string.IsNullOrWhiteSpace(SetNeutralResourceLanguage))
                        parser.AddAttribute<NeutralResourcesLanguageAttribute>();

                    parser.WriteComment(sw, string.Format("Generated by {0}", typeof(TransformAssemblyInfo).Assembly.FullName));
                    parser.WriteComment(sw, string.Format("Based on {0}", src));

                    while (null != (line = sr.ReadLine()))
                    {
                        if (parser.FilterLine(line))
                            parser.WriteComment(sw, line);
                        else
                            sw.WriteLine(line);
                    }

                    sw.WriteLine();
                    sw.WriteLine();
                    parser.WriteComment(sw, "** Generated attributes **");

                    if (!string.IsNullOrWhiteSpace(SetOnlyAssemblyVersion))
                        parser.WriteAttribute<AssemblyVersionAttribute>(sw, SetOnlyAssemblyVersion);
                    else if (!string.IsNullOrWhiteSpace(SetVersion))
                        parser.WriteAttribute<AssemblyVersionAttribute>(sw, SetVersion);

                    if (!string.IsNullOrWhiteSpace(SetFileVersion))
                        parser.WriteAttribute<AssemblyVersionAttribute>(sw, SetFileVersion);
                    if (!string.IsNullOrWhiteSpace(SetInformationalVersion))
                        parser.WriteAttribute<AssemblyVersionAttribute>(sw, SetInformationalVersion);
                    if (!string.IsNullOrWhiteSpace(SetSatelliteContractVersion))
                        parser.WriteAttribute<SatelliteContractVersionAttribute>(sw, SetSatelliteContractVersion);
                    if (!string.IsNullOrWhiteSpace(SetComCompatibleVersion))
                        parser.WriteAttribute<ComCompatibleVersionAttribute>(sw, new Version(SetComCompatibleVersion));

                    if (!string.IsNullOrWhiteSpace(SetCopyright))
                        parser.WriteAttribute<AssemblyCopyrightAttribute>(sw, parser.CopyrightEscape(SetCopyright));
                    if (!string.IsNullOrWhiteSpace(SetCompany))
                        parser.WriteAttribute<AssemblyCompanyAttribute>(sw, SetCompany);
                    if (!string.IsNullOrWhiteSpace(SetTrademark))
                        parser.WriteAttribute<AssemblyTrademarkAttribute>(sw, SetTrademark);
                    if (!string.IsNullOrWhiteSpace(SetConfiguration))
                        parser.WriteAttribute<AssemblyConfigurationAttribute>(sw, SetConfiguration);
                    if (!string.IsNullOrWhiteSpace(SetDescription))
                        parser.WriteAttribute<AssemblyDescriptionAttribute>(sw, SetDescription);
                    if (!string.IsNullOrWhiteSpace(SetProduct))
                        parser.WriteAttribute<AssemblyProductAttribute>(sw, SetProduct);
                    if (!string.IsNullOrWhiteSpace(SetTitle))
                        parser.WriteAttribute<AssemblyTitleAttribute>(sw, SetTitle);

                    if (!string.IsNullOrWhiteSpace(SetClsCompliant))
                        parser.WriteAttribute<CLSCompliantAttribute>(sw, ParseBool(SetClsCompliant, true));
                    if (!string.IsNullOrWhiteSpace(SetComVisible))
                        parser.WriteAttribute<ComVisibleAttribute>(sw, ParseBool(SetComVisible, false));

                    if (!string.IsNullOrWhiteSpace(SetNeutralResourceLanguage))
                        parser.WriteAttribute<NeutralResourcesLanguageAttribute>(sw, SetNeutralResourceLanguage);
                }

                if (File.Exists(dest))
                {
                    bool same = false;
                    using (StreamReader f1 = File.OpenText(dest))
                    using (StreamReader f2 = File.OpenText(tmp))
                    {
                        string line;

                        while (f1.ReadLine() == (line = f2.ReadLine()))
                        {
                            if (line == null)
                            {
                                same = true;
                                break;
                            }
                        }
                    }

                    if (same)
                    {
                        File.Delete(tmp);
                        continue;
                    }

                    File.Delete(dest);
                }
                File.Move(tmp, dest);
            }
            return true;
        }

        static bool ParseBool(string value, bool defaultValue)
        {
            if (value.StartsWith("?"))
                value = value.Substring(1);

            if (string.Equals(value, "true", StringComparison.OrdinalIgnoreCase))
                return true;
            else if (string.Equals(value, "false", StringComparison.OrdinalIgnoreCase))
                return false;

            return defaultValue;
        }

        private LanguageParser CreateParser()
        {
            switch (_language)
            {
                case MSBuild.Language.cpp:
                    return new CppParser();
                case MSBuild.Language.csharp:
                    return new CSharpParser();
                case MSBuild.Language.vb:
                    return new VBParser();
                default:
                    throw new NotImplementedException("Unimplemented language");
            }
        }
    }
}
