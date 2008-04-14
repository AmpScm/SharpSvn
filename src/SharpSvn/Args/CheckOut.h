// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

/// <summary>Extended Parameter container of <see cref="SvnClient::CheckOut(SvnUriTarget^, String^, SvnCheckOutArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCheckOutArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreExternals;
		SvnRevision^ _revision;
		bool _allowObstructions;

	public:
		SvnCheckOutArgs()
		{
			_depth = SvnDepth::Unknown;
			_revision = SvnRevision::None;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::CheckOut;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}

		/// <summary>Gets or sets the revision to load. Defaults to the peg revision or Head</summary>
		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		/// <summary>Gets or sets the AllowObsstructions value</summary>
		/// <remarks>
		/// <para> If AllowObstructions is TRUE then the update tolerates
		/// existing unversioned items that obstruct added paths from @a URL.  Only
		/// obstructions of the same type (file or dir) as the added item are
		/// tolerated.  The text of obstructing files is left as-is, effectively
		/// treating it as a user modification after the update.  Working
		/// properties of obstructing items are set equal to the base properties.</para>
		/// <para>If AllowObstructions is FALSE then the update will abort
		/// if there are any unversioned obstructing items
		/// </para>
		/// </remarks>
		property bool AllowObstructions
		{
			bool get()
			{
				return _allowObstructions;
			}
			void set(bool value)
			{
				_allowObstructions = value;
			}
		}
	};

}
