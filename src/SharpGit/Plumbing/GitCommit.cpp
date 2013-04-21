#include "stdafx.h"

#include "GitCommit.h"
#include "GitRepository.h"
#include "../GitClient/GitCommitCmd.h"

using namespace SharpGit::Plumbing;
using namespace SharpGit;

struct git_commit {};

bool GitCommit::IsDisposed::get()
{
	return !_commit || _repository->IsDisposed;
}


GitSignature^ GitCommit::Author::get()
{
	if (!_author && !IsDisposed)
	{
		const git_signature *s = git_commit_author(_commit);
		_author = gcnew GitSignature(s);
	}
	return _author;
}

GitSignature^ GitCommit::Committer::get()
{
	if (!_committer && !IsDisposed)
	{
		const git_signature *s = git_commit_committer(_commit);
		_committer = gcnew GitSignature(s);
	}
	return _committer;
}

String^ GitCommit::LogMessage::get()
{
	if (!_logMessage && !IsDisposed)
	{
		const char *encoding = git_commit_message_encoding(_commit);
		const char *msg = git_commit_message(_commit);

		if (!msg || !msg[0])
			return _logMessage = "";

		if (!encoding || !_stricmp(encoding, "UTF-8"))
		{
			// The simple case
			return _logMessage = Utf8_PtrToString(msg);
		}

		try
		{
			System::Text::Encoding ^enc = System::Text::Encoding::GetEncoding(gcnew String(encoding));

			_logMessage = gcnew String(msg, 0, strlen(msg), enc);
		}
		catch (ArgumentException^)
		{
			const char *c = msg;

			while (*c > 0 && *c < 128)
				c++;

			if (*c == 0)
				return _logMessage = gcnew String(msg); // Plain ASCII

			throw;
		}
	}

	return _logMessage;
}
