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

#include "AprPool.h"
#include "SvnBase.h"

namespace SharpSvn {
	namespace Diff {
		using namespace System::Collections::Generic;

		generic<typename TToken>
		where TToken : ref class
			public ref class SvnDiffSource abstract : public System::IDisposable, System::Collections::Generic::IEnumerable<TToken>
		{
		protected:
			SvnDiffSource<TToken>();
		private:
			~SvnDiffSource();

		public:
			virtual IEnumerator<TToken>^ GetEnumerator() abstract;

		private:
			virtual System::Collections::IEnumerator^ OldGetEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}
		};

		generic<typename TToken> where TToken : ref class
			public ref class SvnDiff : public SvnBase
		{
			AprPool _pool;
			initonly AprBaton<SvnDiff<TToken>^>^ _diffBaton;
			svn_diff_t* _diff;
			~SvnDiff();

		internal:
			property AprPool^ Pool
			{
				AprPool^ get()
				{
					return %_pool;
				}
			}

			property svn_diff_t *DiffHandle
			{
				svn_diff_t *get()
				{
					return _diff;
				}
			}

		private:
			array<SvnDiffSource<TToken>^>^ _origins;
			array<IEnumerator<TToken>^>^ _walkers;
			Dictionary<int, TToken>^ _ItoT;
			Dictionary<TToken, int>^ _TtoI;
			IComparer<TToken>^ _comparer;

		public:
			SvnDiff(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified);
			SvnDiff(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified, SvnDiffSource<TToken>^ latest);
			SvnDiff(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified, SvnDiffSource<TToken>^ latest, SvnDiffSource<TToken>^ ancestor);

		internal:
			SvnDiff(svn_diff_t* diff, AprPool^ pool);

		public:
			/// <summary>Gets a boolean indicating whether the merge result contains conflicts</summary>
			property bool HasConflicts
			{
				bool get()
				{
					return _diff ? (svn_diff_contains_conflicts(_diff) != 0) : false;
				}
			}

			/// <summary>Gets a boolean indicating whether the merge result contains differences</summary>
			property bool HasDifferences
			{
				bool get()
				{
					return _diff ? (svn_diff_contains_diffs(_diff) != 0) : false;
				}
			}

			/// <summary>Gets the comparer used to compare the tokens</summary>
			/// <remarks>The instance returned is creatd by <see cref="CreateComparer" /></remarks>
			property IComparer<TToken>^ TokenComparer
			{
				IComparer<TToken>^ get()
				{
					if (!_comparer)
						_comparer = CreateComparer();

					return _comparer;
				}
			}

		protected:
			/// <summary>Creates the comparer used to compare the tokens. Defaults to Comparer&lt;TToken&gt;::Default</summary>
			virtual IComparer<TToken>^ CreateComparer()
			{
				return Comparer<TToken>::Default;
			}

		private:
			void Init(SvnDiffSource<TToken>^ original, SvnDiffSource<TToken>^ modified, SvnDiffSource<TToken>^ latest, SvnDiffSource<TToken>^ ancestor);
		};
	}
}