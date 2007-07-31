
#pragma once

#include "AprArray.h"
#include "AprBaton.h"

namespace SharpSvn {
	namespace Security {
		using SharpSvn::Apr::AprBaton;
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

		ref class SvnAuthentication;

		generic<typename TSvnAuthorizationArgs>
		where TSvnAuthorizationArgs : SvnAuthorizationArgs
		public delegate bool SvnAuthenticationHandler(Object^ sender, TSvnAuthorizationArgs e);

		interface class ISvnAuthWrapper
		{
			svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool);

			property int RetryLimit
			{
				int get();
				void set(int value);
			}

			property SvnAuthentication^ Authentication
			{
				SvnAuthentication^ get();
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
				UNUSED_ALWAYS(ptr);
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
			initonly SvnAuthenticationHandler<T>^ _handler;
			initonly SvnAuthentication^ _authentication;
			int _retryLimit;

		protected:
			SvnAuthWrapper(SvnAuthenticationHandler<T>^ handler, SvnAuthentication^ authentication)
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

		internal:
			bool Raise(T item)
			{
				return _handler(this, item);
			}

		public:
			property SvnAuthentication^ Authentication
			{
				virtual SvnAuthentication^ get()
				{
					return _authentication;
				}
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
				Wrapper(SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^ handler, SvnAuthentication^ authentication)
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
				Wrapper(SvnAuthenticationHandler<SvnUsernameArgs^>^ handler, SvnAuthentication^ authentication)
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

			/// <summary>FingerPrint name of the certificate</summary>
			property String^ FingerPrint
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

			/// <summary>Issuer value of the certificate</summary>
			property String^ Issuer
			{
				String^ get()
				{
					return _certIssuer;
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
				Wrapper(SvnAuthenticationHandler<SvnSslServerTrustArgs^>^ handler, SvnAuthentication^ authentication)
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
				Wrapper(SvnAuthenticationHandler<SvnSslClientCertificateArgs^>^ handler, SvnAuthentication^ authentication)
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
				Wrapper(SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>^ handler, SvnAuthentication^ authentication)
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
			SvnAuthentication();

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
			event SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^ UsernamePasswordHandlers
			{
				void add(SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^ e)
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

				void remove(SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^ e)
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

			event SvnAuthenticationHandler<SvnUsernameArgs^>^ UsernameHandlers
			{
				void add(SvnAuthenticationHandler<SvnUsernameArgs^>^ e)
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

				void remove(SvnAuthenticationHandler<SvnUsernameArgs^>^ e)
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

			event SvnAuthenticationHandler<SvnSslServerTrustArgs^>^ SslServerTrustHandlers
			{
				void add(SvnAuthenticationHandler<SvnSslServerTrustArgs^>^ e)
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

				void remove(SvnAuthenticationHandler<SvnSslServerTrustArgs^>^ e)
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

			event SvnAuthenticationHandler<SvnSslClientCertificateArgs^>^ SslClientCertificateHandlers
			{
				void add(SvnAuthenticationHandler<SvnSslClientCertificateArgs^>^ e)
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

				void remove(SvnAuthenticationHandler<SvnSslClientCertificateArgs^>^ e)
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

			event SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>^ SslClientCertificatePasswordHandlers
			{
				void add(SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>^ e)
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

				void remove(SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>^ e)
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
			static bool ImpSubversionFileUsernameHandler(Object ^sender, SvnUsernameArgs^ e);
			static bool ImpSubversionFileUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e);
			static bool ImpSubversionWindowsFileUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e);
			static bool ImpSubversionFileSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e);
			static bool ImpSubversionFileSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e);
			static bool ImpSubversionFileSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e);
			static bool ImpSubversionWindowsSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e);

		public:
			/// <summary>Subversion UsernameHandler file backend (managed representation)</summary>
			static initonly SvnAuthenticationHandler<SvnUsernameArgs^>^						SubversionFileUsernameHandler
				= gcnew SvnAuthenticationHandler<SvnUsernameArgs^>(ImpSubversionFileUsernameHandler);

			/// <summary>Subversion UsernamePasswordHandler file backend (managed representation)</summary>
			static initonly SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^				SubversionFileUsernamePasswordHandler
				= gcnew SvnAuthenticationHandler<SvnUsernamePasswordArgs^>(ImpSubversionFileUsernamePasswordHandler);

			/// <summary>Subversion UsernameHandler file backend using Windows CryptoStore (managed representation)</summary>
			/// <remarks>Should be added after <see cref="SubversionFileUsernamePasswordHandler" /></remarks>
			static initonly SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^				SubversionWindowsUsernamePasswordHandler
				= gcnew SvnAuthenticationHandler<SvnUsernamePasswordArgs^>(ImpSubversionWindowsFileUsernamePasswordHandler);

			/// <summary>Subversion SslServerTrust file backend (managed representation)</summary>
			static initonly SvnAuthenticationHandler<SvnSslServerTrustArgs^>^				SubversionFileSslServerTrustHandler
				= gcnew SvnAuthenticationHandler<SvnSslServerTrustArgs^>(ImpSubversionFileSslServerTrustHandler);

			/// <summary>Subversion SslClientCertificate file backend (managed representation)</summary>
			static initonly SvnAuthenticationHandler<SvnSslClientCertificateArgs^>^			SubversionFileSslClientCertificateHandler
				= gcnew SvnAuthenticationHandler<SvnSslClientCertificateArgs^>(ImpSubversionFileSslClientCertificateHandler);

			/// <summary>Subversion SslClientCertificatePassword file backend (managed representation)</summary>
			static initonly SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>^ SubversionFileSslClientCertificatePasswordHandler
				= gcnew SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>(ImpSubversionFileSslClientCertificatePasswordHandler);

			/// <summary>Subversion CryptoApi Ssl Trust handler</summary>
			static initonly SvnAuthenticationHandler<SvnSslServerTrustArgs^>^				SubversionWindowsSslServerTrustHandler
				= gcnew SvnAuthenticationHandler<SvnSslServerTrustArgs^>(ImpSubversionWindowsSslServerTrustHandler);

		private:
			static IntPtr GetParentHandle(Object ^sender);
			static bool ImpDialogUsernameHandler(Object ^sender, SvnUsernameArgs^ e); 
			static bool ImpDialogUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e);
			static bool ImpDialogSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e);
			static bool ImpDialogSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e);
			static bool ImpDialogSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e);

