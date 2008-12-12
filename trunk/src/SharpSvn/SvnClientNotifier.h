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

#pragma once

#include "SvnClient.h"

namespace SharpSvn {
	using namespace System::IO;

	/// <summary>Subversion Client Context wrapper; base class of objects using client context</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnClientReporter : public System::MarshalByRefObject, public IDisposable
	{
		initonly SvnClient^ _client;
		initonly SvnClientArgs^ _args;
		initonly TextWriter^ _textWriter;
		initonly IFormatProvider^ _fp;
		bool _afterInitial;

	private:
		SvnCommandType _commandType;
		bool _receivedSomeChange;
		bool _suppressFinalLine;
		bool _sentFirstTxdelta;
		int _inExternal;

	public:
		SvnClientReporter(SvnClient^ client, System::IO::TextWriter^ to)
		{
			if (!client)
				throw gcnew ArgumentNullException("client");
			else if (!to)
				throw gcnew ArgumentNullException("to");

			_client = client;
			_textWriter = to;
			_fp = to->FormatProvider;
			_client->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);
			_client->Processing += gcnew EventHandler<SvnProcessingEventArgs^>(this, &SvnClientReporter::HandleProcessing);
		}

		SvnClientReporter(SvnClient^ client, System::Text::StringBuilder^ sb)
		{
			if (!client)
				throw gcnew ArgumentNullException("client");
			else if (!sb)
				throw gcnew ArgumentNullException("sb");

			_client = client;
			_textWriter = gcnew System::IO::StringWriter(sb, (IFormatProvider^)nullptr);
			_client->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);
			_client->Processing += gcnew EventHandler<SvnProcessingEventArgs^>(this, &SvnClientReporter::HandleProcessing);
		}

		SvnClientReporter(SvnClient^ client, System::Text::StringBuilder^ sb, IFormatProvider^ formatProvider)
		{
			if (!client)
				throw gcnew ArgumentNullException("client");
			else if (!sb)
				throw gcnew ArgumentNullException("sb");

			_client = client;
			_fp = formatProvider;
			_textWriter = gcnew System::IO::StringWriter(sb, formatProvider);
			_client->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);
			_client->Processing += gcnew EventHandler<SvnProcessingEventArgs^>(this, &SvnClientReporter::HandleProcessing);
		}

		SvnClientReporter(SvnClientArgs^ clientArgs, System::IO::TextWriter^ to)
		{
			if (!clientArgs)
				throw gcnew ArgumentNullException("clientArgs");
			else if (!to)
				throw gcnew ArgumentNullException("to");

			_args = clientArgs;
			_textWriter = to;
			_fp = to->FormatProvider;
			_args->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);
		}

		SvnClientReporter(SvnClientArgs^ clientArgs, System::Text::StringBuilder^ sb)
		{
			if (!clientArgs)
				throw gcnew ArgumentNullException("clientArgs");
			else if (!sb)
				throw gcnew ArgumentNullException("sb");

			_args = clientArgs;
			_textWriter = gcnew System::IO::StringWriter(sb, (IFormatProvider^)nullptr);
			_args->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);
		}

		SvnClientReporter(SvnClientArgs^ clientArgs, System::Text::StringBuilder^ sb, IFormatProvider^ formatProvider)
		{
			if (!clientArgs)
				throw gcnew ArgumentNullException("clientArgs");
			else if (!sb)
				throw gcnew ArgumentNullException("sb");

			_args = clientArgs;
			_fp = formatProvider;
			_textWriter = gcnew System::IO::StringWriter(sb, formatProvider);
			_args->Notify += gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);
		}

	public:
		~SvnClientReporter()
		{
			if (_client)
			{
				_client->Notify -= gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);
				_client->Processing -= gcnew EventHandler<SvnProcessingEventArgs^>(this, &SvnClientReporter::HandleProcessing);
			}
			if (_args)
				_args->Notify -= gcnew EventHandler<SvnNotifyEventArgs^>(this, &SvnClientReporter::HandleNotify);

			_textWriter->Close();
		}

	public:
		property bool SuppressFinalLine
		{
			bool get()
			{
				return _suppressFinalLine;
			}
			void set(bool value)
			{
				_suppressFinalLine = value;
			}
		}

	protected:
		property SvnCommandType CurrentCommandType
		{
			SvnCommandType get()
			{
				return _commandType;
			}
		}

		virtual void OnNotify(SvnNotifyEventArgs^ e);
		virtual void OnProcessing(SvnProcessingEventArgs^ e);

		/// <summary>Writes the specified message to the result writer</summary>
		virtual void Write(String^ value)
		{
			_textWriter->Write(value);
		}

		/// <summary>Writes the progress message to the result writer</summary>
		/// <remarks>The default implementation calls <see cref="Write(String^)" /> with the specified message</remarks>
		virtual void WriteProgress(String^ value)
		{
			Write(value);
		}

		/// <summary>Writes the specified message to the result writer and appends a <see cref="System::Environment::NewLine" /></summary>
		virtual void WriteLine(String^ value)
		{
			_textWriter->WriteLine(value);
		}

	private:
		void HandleNotify(Object^ sender, SvnNotifyEventArgs^ e)
		{
			UNUSED_ALWAYS(sender);

			if (!_afterInitial && !_client)
			{
				_afterInitial = true;
				OnProcessing(gcnew SvnProcessingEventArgs(_args->CommandType));
			}

			OnNotify(e);
		}

		void HandleProcessing(Object^ sender, SvnProcessingEventArgs^ e)
		{
			UNUSED_ALWAYS(sender);
			OnProcessing(e);
		}
	};

};