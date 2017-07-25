using System;
using System.Collections.Generic;
using System.Text;
using SharpSvn.PdbAnnotate.Framework;
using System.IO;

namespace SharpSvn.PdbAnnotate.Providers
{
    /// <summary>
    /// The base class of all sourceproviders
    /// </summary>
    /// <remarks>
    ///		<para>A sourceprovider has a unique <see cref="Id"/> which it can use to prefix variables.</para>
    ///		<para>All variables starting with <c>Id</c>_ are automatically reserved for the provider, except for
    ///		the <c>Id</c>__ prefix, which is used for framework variables. This
    ///		id is reserved at the time the sourceprovider is registered in the <see cref="IndexerState"/>;
    ///		which is after the <see cref="SourceProvider"/>s constructor, completes</para>
    /// </remarks>
    abstract class SourceProvider
    {
        readonly IndexerState _state;
        readonly string _id;

        /// <summary>
        /// Initializes a new <see cref="SourceProvider"/> with the specified name
        /// </summary>
        /// <param name="state"></param>
        /// <param name="name"></param>
        /// <remarks>Generates a unique <see cref="Id"/> by stripping invalid chars of the name, and making it unique by adding extra characters</remarks>
        protected SourceProvider(IndexerState state, string name)
        {
            if (state == null)
                throw new ArgumentNullException("state");

            _state = state;
            _id = name;
        }

        /// <summary>
        /// Gets the <see cref="SourceProvider"/> id; used for creating references
        /// </summary>
        /// <remarks>An Id starts with a letter followed by letters and digits</remarks>
        public string Id
        {
            get { return _id; }
        }

        /// <summary>
        /// 
        /// </summary>
        public virtual string Name
        {
            get { return null; }
        }

        /// <summary>
        /// Gets a reference to the <see cref="IndexerState"/>
        /// </summary>
        /// <value>The value passed to the constructor</value>
        protected IndexerState State
        {
            get { return _state; }
        }

        /// <summary>
        /// Gets the number of variables the provider needs to encode entries
        /// </summary>
        /// <value>The number of variable the provider requires for encoding the per-sourcefile data of the sourcelocation</value>
        /// <remarks>This value must remain constant. The provider may use <c>%VAR3%</c> upto <c>%VARx%</c> where x = <see cref="SourceEntryVariableCount"/>+2; 
        /// All variables above this number are reserved for the framework</remarks>
        public abstract int SourceEntryVariableCount
        {
            get;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="writer"></param>
        public abstract void WriteEnvironment(StreamWriter writer);
    }
}
