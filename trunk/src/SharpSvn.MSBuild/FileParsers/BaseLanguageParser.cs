using System;
using System.IO;

namespace SharpSvn.MSBuild.FileParsers
{
    abstract class BaseLanguageParser
    {
        public abstract void WriteComment(StreamWriter sw, string text);

        public void WriteComment(StreamWriter sw, string format, params object[] args)
        {
            WriteComment(sw, String.Format(format, args));
        }

        public abstract bool FilterLine(string line);

        public abstract void AddAttribute(Type type);

        protected abstract void StartAttribute(StreamWriter sw, Type type);
        protected abstract void EndAttribute(StreamWriter sw);

        public abstract void WriteAttribute(StreamWriter sw, Type type, string value);

        public abstract void WriteAttribute(StreamWriter sw, Type type, bool value);

        public virtual void WriteAttribute(StreamWriter sw, Type type, Version version)
        {
            StartAttribute(sw, type);

            sw.Write(version.Major);
            sw.Write(", ");
            sw.Write(version.Minor);
            if (version.Build > 0 || version.Revision > 0)
            {
                sw.Write(", ");
                sw.Write(version.Build);

                if (version.Build > 0)
                {
                    sw.Write(", ");
                    sw.Write(version.Revision);
                }
            }

            EndAttribute(sw);
        }

        public virtual string CopyrightEscape(string from)
        {
            return from ?? "";
        }
    }
}