		public:
			/// <summary>Dialog based UsernameHandler implementation</summary>
			static initonly SvnAuthenticationHandler<SvnUsernameArgs^>^						DialogUsernameHandler
				= gcnew SvnAuthenticationHandler<SvnUsernameArgs^>(ImpDialogUsernameHandler);

			/// <summary>Dialog based UsernamePasswordHandler implementation</summary>
			static initonly SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^				DialogUsernamePasswordHandler
				= gcnew SvnAuthenticationHandler<SvnUsernamePasswordArgs^>(ImpDialogUsernamePasswordHandler);

			/// <summary>Dialog based SslServerTrust implementation</summary>
			static initonly SvnAuthenticationHandler<SvnSslServerTrustArgs^>^				DialogSslServerTrustHandler
				= gcnew SvnAuthenticationHandler<SvnSslServerTrustArgs^>(ImpDialogSslServerTrustHandler);

			/// <summary>Dialog based SslClientCertificate implementation</summary>
			static initonly SvnAuthenticationHandler<SvnSslClientCertificateArgs^>^			DialogSslClientCertificateHandler
				= gcnew SvnAuthenticationHandler<SvnSslClientCertificateArgs^>(ImpDialogSslClientCertificateHandler);

			/// <summary>Dialog based SslClientCertificatePassword implementation</summary>
			static initonly SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>^ DialogSslClientCertificatePasswordHandler
				= gcnew SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>(ImpDialogSslClientCertificatePasswordHandler);

		private:
			static void MaybePrintRealm(SvnAuthorizationArgs^ e);
			static String^ ReadPassword();
			static bool ImpConsoleUsernameHandler(Object ^sender, SvnUsernameArgs^ e);
			static bool ImpConsoleUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordArgs^ e);
			static bool ImpConsoleSslServerTrustHandler(Object ^sender, SvnSslServerTrustArgs^ e);
			static bool ImpConsoleSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateArgs^ e);
			static bool ImpConsoleSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordArgs^ e);

		public:
			/// <summary>Console based UsernameHandler implementation</summary>
			static initonly SvnAuthenticationHandler<SvnUsernameArgs^>^						ConsoleUsernameHandler
				= gcnew SvnAuthenticationHandler<SvnUsernameArgs^>(ImpConsoleUsernameHandler);

			/// <summary>Console based UsernamePasswordHandler implementation</summary>
			static initonly SvnAuthenticationHandler<SvnUsernamePasswordArgs^>^				ConsoleUsernamePasswordHandler
				= gcnew SvnAuthenticationHandler<SvnUsernamePasswordArgs^>(ImpConsoleUsernamePasswordHandler);

			/// <summary>Console based SslServerTrust implementation</summary>
			static initonly SvnAuthenticationHandler<SvnSslServerTrustArgs^>^				ConsoleSslServerTrustHandler
				= gcnew SvnAuthenticationHandler<SvnSslServerTrustArgs^>(ImpConsoleSslServerTrustHandler);

			/// <summary>Console based SslClientCertificate implementation</summary>
			static initonly SvnAuthenticationHandler<SvnSslClientCertificateArgs^>^			ConsoleSslClientCertificateHandler
				= gcnew SvnAuthenticationHandler<SvnSslClientCertificateArgs^>(ImpConsoleSslClientCertificateHandler);

			/// <summary>Console based SslClientCertificatePassword implementation</summary>
			static initonly SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>^ ConsoleSslClientCertificatePasswordHandler
				= gcnew SvnAuthenticationHandler<SvnSslClientCertificatePasswordArgs^>(ImpConsoleSslClientCertificatePasswordHandler);
		};
	}
}
