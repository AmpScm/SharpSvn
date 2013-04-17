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

	public ref class SvnListChangeListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		const char* _pChangeList;
		String^ _changelist;

	internal:
		SvnListChangeListEventArgs(const char *path, const char *changelist)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");

			_path = SvnBase::Utf8_PtrToString(path)->Replace('/', System::IO::Path::DirectorySeparatorChar);
			_pChangeList = changelist;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property String^ ChangeList
		{
			String^ get()
			{
				if (!_changelist && _pChangeList)
					_changelist = SvnBase::Utf8_PtrToString(_pChangeList);

				return _changelist;
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
					GC::KeepAlive(ChangeList);
				}
			}
			finally
			{
				_pChangeList = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}