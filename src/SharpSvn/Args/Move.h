// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Move(String^,String^,SvnMoveArgs^)" /> and
	/// <see cref="SvnClient::RemoteMove(Uri^,Uri^,SvnMoveArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnMoveArgs : public SvnClientArgsWithCommit
	{
		bool _force;
		bool _moveAsChild;
		bool _makeParents;
	public:
		SvnMoveArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Move;
			}
		}

		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}

		property bool AlwaysMoveAsChild
		{
			bool get()
			{
				return _moveAsChild;
			}
			void set(bool value)
			{
				_moveAsChild = value;
			}
		}

		/// <summary>Creates parent directories if required</summary>
		property bool CreateParents
		{
			bool get()
			{
				return _makeParents;
			}
			void set(bool value)
			{
				_makeParents = value;
			}
		}

		[Obsolete("Use .CreateParents")]
		property bool MakeParents
		{
			bool get()
			{
				return CreateParents;
			}
			void set(bool value)
			{
				CreateParents = value;
			}
		}
	};

}
