#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnHotCopyRepositoryArgs : public SvnRepositoryClientArgs
	{
		bool _dontCleanLogFiles;
		bool _incremental;

	public:
		SvnHotCopyRepositoryArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

	public:
		/// <summary>Gets or sets a value indicating whether to clean the database logfiles after the hotcopy</summary>
		property bool CleanLogFiles
		{
			bool get()
			{
				return !_dontCleanLogFiles;
			}
			void set(bool value)
			{
				_dontCleanLogFiles = !value;
			}
		}

		property bool Incremental
		{
			bool get()
			{
				return _incremental;
			}
			void set(bool value)
			{
				_incremental = value;
			}
		}
	};
}