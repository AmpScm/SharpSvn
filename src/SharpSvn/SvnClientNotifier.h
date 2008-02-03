// $Id: SvnPathTarget.h 171 2007-10-14 19:38:35Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnClient.h"

namespace SharpSvn {
	using namespace System::IO;

	/// <summary>Subversion Client Context wrapper; base class of objects using client context</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientNotifier : public IDisposable
	{
		initonly SvnClient^ _client;
		initonly SvnClientArgs^ _args;
		initonly TextWriter^ _textWriter;
		bool _afterInitial;

	private:
		bool _receivedSomeChange;
		bool _isCheckout;
		bool _isExport;
		bool _suppressFinalLine;
		bool _sentFirstTxdelta;
		int _inExternal;

	public:
		SvnClientNotifier(SvnClient^ client, System::IO::TextWriter^ tw)
		{
			if(!client)
				throw gcnew ArgumentNullException("client");
			else if(!tw)
				throw gcnew ArgumentNullException("tw");

			_client = client;
			_textWriter = tw;
			_client->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientNotifier::HandleNotify);
			_client->BeforeCommand += gcnew EventHandler<SvnBeforeCommandEventArgs^>(this, &SvnClientNotifier::HandleBeforeCommand);
		}

		SvnClientNotifier(SvnClient^ client, System::Text::StringBuilder^ sb)			
		{
			if(!client)
				throw gcnew ArgumentNullException("client");
			else if(!sb)
				throw gcnew ArgumentNullException("sb");

			_client = client;
			_textWriter = gcnew System::IO::StringWriter(sb);
			_client->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientNotifier::HandleNotify);
			_client->BeforeCommand += gcnew EventHandler<SvnBeforeCommandEventArgs^>(this, &SvnClientNotifier::HandleBeforeCommand);
		}

		SvnClientNotifier(SvnClientArgs^ clientArgs, System::IO::TextWriter^ tw)
		{
			if(!clientArgs)
				throw gcnew ArgumentNullException("clientArgs");
			else if(!tw)
				throw gcnew ArgumentNullException("tw");

			_args = clientArgs;
			_textWriter = tw;
			_args->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientNotifier::HandleNotify);
		}

		SvnClientNotifier(SvnClientArgs^ clientArgs, System::Text::StringBuilder^ sb)
		{
			if(!clientArgs)
				throw gcnew ArgumentNullException("clientArgs");
			else if(!sb)
				throw gcnew ArgumentNullException("sb");

			_args = clientArgs;
			_textWriter = gcnew System::IO::StringWriter(sb);
			_args->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientNotifier::HandleNotify);
		}

	public:
		~SvnClientNotifier()
		{
			if(_client)
			{
				_client->Notify -= gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientNotifier::HandleNotify);
				_client->BeforeCommand -= gcnew EventHandler<SvnBeforeCommandEventArgs^>(this, &SvnClientNotifier::HandleBeforeCommand);
			}
			if(_args)
				_args->Notify -= gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientNotifier::HandleNotify);				
		}

	protected:
		property SvnClientArgs^ CurrentCommandArgs
		{
			SvnClientArgs^ get()
			{
				if(_args)
					return _args;
				else
					return _client->CurrentCommandArgs;
			}
		}

		virtual void OnNotify(SvnNotifyEventArgs^ e);
		virtual void OnBeforeCommand(SvnBeforeCommandEventArgs^ e);

		/// <summary>Writes the specified message to the result writer</summary>
		virtual void Write(String^ message)
		{
			_textWriter->Write(message);
		}

		/// <summary>Writes the progress message to the result writer</summary>
		/// <remarks>The default implementation calls <see cref="Write(String^)" /> with the specified message</remarks>
		virtual void WriteProgress(String^ message)
		{
			Write(message);
		}

		/// <summary>Writes the specified message to the result writer and appends a <see cref="System::Environment::NewLine" /></summary>
		virtual void WriteLine(String^ message)
		{
			_textWriter->WriteLine(message);
		}

	private:
		void HandleNotify(Object^ sender, SvnNotifyEventArgs^ e)
		{
			UNUSED_ALWAYS(sender);

			if(!_afterInitial && !_client)
			{
				_afterInitial = true;
				OnBeforeCommand(gcnew SvnBeforeCommandEventArgs(_args));
			}

			OnNotify(e);
		}

		void HandleBeforeCommand(Object^ sender, SvnBeforeCommandEventArgs^ e)
		{
			UNUSED_ALWAYS(sender);
			OnBeforeCommand(e);
		}
	};

};