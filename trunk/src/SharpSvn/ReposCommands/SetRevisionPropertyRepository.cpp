#include "stdafx.h"
#include "SvnAll.h"
#include "ReposArgs/SetRevisionPropertyRepository.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnRepositoryClient::SetRevisionProperty(String^ repositoryPath, SvnRevision^ revision, String^ propertyName, String^ value)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!value)
		throw gcnew ArgumentNullException("value");
	else if (revision->RevisionType != SvnRevisionType::Head && revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return SetRevisionProperty(repositoryPath, revision, propertyName, value, gcnew SvnSetRevisionPropertyRepositoryArgs());
}

bool SvnRepositoryClient::SetRevisionProperty(String^ repositoryPath, SvnRevision^ revision, String^ propertyName, ICollection<Byte>^ bytes)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!bytes)
		throw gcnew ArgumentNullException("bytes");
	else if (revision->RevisionType != SvnRevisionType::Head && revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return SetRevisionProperty(repositoryPath, revision, propertyName, bytes, gcnew SvnSetRevisionPropertyRepositoryArgs());
}

bool SvnRepositoryClient::SetRevisionProperty(String^ repositoryPath, SvnRevision^ revision, String^ propertyName, String^ value, SvnSetRevisionPropertyRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!value)
		throw gcnew ArgumentNullException("value");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (revision->RevisionType != SvnRevisionType::Head && revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	AprPool pool(%_pool);

	return InternalSetRevisionProperty(
		repositoryPath,
		revision,
		propertyName,
		// Subversion does no normalization on the property value; so we have to do this before sending it
		// to the server
		pool.AllocPropertyValue(value, propertyName),
		args,
		%pool);
}

bool SvnRepositoryClient::SetRevisionProperty(String^ repositoryPath, SvnRevision^ revision, String^ propertyName, ICollection<Byte>^ bytes, SvnSetRevisionPropertyRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!bytes)
		throw gcnew ArgumentNullException("bytes");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (revision->RevisionType != SvnRevisionType::Head && revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	AprPool pool(%_pool);

	array<Byte> ^byteArray = dynamic_cast<array<Byte>^>(bytes);

	if (!byteArray)
	{
		byteArray = gcnew array<Byte>(bytes->Count);

		bytes->CopyTo(byteArray, 0);
	}

	return InternalSetRevisionProperty(
		repositoryPath,
		revision,
		propertyName,
		pool.AllocSvnString(byteArray),
		args,
		%pool);
}

bool SvnRepositoryClient::DeleteRevisionProperty(String^ repositoryPath, SvnRevision^ revision, String^ propertyName)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (revision->RevisionType != SvnRevisionType::Head && revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return DeleteRevisionProperty(repositoryPath, revision, propertyName, gcnew SvnSetRevisionPropertyRepositoryArgs());
}

bool SvnRepositoryClient::DeleteRevisionProperty(String^ repositoryPath, SvnRevision^ revision, String^ propertyName, SvnSetRevisionPropertyRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (revision->RevisionType != SvnRevisionType::Head && revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	AprPool pool(%_pool);

	return InternalSetRevisionProperty(
		repositoryPath,
		revision,
		propertyName,
		nullptr,
		args,
		%pool);
}

bool SvnRepositoryClient::InternalSetRevisionProperty(String^ repositoryPath, SvnRevision^ revision, String^ propertyName, const svn_string_t* value, SvnSetRevisionPropertyRepositoryArgs^ args, AprPool^ pool)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool subpool(pool);
	ArgsStore store(this, args, %subpool);

	svn_error_t *r;
	svn_repos_t *repos;

	if (r = svn_repos_open(&repos, subpool.AllocDirent(repositoryPath), subpool.Handle))
		return args->HandleResult(this, r);

	svn_revnum_t rev;

	switch(revision->RevisionType)
	{
	case SvnRevisionType::Head:
		if (r = svn_fs_youngest_rev(&rev, svn_repos_fs(repos), subpool.Handle))
			return args->HandleResult(this, r);
		break;
	case SvnRevisionType::Number:
		rev = (svn_revnum_t)revision->Revision;
		break;
	default:
		throw gcnew InvalidOperationException();
	}

	return args->HandleResult(this,
							  svn_repos_fs_change_rev_prop3(
										repos,
										rev,
										args->Author ? subpool.AllocString(args->Author) : nullptr,
										subpool.AllocString(propertyName),
										value,
										args->CallPreRevPropChangeHook,
										args->CallPostRevPropChangeHook,
										nullptr,
										nullptr,
										subpool.Handle));
}