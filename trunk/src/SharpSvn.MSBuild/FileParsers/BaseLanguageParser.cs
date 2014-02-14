using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace SharpSvn.MSBuild.FileParsers
{
    sealed class AttributeRegex
    {
        readonly Type _attribute;
        readonly Regex _re;
        public AttributeRegex(Type attributeType, Regex re)
        {
            _attribute = attributeType;
            _re = re;

        }

        public AttributeRegex(Type attributeType, string pattern, RegexOptions extraOptions)
            : this (attributeType, new Regex(pattern, extraOptions | RegexOptions.CultureInvariant | RegexOptions.Singleline | RegexOptions.ExplicitCapture))
        {

        }

        public bool Matches(string input)
        {
            if (_re.Match(input).Success)
            {
                Found = true;
                return true;
            }
            return false;
        }

        public Type AttributeType
        {
            get { return _attribute; }
        }

        public bool Found
        { get; set; }

        public bool KeepExisting
        { get; set; }
    }

    abstract class LanguageParser
    {
        readonly Dictionary<Type, AttributeRegex> _attributeMap = new Dictionary<Type, AttributeRegex>();

        public abstract void WriteComment(StreamWriter sw, string text);

        public void WriteComment(StreamWriter sw, string format, params object[] args)
        {
            WriteComment(sw, String.Format(format, args));
        }

        protected Dictionary<Type, AttributeRegex> AttrMap
        {
            get { return _attributeMap; }
        }

        public abstract bool FilterLine(string line);

        protected abstract void AddAttribute(Type attributeType);

        public void AddAttribute<T>() where T : Attribute
        {
            AddAttribute(typeof(T));
        }

        internal void AddAttribute(Type attributeType, bool keepExisting)
        {
            AddAttribute(attributeType);

            AttributeRegex ar;

            if (AttrMap.TryGetValue(attributeType, out ar))
            {
                ar.KeepExisting = false;
            }
        }

        public void AddAttribute<T>(bool keepExisting) where T : Attribute
        {
            AddAttribute(typeof(T), keepExisting);
        }

        protected abstract void StartAttribute(StreamWriter sw, Type type);
        protected abstract void EndAttribute(StreamWriter sw);

        bool SkipWrite(Type attributeType)
        {
            AttributeRegex ar;

            if (AttrMap.TryGetValue(attributeType, out ar))
            {
                return (ar.KeepExisting && ar.Found);
            }
            return false;
        }

        protected abstract void WriteAttribute(StreamWriter sw, Type attributeType, string value);

        public void WriteAttribute<T>(StreamWriter sw, string value) where T: Attribute
        {
            Type attributeType = typeof(T);

            if (SkipWrite(attributeType))
                return;

            if (value.StartsWith("?"))
                value = value.Substring(1);

            WriteAttribute(sw, attributeType, value);
        }

        protected abstract void WriteAttribute(StreamWriter sw, Type type, bool value);

        public virtual void WriteAttribute<T>(StreamWriter sw, bool value) where T : Attribute
        {
            Type attributeType = typeof(T);

            if (SkipWrite(attributeType))
                return;

            WriteAttribute(sw, attributeType, value);
        }

        protected virtual void WriteAttribute(StreamWriter sw, Type type, Version version)
        {
            if (SkipWrite(type))
                return;

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

        public virtual void WriteAttribute<T>(StreamWriter sw, Version version) where T : Attribute
        {
            Type attributeType = typeof(T);

            if (SkipWrite(attributeType))
                return;

            WriteAttribute(sw, attributeType, version);
        }

        public virtual string CopyrightEscape(string from)
        {
            return from ?? "";
        }
    }
}
