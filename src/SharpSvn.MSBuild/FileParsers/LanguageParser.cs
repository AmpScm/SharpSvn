using System;
using System.IO;

namespace SharpSvn.MSBuild.FileParsers
{
    abstract class LanguageParser
    {
        public abstract void WriteComment(StreamWriter sw, string text);

        public void WriteComment(StreamWriter sw, string format, params object[] args)
        {
            WriteComment(sw, String.Format(format, args));
        }

        public abstract bool FilterLine(string line);

        public abstract void AddAttribute(Type type);

        public abstract void WriteAttribute(StreamWriter sw, Type type, string value);
    }
}
