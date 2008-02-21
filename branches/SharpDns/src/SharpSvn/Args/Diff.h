// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

/// <summary>Extended Parameter container of <see cref="SvnClient" />'s Diff command</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDiffArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreAncestry;
		bool _noDeleted;
		bool _ignoreContentType;
		String^ _headerEncoding;
		String^ _relativeFrom;
		SvnCommandLineArgumentCollection^ _diffArguments;
		SvnChangeListCollection^ _changelists;
		Stream^ _errorStream;

	public:
		SvnDiffArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Diff;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
			}
		}

		property bool IgnoreAncestry
		{
			bool get()
			{
				return _ignoreAncestry;
			}
			void set(bool value)
			{
				_ignoreAncestry = value;
			}
		}

		property bool NoDeleted
		{
			bool get()
			{
				return _noDeleted;
			}
			void set(bool value)
			{
				_noDeleted = value;
			}
		}

		property bool IgnoreContentType
		{
			bool get()
			{
				return _ignoreContentType;
			}
			void set(bool value)
			{
				_ignoreContentType = value;
			}
		}

		property String^ HeaderEncoding
		{
			String^ get()
			{
				return _headerEncoding ? _headerEncoding : "utf-8";
			}
			void set(String^ value)
			{
				_headerEncoding = value;
			}
		}

		property SvnCommandLineArgumentCollection^ DiffArguments
		{
			SvnCommandLineArgumentCollection^ get()
			{
				if (!_diffArguments)
					_diffArguments = gcnew SvnCommandLineArgumentCollection();

				return _diffArguments;
			}
		}

		property String^ RelativeToPath
		{
			String^ get()
			{
				return _relativeFrom;
			}
			void set(String^ value)
			{
				if (String::IsNullOrEmpty(value))
					_relativeFrom = nullptr;
				else
					_relativeFrom = value;
			}
		}

		/// <summary>Gets the list of changelist-names</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if (!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}

		property Stream^ ErrorStream
		{
			Stream^ get()
			{
				return _errorStream;
			}
			void set(Stream^ value)
			{
				_errorStream = value;
			}
		}
	};

}
