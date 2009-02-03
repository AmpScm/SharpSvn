// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnDiffSummaryEventArgs : public SvnCancelEventArgs
	{
		const svn_client_diff_summarize_t *_diffSummary;
		const char* _pFromUri;
		const char* _pToUri;
		AprPool^ _pool;

		Uri^ _fromUri;
		Uri^ _toUri;
		String^ _path;
		initonly bool _propertiesChanged;
		initonly SvnNodeKind _nodeKind;
		initonly SvnDiffKind _diffKind;
	internal:
		SvnDiffSummaryEventArgs(const svn_client_diff_summarize_t *diffSummary, const char* fromUri, const char* toUri, AprPool^ pool)
		{
			if (!diffSummary)
				throw gcnew ArgumentNullException("diffSummary");
			else if (!fromUri)
				throw gcnew ArgumentNullException("fromUri");
			else if (!toUri)
				throw gcnew ArgumentNullException("toUri");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_diffSummary = diffSummary;
			_pFromUri = fromUri;
			_pToUri = toUri;
			_pool = pool;

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

	public:
		property Uri^ FromUri
		{
			Uri^ get()
			{
				if (!_fromUri && _pFromUri && _diffSummary && _pool)
					_fromUri = SvnBase::Utf8_PtrToUri(svn_path_join(_pFromUri, _diffSummary->path, _pool->Handle), NodeKind);

				return _fromUri;
			}
		}

		property Uri^ ToUri
		{
			Uri^ get()
			{
				if (!_toUri && _pToUri && _diffSummary && _pool)
					_toUri = SvnBase::Utf8_PtrToUri(svn_path_join(_pToUri, _diffSummary->path, _pool->Handle), NodeKind);

				return _toUri;
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
					GC::KeepAlive(FromUri);
					GC::KeepAlive(ToUri);
				}
			}
			finally
			{
				_diffSummary = nullptr;
				_pFromUri = nullptr;
				_pToUri = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};


}