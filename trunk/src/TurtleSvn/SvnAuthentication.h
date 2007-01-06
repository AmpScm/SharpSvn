
#pragma once

#include "AprArray.h"
#include "AprBaton.h"

namespace TurtleSvn {
	namespace Security {
		using TurtleSvn::Apr::AprBaton;
		using System::Collections::Generic::Dictionary;
		using System::Collections::Generic::List;

		public interface class IParentWindowHandleProvider
		{
			property IntPtr ParentWindowHandle
			{
				IntPtr get();
			}
		};


		ref class SvnAuthentication;

		public ref class SvnAuthorizationArgs abstract: public EventArgs
		{
			initonly bool _maySave;
			initonly String^ _realm;
			bool _save;
			bool _cancel;

		protected:
			SvnAuthorizationArgs(String^ realm, bool maySave)
			{
				_realm = realm;
				_maySave = maySave;
			}

		public:
			/// <summary>If MaySave is false, the auth system does not allow the credentials 
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

			property bool Cancel
			{
				bool get()
				{
					return _cancel;
				}
				void set(bool value)
				{
					_cancel = value;
				}
			}
		};

		interface class ISvnAuthWrapper
		{
			svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool);

			property int RetryLimit
			{
				int get();
				void set(int value);
			}
		};

		ref class SvnAuthProviderMarshaller sealed : public SvnBase, public IItemMarshaller<ISvnAuthWrapper^>
		{
		public:
			SvnAuthProviderMarshaller()
			{}

			property int ItemSize
			{
				virtual int get()
				{
					return sizeof(svn_auth_provider_object_t*);
				}
			}

			virtual void Write(ISvnAuthWrapper^ value, void* ptr, AprPool^ pool)
			{
				svn_auth_provider_object_t **ppProvider = (svn_auth_provider_object_t **)ptr;

				*ppProvider = value->GetProviderPtr(pool);
			}

			virtual ISvnAuthWrapper^ Read(const void* ptr)
			{
				// Not needed; we only provide arrays; item is black box to us
				throw gcnew NotImplementedException();
			}
		};

		generic<typename T>
		where T : SvnAuthorizationArgs
		ref class SvnAuthWrapper abstract : public ISvnAuthWrapper
		{
		protected:
			initonly AprBaton<SvnAuthWrapper<T>^>^ _baton;
			initonly EventHandler<T>^ _handler;
			initonly SvnAuthentication^ _authentication;
			int _retryLimit;

		protected:
			SvnAuthWrapper(EventHandler<T>^ handler, SvnAuthentication^ authentication)
			{
				if(!handler)
					throw gcnew ArgumentNullException("handler");
				else if(!authentication)
					throw gcnew ArgumentNullException("authentication");

				_handler = handler;
				_baton = gcnew AprBaton<SvnAuthWrapper<T>^>(this);
				_authentication = authentication;
				_retryLimit = 128;
			}

		public:
			virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) abstract;

			property virtual int RetryLimit
			{
				int get()
				{
					return _retryLimit;
				}
				void set(int value)
				{
					_retryLimit = value;
				}
			}

