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
        {
            get;
            set;
        }

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

                    BaseLanguageParser parser = CreateParser();

                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetOnlyAssemblyVersion))
                        parser.AddAttribute(typeof(AssemblyVersionAttribute));
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetFileVersion))
                        parser.AddAttribute(typeof(AssemblyFileVersionAttribute));
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetInformationalVersion))
                        parser.AddAttribute(typeof(AssemblyInformationalVersionAttribute));
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetSatelliteContractVersion))
                        parser.AddAttribute(typeof(System.Resources.SatelliteContractVersionAttribute));
                    if (!string.IsNullOrWhiteSpace(SetVersion) || !string.IsNullOrWhiteSpace(SetComCompatibleVersion))
                        parser.AddAttribute(typeof(System.Runtime.InteropServices.ComCompatibleVersionAttribute));

                    if (!string.IsNullOrWhiteSpace(SetCopyright))
                        parser.AddAttribute(typeof(AssemblyCopyrightAttribute));
                    if (!string.IsNullOrWhiteSpace(SetCompany))
                        parser.AddAttribute(typeof(AssemblyCompanyAttribute));
                    if (!string.IsNullOrWhiteSpace(SetTrademark))
                        parser.AddAttribute(typeof(AssemblyTrademarkAttribute));
                    if (!string.IsNullOrWhiteSpace(SetConfiguration))
                        parser.AddAttribute(typeof(AssemblyConfigurationAttribute));
                    if (!string.IsNullOrWhiteSpace(SetDescription))
                        parser.AddAttribute(typeof(AssemblyDescriptionAttribute));
                    if (!string.IsNullOrWhiteSpace(SetProduct))
                        parser.AddAttribute(typeof(AssemblyProductAttribute));
                    if (!string.IsNullOrWhiteSpace(SetTitle))
                        parser.AddAttribute(typeof(AssemblyTitleAttribute));

                    if (!string.IsNullOrWhiteSpace(SetClsCompliant))
                        parser.AddAttribute(typeof(CLSCompliantAttribute));
                    if (!string.IsNullOrWhiteSpace(SetComVisible))
                        parser.AddAttribute(typeof(ComVisibleAttribute));
                    if (!string.IsNullOrWhiteSpace(SetNeutralResourceLanguage))
                        parser.AddAttribute(typeof(NeutralResourcesLanguageAttribute));

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
                        parser.WriteAttribute(sw, typeof(AssemblyVersionAttribute), SetOnlyAssemblyVersion);
                    else if (!string.IsNullOrWhiteSpace(SetVersion))
                        parser.WriteAttribute(sw, typeof(AssemblyVersionAttribute), SetVersion);

                    if (!string.IsNullOrWhiteSpace(SetFileVersion))
                        parser.WriteAttribute(sw, typeof(AssemblyVersionAttribute), SetFileVersion);
                    if (!string.IsNullOrWhiteSpace(SetInformationalVersion))
                        parser.WriteAttribute(sw, typeof(AssemblyVersionAttribute), SetInformationalVersion);
                    if (!string.IsNullOrWhiteSpace(SetSatelliteContractVersion))
                        parser.WriteAttribute(sw, typeof(SatelliteContractVersionAttribute), SetSatelliteContractVersion);
                    if (!string.IsNullOrWhiteSpace(SetComCompatibleVersion))
                        parser.WriteAttribute(sw, typeof(ComCompatibleVersionAttribute), new Version(SetComCompatibleVersion));

                    if (!string.IsNullOrWhiteSpace(SetCopyright))
                        parser.WriteAttribute(sw, typeof(AssemblyCopyrightAttribute), parser.CopyrightEscape(SetCopyright));
                    if (!string.IsNullOrWhiteSpace(SetCompany))
                        parser.WriteAttribute(sw, typeof(AssemblyCompanyAttribute), SetCompany);
                    if (!string.IsNullOrWhiteSpace(SetTrademark))
                        parser.WriteAttribute(sw, typeof(AssemblyTrademarkAttribute), SetTrademark);
                    if (!string.IsNullOrWhiteSpace(SetConfiguration))
                        parser.WriteAttribute(sw, typeof(AssemblyConfigurationAttribute), SetConfiguration);
                    if (!string.IsNullOrWhiteSpace(SetDescription))
                        parser.WriteAttribute(sw, typeof(AssemblyDescriptionAttribute), SetDescription);
                    if (!string.IsNullOrWhiteSpace(SetProduct))
                        parser.WriteAttribute(sw, typeof(AssemblyProductAttribute), SetProduct);
                    if (!string.IsNullOrWhiteSpace(SetTitle))
                        parser.WriteAttribute(sw, typeof(AssemblyTitleAttribute), SetTitle);

                    if (!string.IsNullOrWhiteSpace(SetClsCompliant))
                        parser.WriteAttribute(sw, typeof(CLSCompliantAttribute), !String.Equals(SetClsCompliant, "false", StringComparison.OrdinalIgnoreCase));
                    if (!string.IsNullOrWhiteSpace(SetComVisible))
                        parser.WriteAttribute(sw, typeof(ComVisibleAttribute), String.Equals(SetClsCompliant, "true", StringComparison.OrdinalIgnoreCase));

                    if (!string.IsNullOrWhiteSpace(SetNeutralResourceLanguage))
                        parser.WriteAttribute(sw, typeof(NeutralResourcesLanguageAttribute), SetNeutralResourceLanguage);
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

        private BaseLanguageParser CreateParser()
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
