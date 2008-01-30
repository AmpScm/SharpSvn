// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::CheckOut(SvnUriTarget^, String^, SvnCheckOutArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCheckOutArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreExternals;
		SvnRevision^ _revision;
		bool _allowUnversionedObstructions;

	public:
		SvnCheckOutArgs()
		{
			_depth = SvnDepth::Unknown;
			_revision = SvnRevision::Head;
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

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property bool AllowUnversionedObstructions
		{
			bool get()
			{
				return _allowUnversionedObstructions;
			}
			void set(bool value)
			{
				_allowUnversionedObstructions = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Update(String^, SvnUpdateArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnUpdateArgs : public SvnClientArgsWithConflict
	{
		SvnDepth _depth;
		bool _ignoreExternals;
		bool _allowUnversionedObstructions;
		SvnRevision^ _revision;
		bool _keepDepth;

	public:
		SvnUpdateArgs()
		{
			_depth = SvnDepth::Unknown;
			_revision = SvnRevision::Head;
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

		property bool KeepDepth
		{
			bool get()
			{
				return _keepDepth;
			}
			void set(bool value)
			{
				_keepDepth = value;
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property bool AllowUnversionedObstructions
		{
			bool get()
			{
				return _allowUnversionedObstructions;
			}
			void set(bool value)
			{
				_allowUnversionedObstructions = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Switch(String^, SvnUriTarget^, SvnSwitchArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnSwitchArgs : public SvnClientArgsWithConflict
	{
		SvnDepth _depth;
		SvnRevision^ _revision;
		bool _allowUnversionedObstructions;
		bool _ignoreExternals;
		bool _keepDepth;

	public:
		SvnSwitchArgs()
		{
			_depth = SvnDepth::Infinity;
			_revision = SvnRevision::None;
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

		property bool KeepDepth
		{
			bool get()
			{
				return _keepDepth;
			}
			void set(bool value)
			{
				_keepDepth = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property bool AllowUnversionedObstructions
		{
			bool get()
			{
				return _allowUnversionedObstructions;
			}
			void set(bool value)
			{
				_allowUnversionedObstructions = value;
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}
	};

	public ref class SvnMergeBaseArgs abstract : public SvnClientArgsWithConflict
	{
		IList<String^>^ _mergeArguments;
	protected:
		SvnMergeBaseArgs()
		{
		}

	public:
		property IList<String^>^ MergeArguments
		{
			IList<String^>^ get()
			{
				return _mergeArguments;
			}
			void set(IList<String^>^ value)
			{
				if(value)
					_mergeArguments = (gcnew System::Collections::Generic::List<String^>(value))->AsReadOnly();
				else
					_mergeArguments = nullptr;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::DiffMerge(String^, SvnTarget^, SvnTarget^, SvnDiffMergeArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDiffMergeArgs : public SvnMergeBaseArgs
	{
		SvnDepth _depth;
		bool _ignoreAncestry;
		bool _force;
		bool _recordOnly;
		bool _dryRun;		
	public:
		SvnDiffMergeArgs()
		{
			_depth = SvnDepth::Unknown;
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

		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}

		property bool RecordOnly
		{
			bool get()
			{
				return _recordOnly;
			}
			void set(bool value)
			{
				_recordOnly = value;
			}
		}

		property bool DryRun
		{
			bool get()
			{
				return _dryRun;
			}
			void set(bool value)
			{
				_dryRun = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Merge(String^,SvnTarget^, SvnRevisionRange^, SvnMergeArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnMergeArgs : public SvnMergeBaseArgs
	{
		SvnDepth _depth;
		bool _ignoreAncestry;
		bool _force;
		bool _recordOnly;
		bool _dryRun;
	public:
		SvnMergeArgs()
		{
			_depth = SvnDepth::Unknown;
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

		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}

		property bool RecordOnly
		{
			bool get()
			{
				return _recordOnly;
			}
			void set(bool value)
			{
				_recordOnly = value;
			}
		}

		property bool DryRun
		{
			bool get()
			{
				return _dryRun;
			}
			void set(bool value)
			{
				_dryRun = value;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::ReintegrationMerge(String^,SvnTarget^, SvnReintegrationMergeArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnReintegrationMergeArgs : public SvnMergeBaseArgs
	{
		bool _force;
		bool _dryRun;
	public:
		SvnReintegrationMergeArgs()
		{
		}

		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}

		property bool DryRun
		{
			bool get()
			{
				return _dryRun;
			}
			void set(bool value)
			{
				_dryRun = value;
			}
		}		
	};
};

