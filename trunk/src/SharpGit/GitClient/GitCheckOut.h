#pragma once

#include "GitClientContext.h"

namespace SharpGit {

	public ref class GitClientRemoteArgs abstract : public GitClientArgs
	{
		bool _synchronous;

	public:
		property bool Synchronous
		{
			bool get()
			{
				return _synchronous;
			}
			void set(bool value)
			{
				_synchronous = value;
			}
		}
	};

	public ref class GitCheckOutArgs : public GitClientRemoteArgs
	{
	internal:
		const git_checkout_opts * MakeCheckOutOptions(GitPool ^pool);

	private:
		bool _dryRun;
		bool _create;
		bool _overwrite;
		bool _failOnConflicts;
		bool _removeUnversioned;
		bool _removeIgnored;
		bool _updateOnly;
		bool _skipIndexUpdate;
		bool _skipInitialRefresh;

	public:
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

		property bool Create
		{
			bool get()
			{
				return _create;
			}
			void set(bool value)
			{
				_create = value;
			}
		}

		property bool OverwriteExisting
		{
			bool get()
			{
				return _overwrite;
			}
			void set(bool value)
			{
				_overwrite = value;
			}
		}

		property bool FailOnConflicts
		{
			bool get()
			{
				return _failOnConflicts;
			}
			void set(bool value)
			{
				_failOnConflicts = value;
			}
		}

		property bool RemoveUnversioned
		{
			bool get()
			{
				return _removeUnversioned;
			}
			void set(bool value)
			{
				_removeUnversioned = value;
			}
		}

		property bool RemoveIgnored
		{
			bool get()
			{
				return _removeIgnored;
			}
			void set(bool value)
			{
				_removeIgnored = value;
			}
		}

		property bool UpdateOnly
		{
			bool get()
			{
				return _updateOnly;
			}
			void set(bool value)
			{
				_updateOnly = value;
			}
		}

		property bool SkipIndexUpdate
		{
			bool get()
			{
				return _skipIndexUpdate;
			}
			void set(bool value)
			{
				_skipIndexUpdate = value;
			}
		}

		property bool SkipInitialRefresh
		{
			bool get()
			{
				return _skipInitialRefresh;
			}
			void set(bool value)
			{
				_skipInitialRefresh = value;
			}
		}
	};
};