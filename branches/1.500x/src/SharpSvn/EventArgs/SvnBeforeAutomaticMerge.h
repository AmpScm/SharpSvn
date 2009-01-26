// $Id: AprArray.h 534 2008-04-14 22:52:59Z rhuijben $
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

	public ref class SvnBeforeAutomaticMergeEventArgs sealed : public SvnEventArgs
	{
		initonly bool _wasBinary;
		initonly bool _dryRun;
		bool _isBinary;
		const char* _pMergeTarget;
		String^ _path;
		String^ _fullPath;
		AprPool^ _pool;

	internal:
		SvnBeforeAutomaticMergeEventArgs(bool isBinary, bool dryRun, const char* merge_target, AprPool ^pool)
		{
			if (!merge_target)
				throw gcnew ArgumentNullException("merge_target");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_isBinary = _wasBinary = isBinary;
			_dryRun = dryRun;
			_pMergeTarget = merge_target;
			_pool = pool;
		}

	public:
		/// <summary>Gets the path of the file to merge</summary>
		property String^ Path
		{
			String^ get()
			{
				if (!_path && _pMergeTarget && _pool)
					_path = SvnBase::Utf8_PathPtrToString(_pMergeTarget, _pool);

				return _path;
			}
		}

		/// <summary>Gets the full path of the file to merge</summary>
		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && Path)
					_fullPath = SvnTools::GetNormalizedFullPath(Path);

				return _fullPath;
			}
		}

		/// <summary>Gets a boolean indicating whether the merge will be performed or the merge is just tested</summary>
		property bool DryRun
		{
			bool get()
			{
				return _dryRun;
			}
		}

		/// <summary>Gets a boolean indicating whether the file was originaly threated as binary by Subversion</summary>
		property bool WasBinary
		{
			bool get()
			{
				return _wasBinary;
			}
		}

		/// <summary>Gets or sets a boolean indicating whether Subversion should handle the file as binary</summary>
		/// <remarks>You can only change the value of IsBinary when WasBinary is false</remarks>
		property bool IsBinary
		{
			bool get()
			{
				return _isBinary;
			}
			void set(bool value)
			{
				if (_wasBinary && !value)
					throw gcnew InvalidOperationException("Can't remove binary state");

				_isBinary = value;
			}
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(FullPath);
				}
			}
			finally
			{
				_pool = nullptr;
				_pMergeTarget = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
}
