using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.PdbAnnotate.Framework;

namespace SharpSvn.PdbAnnotate.Providers
{
    /// <summary>
    /// 
    /// </summary>
    interface ISourceProviderDetector
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="state"></param>
        /// <returns></returns>
        bool CanProvideSources(IndexerState state);
    }

    /// <summary>
    /// Base class of sourceproviders
    /// </summary>
    abstract class SourceResolver : SourceProvider
    {
        readonly string _name;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="state"></param>
        /// <param name="name"></param>
        protected SourceResolver(IndexerState state, string name)
            : base(state, name)
        {
            _name = name;
        }

        /// <summary>
        /// 
        /// </summary>
        public override string Name
        {
            get { return _name; }
        }

        /// <summary>
        /// 
        /// </summary>
        public abstract bool Available
        {
            get;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public abstract bool ResolveFiles();

        /// <summary>
        /// Gets the number of variables the provider needs to encode entries
        /// </summary>
        /// <value>The number of variable the provider requires for encoding the per-sourcefile data of the sourcelocation</value>
        /// <remarks>This value must remain constant. The provider may use <c>%VAR3%</c> upto <c>%VARx%</c> where x = <see cref="SourceEntryVariableCount"/>+2; 
        /// All variables above this number are reserved for the framework</remarks>
        public override int SourceEntryVariableCount
        {
            get { return 3; }
        }
    }
}
