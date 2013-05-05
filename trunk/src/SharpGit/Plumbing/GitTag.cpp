#include "stdafx.h"

#include "GitRepository.h"
#include "GitObject.h"
#include "GitBlob.h"
#include "GitCommit.h"
#include "GitTag.h"
#include "GitTree.h"

#include "../GitClient/GitCommitCmd.h"
#include "../GitClient/GitTagCmd.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

GitSignature^ GitTag::Tagger::get()
{
	if (!_tagger && !IsDisposed)
		_tagger = gcnew GitSignature(git_tag_tagger(Handle));

	return _tagger;
}

GitObject^ GitTag::Target::get()
{
	if (!_target && !IsDisposed)
	{
		git_object *ob;

		if (0 == git_tag_target(&ob, Handle))
			_target = GitObject::Create(Repository, ob);
	}
	return _target;
}

GitId^ GitTag::TargetId::get()
{
	if (!_targetId && !IsDisposed)
		_targetId = gcnew GitId(git_tag_target_id(Handle));

	return _targetId;
}

bool GitObject::Tag(String ^tagName, GitTagArgs ^args)
{
	GitId ^ignored;
	return Tag(tagName, args, ignored);
}

bool GitObject::Tag(String ^tagName, GitTagArgs ^args, [Out] GitId ^%id)
{
	if (String::IsNullOrEmpty(tagName))
		throw gcnew ArgumentNullException("tagName");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	bool normalize = args->NormalizeLogMessage;
	bool strip = args->StripLogMessageComments;
	const char *msg;
	if (normalize || strip)
	{
		String^ msgString = args->LogMessage ? args->LogMessage : "";
		msgString = msgString->Replace("\r", "");
		msg = pool.AllocString(msgString);
		size_t sz = strlen(msg);
		sz += sz/4 + 4;
		char *result = (char*)pool.Alloc(sz+1);

		int r = git_message_prettify(result, sz, msg, strip);

		if (r < 0)
		{
			int len = git_message_prettify(NULL, 0, msg, strip);

			if (len >= 0)
			{
				result = (char*)pool.Alloc(sz+1);

				r = git_message_prettify(result, sz, msg, strip);
			}
		}

		if (r < 0)
			return args->HandleGitError(this, r);

		msg = result;
	}
	else
		msg = args->LogMessage ? pool.AllocString(args->LogMessage) : "";

	git_oid result;
	int r = git_tag_create(&result, Repository->Handle,
						   pool.AllocString(tagName),
						   Handle,
						   args->Tagger->Alloc(Repository, %pool),
						   msg,
						   args->OverwriteExisting);

	if (r == 0)
		id = gcnew GitId(result);
	else
		id = nullptr;

	return args->HandleGitError(this, r);
}
