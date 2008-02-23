// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnDiffSummaryEventArgs : public SvnCancelEventArgs
	{
		const svn_client_diff_summarize_t *_diffSummary;
		String^ _path;
		initonly bool _propertiesChanged;
		initonly SvnNodeKind _nodeKind;
		initonly SvnDiffKind _diffKind;
	internal:
		SvnDiffSummaryEventArgs(const svn_client_diff_summarize_t *diffSummary)
		{
			if (!diffSummary)
				throw gcnew ArgumentNullException("diffSummary");

			_diffSummary = diffSummary;
			_propertiesChanged = (0 != diffSummary->prop_changed);
			_nodeKind = (SvnNodeKind)diffSummary->node_kind;
			_diffKind = (SvnDiffKind)diffSummary->summarize_kind;
		}


	public:
		property String^ Path
		{
			String^ get()
			{
				if (!_path && _diffSummary)
					_path = SvnBase::Utf8_PtrToString(_diffSummary->path);

				return _path;
			}
		}

		property bool PropertiesChanged
		{
			bool get()
			{
				return _propertiesChanged;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property SvnDiffKind DiffKind
		{
			SvnDiffKind get()
			{
				return _diffKind;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			String^ path = Path;
			return path ? path->GetHashCode() : __super::GetHashCode();
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
				}
			}
			finally
			{
				_diffSummary = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};


}