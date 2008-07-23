// $Id: GetWorkingCopyState.h 500 2008-03-26 17:47:22Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	
	public ref class SvnWorkingCopyEntriesArgs : public SvnClientArgs
	{
		bool _hideSome;
	public:
		event EventHandler<SvnWorkingCopyEntryEventArgs^>^ Entry;

	protected public:
		virtual void OnEntry(SvnWorkingCopyEntryEventArgs^ e)
		{
			Entry(this, e);
		}

	public:
		property bool ShowHidden
		{
			bool get()
			{
				return !_hideSome;
			}
			void set(bool value)
			{
				_hideSome = !value;
			}
		}


	public:
		/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
		virtual property SvnCommandType CommandType
		{
			SvnCommandType get() override
			{
				return SvnCommandType::GetWorkingCopyEntries;
			}
		}
	
	};

}