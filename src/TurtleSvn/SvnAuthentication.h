
#pragma once

#include "AprBaton.h"

namespace TurtleSvn {
	namespace Security {
		using TurtleSvn::Apr::AprBaton;
		using System::Collections::Generic::Dictionary;
		using System::Collections::Generic::List;

		public ref class SvnAuthorizationArgs abstract: public EventArgs
		{
			initonly bool _maySave;
			initonly String^ _realm;
			bool _save;

		protected:
			SvnAuthorizationArgs(String^ realm, bool maySave)
			{
				_realm = realm;
				_maySave = maySave;
			}

		public:
			/// <summary>If may_save is false, the auth system does not allow the credentials 
			/// to be saved (to disk). A prompt function shall not ask the user if the 
			/// credentials shall be saved if may_save is FALSE. For example, a GUI client 
			/// with a remember password checkbox would grey out the checkbox if may_save 
			/// is false</summary>
			property bool MaySave
			{
				bool get()
				{ 
					return _maySave; 
				}	
			}

			/// <summary>If realm is non-null, maybe use it in the prompt string</summary>
			property String^ Realm
			{
				String^ get()
				{
					return _realm;
				}
			}

			property bool Save
			{
				bool get()
				{ 
					return _save; 
				}	

				void set(bool value)
				{
					_save = value && MaySave;
				}
			}
		};

		interface class ISvnAuthWrapper
		{
		};

		generic<typename T>
		where T : SvnAuthorizationArgs
		ref class SvnAuthWrapper : public ISvnAuthWrapper
		{
			initonly AprBaton<SvnAuthWrapper<T>^>^ _baton;
			initonly EventHandler<T>^ _handler;

		public:
			SvnAuthWrapper(EventHandler<T>^ handler)
			{
				if(!handler)
					throw gcnew ArgumentNullException("handler");

				_handler = handler;
				_baton = gcnew AprBaton<SvnAuthWrapper<T>^>(this);
			}
		};

