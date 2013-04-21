#pragma once

#include "GitClientContext.h"

namespace SharpGit {

	public ref class GitSignature : public GitBase
	{
		DateTime _when;
		String ^_name;
		String ^_email;
	internal:
		const git_signature * Alloc(GitPool ^pool)
		{
			git_signature *sig = (git_signature *)pool->Alloc(sizeof(*sig));

			// TODO: Store proper time in local tz
			sig->when.time = When.ToFileTime(); // -> Convert to unix time!!!
			sig->name = const_cast<char*>(pool->AllocString(Name));
			sig->email = const_cast<char*>(pool->AllocString(EmailAddress));

			return sig;
		}

	public:
		property DateTime When
		{
			DateTime get()
			{
				return _when;
			}
			void set(DateTime value)
			{
				if (value.Kind != System::DateTimeKind::Unspecified)
					_when = value.ToUniversalTime();
				else
					_when = DateTime(value.Ticks, System::DateTimeKind::Utc);
			}
		}

		property String^ Name
		{
			String^ get()
			{
				return _name;
			}
			void set(String ^value)
			{
				_name = value;
			}
		}

		property String^ EmailAddress
		{
			String^ get()
			{
				return _email;
			}
			void set(String ^value)
			{
				_email = value;
			}
		}
	};

	public ref class GitCommitArgs : public GitClientArgs
	{
		String ^_logMessage;
		String ^_updateReference;
		GitSignature ^_author;
		GitSignature ^_committer;

	public:
		GitCommitArgs()
		{
			UpdateReference = "HEAD";
			_author = gcnew GitSignature();
			_committer = gcnew GitSignature();
			DateTime now = DateTime::UtcNow;
			_author->When = now;
			_committer->When = now;
		}

	public:
		property String ^ LogMessage
		{
			String ^get()
			{
				return _logMessage;
			}
			void set(String ^value)
			{
				_logMessage = value;
			}
		}

		property String ^ UpdateReference
		{
			String^ get()
			{
				return _updateReference;
			}
			void set(String ^value)
			{
				_updateReference = value;
			}
		}

		property GitSignature^ Author
		{
			GitSignature^ get()
			{
				return _author;
			}
		}

		property GitSignature^ Committer
		{
			GitSignature^ get()
			{
				return _committer;
			}
		}
	};
}