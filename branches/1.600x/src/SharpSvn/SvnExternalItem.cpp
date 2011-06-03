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

#include "stdafx.h"

#include "SvnAll.h"
#include "SvnExternalItem.h"

using namespace SharpSvn;
using System::Text::StringBuilder;

SvnExternalItem::SvnExternalItem(String^ targetName, String^ url)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");
	else if (String::IsNullOrEmpty(url))
		throw gcnew ArgumentNullException("url");

	_target = targetName;
	_url = url;
	_revision = _pegRevision = SvnRevision::None;
}

SvnExternalItem::SvnExternalItem(String^ targetName, Uri^ uri)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");
	else if (!uri)
		throw gcnew ArgumentNullException("uri");

	_target = targetName;
	_url = uri->AbsoluteUri;
	_revision = _pegRevision = SvnRevision::None;
}

SvnExternalItem::SvnExternalItem(String^ targetName, String^ url, SvnRevision^ revision)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");
	else if (String::IsNullOrEmpty(url))
		throw gcnew ArgumentNullException("url");
	else if(revision && revision != SvnRevision::None && !revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	_target = targetName;
	_url = url;
	_revision = _pegRevision = (revision && revision != SvnRevision::Head) ? revision : SvnRevision::None;
}

SvnExternalItem::SvnExternalItem(String^ targetName, Uri^ uri, SvnRevision^ revision)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");
	else if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if(revision && revision != SvnRevision::None && !revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	_target = targetName;
	_url = uri->AbsoluteUri;
	_revision = _pegRevision = (revision && revision != SvnRevision::Head) ? revision : SvnRevision::None;
}

SvnExternalItem::SvnExternalItem(String^ targetName, Uri^ uri, SvnRevision^ revision, SvnRevision^ pegRevision)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");
	else if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!uri->IsAbsoluteUri)
		throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");
	else if(revision && revision != SvnRevision::None && !revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if(pegRevision && pegRevision != SvnRevision::None && !pegRevision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "pegRevision");

	_target = targetName;
	_url = uri->AbsoluteUri;
	_revision = (revision && revision != SvnRevision::Head) ? revision : SvnRevision::None;
	_pegRevision = (pegRevision && pegRevision != SvnRevision::Head) ? pegRevision : SvnRevision::None;
}

SvnExternalItem::SvnExternalItem(String^ targetName, String^ url, SvnRevision^ revision, SvnRevision^ pegRevision)
{
	if (String::IsNullOrEmpty(targetName))
		throw gcnew ArgumentNullException("targetName");
	else if (String::IsNullOrEmpty(url))
		throw gcnew ArgumentNullException("url");
	else if(revision && revision != SvnRevision::None && !revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");
	else if(pegRevision && pegRevision != SvnRevision::None && !pegRevision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "pegRevision");

	_target = targetName;
	_url = url;
	_revision = (revision && revision != SvnRevision::Head) ? revision : SvnRevision::None;
	_pegRevision = (pegRevision && pegRevision != SvnRevision::Head) ? pegRevision : SvnRevision::None;
}

String^ SvnExternalItem::ToString()
{
	return ToString(true);
}
String^ SvnExternalItem::ToString(bool useCompatibleFormat)
{
	StringBuilder^ sb = gcnew StringBuilder();
	WriteTo(sb, useCompatibleFormat);
	return sb->ToString();
}

void SvnExternalItem::WriteTo(System::Text::StringBuilder ^sb, bool useCompatibleFormat)
{
	if (!sb)
		throw gcnew ArgumentNullException("sb");

	bool hasPegRevision = (OperationalRevision != SvnRevision::None) && (OperationalRevision != Revision);
	bool hasNonNumberRevision = (Revision != SvnRevision::None) && (Revision->RevisionType != SvnRevisionType::Number);
	bool urlIsRelative = false;

	System::Uri^ v;

	if (Reference->StartsWith("^/", StringComparison::Ordinal) ||
		Reference->StartsWith("/", StringComparison::Ordinal) ||
		Uri::TryCreate(Reference, UriKind::Relative, v))
	{
		urlIsRelative = true;
	}

	if (useCompatibleFormat && !hasPegRevision && !hasNonNumberRevision && !urlIsRelative)
	{	// Use 1.0-1.4 format

		sb->Append(EnsureSafeAprArgument(Target, true));
		sb->Append(" ");
		if (Revision != SvnRevision::None)
		{
			sb->AppendFormat(System::Globalization::CultureInfo::InvariantCulture, "-r {0} ", Revision->Revision);
		}
		sb->Append(EnsureSafeAprArgument(Reference, true));
	}
	else
	{	// Use 1.5+ format

		if (Revision != SvnRevision::None && Revision != OperationalRevision)
		{
			sb->Append("-r ");
			sb->Append(Revision);
			sb->Append(" ");
		}

		sb->Append(EnsureSafeAprArgument(Reference, true));
		if (OperationalRevision != SvnRevision::None)
		{
			sb->Append("@");
			sb->Append(OperationalRevision);
		}
		sb->Append(" ");
		sb->Append(EnsureSafeAprArgument(Target, true));
	}
}

bool SvnExternalItem::TryParse(String^ value, [Out]array<SvnExternalItem^>^% items)
{
	if (!value)
		throw gcnew ArgumentNullException("value");

	value = value->Trim();
	items = nullptr;

	if (String::IsNullOrEmpty(value))
	{
		items = gcnew array<SvnExternalItem^>(0);
		return true;
	}

	AprPool pool(SmallThreadPool);

	apr_array_header_t* result = nullptr;

	svn_error_t* r = svn_wc_parse_externals_description3(&result, "c:/windows", pool.AllocUnixString(value), false, pool.Handle);

	if (r)
	{
		svn_error_clear(r);
		return false;
	}
	else if(!result || !result->nelts)
	{
		items = gcnew array<SvnExternalItem^>(0);
		return true;
	}

	items = gcnew array<SvnExternalItem^>(result->nelts);

	const svn_wc_external_item2_t** list = (const svn_wc_external_item2_t**)result->elts;
	for (int i = 0; i < result->nelts; i++)
	{
		const svn_wc_external_item2_t* x = list[i];
		items[i] = gcnew SvnExternalItem(Utf8_PtrToString(x->target_dir), Utf8_PtrToString(x->url), SvnRevision::Load(&x->revision), SvnRevision::Load(&x->peg_revision));
	}

	return true;
}

bool SvnExternalItem::TryParse(String^ value, [Out]SvnExternalItem^% item)
{
	if (!value)
		throw gcnew ArgumentNullException("value");

	array<SvnExternalItem^>^ items = nullptr;

	if (!TryParse(value, items) || items->Length != 1)
	{
		item = nullptr;
		return false;
	}
	else
	{
		item = items[0];
		return true;
	}
}

bool SvnExternalItem::Equals(Object^ other)
{
	SvnExternalItem^ item = dynamic_cast<SvnExternalItem^>(other);

	return item && Equals(item);
}

bool SvnExternalItem::Equals(SvnExternalItem^ other)
{
	if (!other)
		return false;

	return	other->Target              == this->Target &&
			other->Reference           == this->Reference &&
			other->Revision            == this->Revision &&
			other->OperationalRevision == this->OperationalRevision;

}

int SvnExternalItem::GetHashCode()
{
	return Target->GetHashCode() ^ (Reference->GetHashCode() << 2);
}
