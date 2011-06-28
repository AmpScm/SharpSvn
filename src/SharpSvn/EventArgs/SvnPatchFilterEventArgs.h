// $Id: SvnBlameEventArgs.h 1704 2011-06-21 18:30:38Z rhuijben $
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

	public ref class SvnPatchFilterEventArgs : public SvnCancelEventArgs
	{
		const char *_canon_path;
		const char *_patched_path;
		const char *_reject_path;
		SvnClient^ _client;
		AprPool^ _pool;
		bool _filtered;
		String^ _internalPath;
		String^ _path;
		String^ _rejectPath;
		String^ _resultPath;

	internal:
		SvnPatchFilterEventArgs(const char *canon_path_from_patchfile, const char *patched_abspath, const char *reject_abspath, SvnClient^ client, AprPool^ pool)
		{
			_canon_path = canon_path_from_patchfile;
			_patched_path = patched_abspath;
			_reject_path = reject_abspath;
			_client = client;
			_pool = pool;
		}

	public:
		/// <summary>The path to the node as stored in the patch file, kept in Subversion canonical format</summary>
		property String^ CanonicalPath
		{
			String^ get()
			{
				if (!_internalPath && _canon_path && _pool)
					_internalPath = SvnBase::Utf8_PtrToString(_canon_path);

				return _internalPath;
			}
		}

		/// <summary>The path to the node as stored in the patch file, normalized to Windows style</summary>
		property String^ Path
		{
			String^ get()
			{
				if (!_path && _canon_path && _pool)
					_path = SvnBase::Utf8_PathPtrToString(_canon_path, _pool);

				return _path;
			}
		}

		/// <summary>Path to a temporary file containing the rejected hunks of the patch</summary>
		property String^ RejectPath
		{
			String^ get()
			{
				if (!_rejectPath && _reject_path && _pool)
					_rejectPath = SvnBase::Utf8_PathPtrToString(_reject_path, _pool);

				return _rejectPath;
			}
		}

		/// <summary>Path to a temporary file containing the result of the patch</summary>
		property String^ ResultPath
		{
			String^ get()
			{
				if (!_resultPath && _patched_path && _pool)
					_resultPath = SvnBase::Utf8_PathPtrToString(_patched_path, _pool);

				return _resultPath;
			}
		}

	public:
		/// <summary>Gets or sets a boolean indicating whether this file should be filtered from the patch handling</summary>
		property bool Filtered
		{
			bool get()
			{
				return _filtered;
			}
			void set(bool value)
			{
				_filtered = value;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					// Use all properties to get them cached in .Net memory					
					GC::KeepAlive(CanonicalPath);
					GC::KeepAlive(Path);
					GC::KeepAlive(RejectPath);
					GC::KeepAlive(ResultPath);
				}
			}
			finally
			{
				_canon_path = nullptr;
				_patched_path = nullptr;
				_reject_path = nullptr;
				_client = nullptr;
				_pool = nullptr;
				__super::Detach(keepProperties);
			}
		}

	};
}
