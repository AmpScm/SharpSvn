// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container for SvnClient.List</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnListArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		SvnDirEntryItems _entryItems;
		bool _fetchLocks;

	public:
		event EventHandler<SvnListEventArgs^>^ List;

	protected public:
		virtual void OnList(SvnListEventArgs^ e)
		{
			List(this, e);
		}

	public:
		SvnListArgs()
		{
			_depth = SvnDepth::Children;
			_revision = SvnRevision::None;
			_entryItems = SvnDirEntryItems::SvnListDefault;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::List;
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

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property SvnDirEntryItems RetrieveEntries
		{
			SvnDirEntryItems get()
			{
				return _entryItems;
			}

			void set(SvnDirEntryItems value)
			{
				_entryItems = value;
			}
		}

		property bool RetrieveLocks
		{
			bool get()
			{
				return _fetchLocks;
			}
			void set(bool value)
			{
				_fetchLocks = value;
			}
		}

	internal:
		String^ _queryRoot;
		Uri^ _repositoryRoot;
		void Prepare(SvnTarget^ target)
		{
			_repositoryRoot = nullptr;

			SvnUriTarget^ uriTarget = dynamic_cast<SvnUriTarget^>(target);
			if (uriTarget)
				_queryRoot = uriTarget->SvnTargetName;			
			else
				_queryRoot = nullptr;
		}

		Uri^ CalculateRepositoryRoot(const char* abs_path)
		{
			if(_repositoryRoot || !_queryRoot)
				return _repositoryRoot;

			String^ qr = _queryRoot;
			_queryRoot = nullptr; // Only parse in the first call, which matches the exact request

			String^ absPath = SvnBase::Utf8_PtrToString(abs_path+1);
			String^ path;

			if(absPath->Length > 0)
				path = SvnBase::PathToUri(SvnBase::Utf8_PtrToString(abs_path+1))->ToString(); // Skip the initial '/'
			else
				path = "";

			Uri^ rt;
			if(path->Length == 0)
			{
				// Special case. The passed path is the solution root; EndsWith would always succeed
				if (qr->Length > 0 && qr[qr->Length-1] != '/')
					_repositoryRoot = gcnew Uri(qr + "/");
				else
					_repositoryRoot = gcnew Uri(qr);
			}
			else if (Uri::TryCreate(qr, UriKind::Absolute, rt))
			{
				int n = path->Length;

				int up = 0;
				while(--n > 0)
				{
					if (path[n] == '/')
						up++;
				}

				if(qr[qr->Length-1] == '/')
					up++;
				else if(up == 0)
					rt = gcnew Uri(rt, "./");

				while(up-- > 0)
					rt = gcnew Uri(rt, "../");

				_repositoryRoot = rt;
			}

			return _repositoryRoot;
		}
	};

}