			void Raise(T item)
			{
				_handler(this, item);
			}
		};

		public ref class SvnUsernamePasswordArgs : public SvnAuthorizationArgs
		{
			initonly String ^_initialUsername;
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnUsernamePasswordArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUsernamePasswordArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnUsernamePasswordArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnUsernameArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUsernameArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnUsernameArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnSslServerTrustArgs : public SvnAuthorizationArgs
		{
			initonly SvnCertificateTrustFailure _failures;
			initonly String^ _certCommonName;
			initonly String^ _certFingerprint;
			initonly String^ _certValidFrom;
			initonly String^ _certValidUntil;
			initonly String^ _certIssuer;
			initonly String^ _certValue;
			SvnCertificateTrustFailure _acceptedFailures;

		public:
			SvnSslServerTrustArgs (SvnCertificateTrustFailure failures, String^ certificateCommonName, String^ certificateFingerprint, String^ certificateValidFrom, 
				String^ certificateValidUntil, String^ certificateIssuer, String^ certificateValue, String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
				if(!certificateCommonName)
					throw gcnew NullReferenceException("certificateCommonName");
				else if(!certificateFingerprint)
					throw gcnew NullReferenceException("certificateFingerprint");
				else if(!certificateValidFrom)
					throw gcnew NullReferenceException("certificateValidFrom");
				else if(!certificateValidUntil)
					throw gcnew NullReferenceException("certificateValidUntil");
				else if(!certificateIssuer)
					throw gcnew NullReferenceException("certificateIssuer");
				else if(!certificateValue)
					throw gcnew NullReferenceException("certificateValue");

				_failures			= failures;
				_certCommonName		= certificateCommonName;
				_certFingerprint	= certificateFingerprint;
				_certValidFrom		= certificateValidFrom;
				_certValidUntil		= certificateValidUntil;
				_certIssuer			= certificateIssuer;
				_certValue			= certificateValue;
			}

		public:
			property SvnCertificateTrustFailure Failures
			{
				SvnCertificateTrustFailure get()
				{
					return _failures;
				}
			}

			property SvnCertificateTrustFailure AcceptedFailures
			{
				SvnCertificateTrustFailure get()
				{
					return _acceptedFailures;
				}

				void set(SvnCertificateTrustFailure value)
				{
					_acceptedFailures = (SvnCertificateTrustFailure)(value & SvnCertificateTrustFailure::MaskAllFailures);
				}
			}

			/// <summary>Common name of the certificate</summary>
			property String^ CommonName
			{
				String^ get()
				{
					return _certCommonName;
				}
			}

			/// <summary>Fingerprint name of the certificate</summary>
			property String^ Fingerprint
			{
				String^ get()
				{
					return _certFingerprint;
				}
			}

			/// <summary>Text valid-from value of the certificate</summary>
			property String^ ValidFrom
			{
				String^ get()
				{
					return _certValidFrom;
				}
			}

			/// <summary>Text valid-until value of the certificate</summary>
			property String^ ValidUntil
			{
				String^ get()
				{
					return _certValidUntil;
				}
			}

			/// <summary>Text version of the certificate</summary>
			property String^ CertificateValue
			{
				String^ get()
				{
					return _certValue;
				}
			}

		internal:
			ref class Wrapper sealed : public SvnAuthWrapper<SvnSslServerTrustArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslServerTrustArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnSslServerTrustArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnSslClientCertificateArgs : public SvnAuthorizationArgs
		{
			String^ _certificateFile;
		public:
			SvnSslClientCertificateArgs(String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
			}

			property String^ CertificateFile
			{
				String^ get()
				{
					return _certificateFile ? _certificateFile : "";
				}

				void set(String^ value)
				{
					_certificateFile = value;
				}
			}

		internal:
			ref class Wrapper sealed : public SvnAuthWrapper<SvnSslClientCertificateArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslClientCertificateArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnSslClientCertificateArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnSslClientCertificatePasswordArgs : public SvnAuthorizationArgs
		{
			String^ _password;
		public:
			SvnSslClientCertificatePasswordArgs(String^ realm, bool maySave)
				: SvnAuthorizationArgs(realm, maySave)
			{
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnSslClientCertificatePasswordArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslClientCertificatePasswordArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnSslClientCertificatePasswordArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};


		/// <summary>Container for all subversion authentication settings on a client</summary>
		public ref class SvnAuthentication : public SvnBase
		{
			Dictionary<Delegate^, ISvnAuthWrapper^>^ _wrappers;
			List<ISvnAuthWrapper^>^ _handlers;
			IParentWindowHandleProvider^ _parentWindowHandleProvider;
			bool _readOnly;
			int _cookie;

		internal:
			SvnAuthentication()
			{
				_wrappers = gcnew Dictionary<Delegate^, ISvnAuthWrapper^>();
				_handlers = gcnew List<ISvnAuthWrapper^>();

				AddSubversionFileHandlers(); // Add handlers which use no interaction by default
			}

			property int Cookie
			{
				int get()
				{
					return _cookie;
				}
			}

			/// <summary>Retrieves an authorization baton allocated in the specified pool; containing the current authorization settings</summary>
			svn_auth_baton_t *GetAuthorizationBaton(AprPool ^pool, [Out] int% cookie);

		public:
			/// <summary>Allows users to provide a parent window handle to use as owner of popup windows</summary>
			property IParentWindowHandleProvider^ WindowHandleProvider
			{
				IParentWindowHandleProvider^ get()
				{
					return _parentWindowHandleProvider;
				}

				void set(IParentWindowHandleProvider^ value)
				{
					_parentWindowHandleProvider = value;
				}
			}

			void SetRetryLimit(Delegate^ handler, int limit)
			{
				if(!handler)
					throw gcnew ArgumentNullException("handler");
				else if(limit < 0)
					throw gcnew ArgumentOutOfRangeException("limit out of range");

				ISvnAuthWrapper^ wrapper;

				if(_wrappers->TryGetValue(handler, wrapper))
				{
					wrapper->RetryLimit = limit;
					_cookie++;
				}
				else
					throw gcnew ArgumentException("Handler not registered", "handler");
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

					ISvnAuthWrapper^ handler = gcnew SvnUsernamePasswordArgs::Wrapper(e, this);

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

					ISvnAuthWrapper^ handler = gcnew SvnUsernameArgs::Wrapper(e, this);

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

					ISvnAuthWrapper^ handler = gcnew SvnSslServerTrustArgs::Wrapper(e, this);

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

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificateArgs::Wrapper(e, this);

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

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificatePasswordArgs::Wrapper(e, this);

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