		public ref class SvnUsernamePasswordArgs : public SvnAuthorizationArgs
		{
			String ^_initialUsername;
			String ^_username;
			String ^_password;
		public:
			SvnUsernamePasswordArgs(String^ initialUsername, String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
				_initialUsername = initialUsername;
				_username = initialUsername ? initialUsername : "";
				_password = "";
			}

			/// <summary>Default username; can be NULL</summary>
			property String^ InitialUsername
			{
				String^ get()
				{
					return _initialUsername;
				}
			}

			/// <summary>The username to authorize with</summary>
			property String^ Username
			{
				String^ get()
				{
					return _username;
				}
				void set(String ^value)
				{
					_username = value ? value : "";
				}
			}

			/// <summary>The password to authorize with</summary>
			property String^ Password
			{
				String^ get()
				{
					return _password;
				}
				void set(String ^value)
				{
					_password = value ? value : "";
				}
			}

		internal:
			ref class Wrapper : public SvnAuthWrapper<SvnUsernamePasswordArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUsernamePasswordArgs^>^ handler)
					: SvnAuthWrapper<SvnUsernamePasswordArgs^>(handler)
				{
				}


			};
		};

		public ref class SvnUsernameArgs : public SvnAuthorizationArgs
		{
			String ^_username;
		public:
			SvnUsernameArgs(String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
				_username = "";
			}

			/// <summary>The username to authorize with</summary>
			property String^ Username
			{
				String^ get()
				{
					return _username;
				}
				void set(String ^value)
				{
					_username = value ? value : "";
				}
			}

		internal:
			ref class Wrapper : public SvnAuthWrapper<SvnUsernameArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUsernameArgs^>^ handler)
					: SvnAuthWrapper<SvnUsernameArgs^>(handler)
				{
				}
			};
		};

		public ref class SvnSslServerTrustArgs : public SvnAuthorizationArgs
		{
		internal:
			SvnSslServerTrustArgs (String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
			}

		internal:
			ref class Wrapper : public SvnAuthWrapper<SvnSslServerTrustArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslServerTrustArgs^>^ handler)
					: SvnAuthWrapper<SvnSslServerTrustArgs^>(handler)
				{
				}


			};
		};

		public ref class SvnSslClientCertificateArgs : public SvnAuthorizationArgs
		{
		internal:
			SvnSslClientCertificateArgs(String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
			}

		internal:
			ref class Wrapper : public SvnAuthWrapper<SvnSslClientCertificateArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslClientCertificateArgs^>^ handler)
					: SvnAuthWrapper<SvnSslClientCertificateArgs^>(handler)
				{
				}
			};
		};

		public ref class SvnSslClientCertificatePasswordArgs : public SvnAuthorizationArgs
		{
		internal:
			SvnSslClientCertificatePasswordArgs(String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
			}

		internal:
			ref class Wrapper : public SvnAuthWrapper<SvnSslClientCertificatePasswordArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslClientCertificatePasswordArgs^>^ handler)
					: SvnAuthWrapper<SvnSslClientCertificatePasswordArgs^>(handler)
				{
				}
			};
		};
	

		/// <summary>Container for all subversion authentication settings on a client</summary>
		public ref class SvnAuthentication : public SvnBase
		{
		internal:			
			Dictionary<Delegate^, ISvnAuthWrapper^>^ _wrappers;
			List<ISvnAuthWrapper^>^ _handlers;
			bool _readOnly;
			int _cookie;
		
			SvnAuthentication()
			{
				_wrappers = gcnew Dictionary<Delegate^, ISvnAuthWrapper^>();
				_handlers = gcnew List<ISvnAuthWrapper^>();

				AddSubversionFileHandlers(); // Add handlers which use no interaction by default
			}

		public:
			event EventHandler<SvnUsernamePasswordArgs^>^ UsernamePasswordHandlers
			{
				void add(EventHandler<SvnUsernamePasswordArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnUsernamePasswordArgs::Wrapper(e);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnUsernamePasswordArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ wrapper;

					if(_wrappers->TryGetValue(e, wrapper))
					{
						_cookie++;
						_wrappers->Remove(e);
						_handlers->Remove(wrapper);
					}
				}
			}

			event EventHandler<SvnUsernameArgs^>^ UsernameHandlers
			{
				void add(EventHandler<SvnUsernameArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnUsernameArgs::Wrapper(e);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnUsernameArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ wrapper;

					if(_wrappers->TryGetValue(e, wrapper))
					{
						_cookie++;
						_wrappers->Remove(e);
						_handlers->Remove(wrapper);
					}
				}
			}

			event EventHandler<SvnSslServerTrustArgs^>^ SslServerTrustHandlers
			{
				void add(EventHandler<SvnSslServerTrustArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnSslServerTrustArgs::Wrapper(e);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslServerTrustArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ wrapper;

					if(_wrappers->TryGetValue(e, wrapper))
					{
						_cookie++;
						_wrappers->Remove(e);
						_handlers->Remove(wrapper);
					}
				}
			}

			event EventHandler<SvnSslClientCertificateArgs^>^ SslClientCertificateHandlers
			{
				void add(EventHandler<SvnSslClientCertificateArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificateArgs::Wrapper(e);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslClientCertificateArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ wrapper;

					if(_wrappers->TryGetValue(e, wrapper))
					{
						_cookie++;
						_wrappers->Remove(e);
						_handlers->Remove(wrapper);
					}
				}
			}

			event EventHandler<SvnSslClientCertificatePasswordArgs^>^ SslClientCertificatePasswordHandlers
			{
				void add(EventHandler<SvnSslClientCertificatePasswordArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificatePasswordArgs::Wrapper(e);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslClientCertificatePasswordArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ wrapper;

					if(_wrappers->TryGetValue(e, wrapper))
					{
						_cookie++;
						_wrappers->Remove(e);
						_handlers->Remove(wrapper);
					}
				}
			}

		public:
			/// <summary>Removes all currently registered providers</summary>
			void Clear();

			/// <summary>Adds all default subversion-configuration-based handlers</summary>
			void AddSubversionFileHandlers();

			/// <summary>Adds all default dialog handlers</summary>
			void AddDialogHandlers();

			/// <summary>Adds all default console handlers</summary>
			void AddConsoleHandlers();


		private:
			static void ImpSubversionFileUsernameHandler(Object ^sender, SvnUsernameArgs^ e) 
			{ throw gcnew NotImplementedException("Managed placeholder for unmanaged function"); }

			static void ImpSubversionFileUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e) 
			{ throw gcnew NotImplementedException("Managed placeholder for unmanaged function"); }

			static void ImpSubversionSvnSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e) 
			{ throw gcnew NotImplementedException("Managed placeholder for unmanaged function"); }

			static void ImpSubversionFileSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e)
			{ throw gcnew NotImplementedException("Managed placeholder for unmanaged function"); }

			static void ImpSubversionFileSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e)
			{ throw gcnew NotImplementedException("Managed placeholder for unmanaged function"); }

		public:
			/// <summary>Subversion UsernameHandler file backend (managed representation)</summary>
			static initonly EventHandler<SvnUsernameArgs^>^						SubversionFileUsernameHandler
				= gcnew EventHandler<SvnUsernameArgs^>(ImpSubversionFileUsernameHandler);

			/// <summary>Subversion UsernamePasswordHandler file backend (managed representation)</summary>
			static initonly EventHandler<SvnUsernamePasswordArgs^>^				SubversionFileUsernamePasswordHandler
				= gcnew EventHandler<SvnUsernamePasswordArgs^>(ImpSubversionFileUsernamePasswordHandler);

			/// <summary>Subversion SslServerTrust file backend (managed representation)</summary>
			static initonly EventHandler<SvnSslServerTrustArgs^>^				SubversionFileSslServerTrustHandler
				= gcnew EventHandler<SvnSslServerTrustArgs^>(ImpSubversionSvnSslServerTrustHandler);

			/// <summary>Subversion SslClientCertificate file backend (managed representation)</summary>
			static initonly EventHandler<SvnSslClientCertificateArgs^>^			SubversionFileSslClientCertificateHandler
				= gcnew EventHandler<SvnSslClientCertificateArgs^>(ImpSubversionFileSslClientCertificateHandler);

			/// <summary>Subversion SslClientCertificatePassword file backend (managed representation)</summary>
			static initonly EventHandler<SvnSslClientCertificatePasswordArgs^>^ SubversionFileSslClientCertificatePasswordHandler
				= gcnew EventHandler<SvnSslClientCertificatePasswordArgs^>(ImpSubversionFileSslClientCertificatePasswordHandler);

		private:
			static void ImpDialogUsernameHandler(Object ^sender, SvnUsernameArgs^ e); 
			static void ImpDialogUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e);
			static void ImpDialogSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e);
			static void ImpDialogSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e);
			static void ImpDialogSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e);

		public:
			/// <summary>Dialog based UsernameHandler implementation</summary>
			static initonly EventHandler<SvnUsernameArgs^>^						DialogUsernameHandler
				= gcnew EventHandler<SvnUsernameArgs^>(ImpDialogUsernameHandler);

			/// <summary>Dialog based UsernamePasswordHandler implementation</summary>
			static initonly EventHandler<SvnUsernamePasswordArgs^>^				DialogUsernamePasswordHandler
				= gcnew EventHandler<SvnUsernamePasswordArgs^>(ImpDialogUsernamePasswordHandler);

			/// <summary>Dialog based SslServerTrust implementation</summary>
			static initonly EventHandler<SvnSslServerTrustArgs^>^				DialogSslServerTrustHandler
				= gcnew EventHandler<SvnSslServerTrustArgs^>(ImpDialogSslServerTrustHandler);

			/// <summary>Dialog based SslClientCertificate implementation</summary>
			static initonly EventHandler<SvnSslClientCertificateArgs^>^			DialogSslClientCertificateHandler
				= gcnew EventHandler<SvnSslClientCertificateArgs^>(ImpDialogSslClientCertificateHandler);

			/// <summary>Dialog based SslClientCertificatePassword implementation</summary>
			static initonly EventHandler<SvnSslClientCertificatePasswordArgs^>^ DialogSslClientCertificatePasswordHandler
				= gcnew EventHandler<SvnSslClientCertificatePasswordArgs^>(ImpDialogSslClientCertificatePasswordHandler);

		private:
			static void ImpConsoleUsernameHandler(Object ^sender, SvnUsernameArgs^ e);
			static void ImpConsoleUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e);
			static void ImpConsoleSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e);
			static void ImpConsoleSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e);
			static void ImpConsoleSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e);

		public:
			/// <summary>Console based UsernameHandler implementation</summary>
			static initonly EventHandler<SvnUsernameArgs^>^						ConsoleUsernameHandler
				= gcnew EventHandler<SvnUsernameArgs^>(ImpConsoleUsernameHandler);

			/// <summary>Console based UsernamePasswordHandler implementation</summary>
			static initonly EventHandler<SvnUsernamePasswordArgs^>^				ConsoleUsernamePasswordHandler
				= gcnew EventHandler<SvnUsernamePasswordArgs^>(ImpConsoleUsernamePasswordHandler);

			/// <summary>Console based SslServerTrust implementation</summary>
			static initonly EventHandler<SvnSslServerTrustArgs^>^				ConsoleSslServerTrustHandler
				= gcnew EventHandler<SvnSslServerTrustArgs^>(ImpConsoleSslServerTrustHandler);

			/// <summary>Console based SslClientCertificate implementation</summary>
			static initonly EventHandler<SvnSslClientCertificateArgs^>^			ConsoleSslClientCertificateHandler
				= gcnew EventHandler<SvnSslClientCertificateArgs^>(ImpConsoleSslClientCertificateHandler);

			/// <summary>Console based SslClientCertificatePassword implementation</summary>
			static initonly EventHandler<SvnSslClientCertificatePasswordArgs^>^ ConsoleSslClientCertificatePasswordHandler
				= gcnew EventHandler<SvnSslClientCertificatePasswordArgs^>(ImpConsoleSslClientCertificatePasswordHandler);
		};
	}
}
