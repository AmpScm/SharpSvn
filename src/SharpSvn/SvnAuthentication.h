
#pragma once

#include "AprArray.h"
#include "AprBaton.h"

namespace SharpSvn {
	namespace Security {
		using SharpSvn::Apr::AprBaton;
		using System::Collections::Generic::Dictionary;
		using System::Collections::Generic::List;

		ref class SvnAuthentication;

		public ref class SvnAuthorizationEventArgs abstract: public EventArgs
		{
			initonly bool _maySave;
			initonly String^ _realm;
			bool _save;
			bool _cancel;
			bool _break;

		protected:
			SvnAuthorizationEventArgs(String^ realm, bool maySave)
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

			property bool Break
			{
				bool get()
				{
					return _break;
				}
				void set(bool value)
				{
					_break = value;
				}
			}
		};

		ref class SvnAuthentication;

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

			virtual ISvnAuthWrapper^ Read(const void* ptr, AprPool^ pool)
			{
				UNUSED_ALWAYS(ptr);
				UNUSED_ALWAYS(pool);
				// Not needed; we only provide arrays; item is black box to us
				throw gcnew NotImplementedException();
			}
		};

		generic<typename T>
		where T : SvnAuthorizationEventArgs
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

		internal:
			void Raise(T item)
			{
				_handler(_authentication, item);
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

		public ref class SvnUsernamePasswordEventArgs : public SvnAuthorizationEventArgs
		{
			initonly String ^_initialUsername;
			String ^_username;
			String ^_password;
		public:
			SvnUsernamePasswordEventArgs(String^ initialUsername, String^ realm, bool maySave)
				: SvnAuthorizationEventArgs(realm, maySave)
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnUsernamePasswordEventArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUsernamePasswordEventArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnUsernamePasswordEventArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnUsernameEventArgs : public SvnAuthorizationEventArgs
		{
			String ^_username;
		public:
			SvnUsernameEventArgs(String^ realm, bool maySave)
				: SvnAuthorizationEventArgs(realm, maySave)
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnUsernameEventArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUsernameEventArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnUsernameEventArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnSslServerTrustEventArgs : public SvnAuthorizationEventArgs
		{
			initonly SvnCertificateTrustFailures _failures;
			initonly String^ _certCommonName;
			initonly String^ _certFingerprint;
			initonly String^ _certValidFrom;
			initonly String^ _certValidUntil;
			initonly String^ _certIssuer;
			initonly String^ _certValue;
			SvnCertificateTrustFailures _acceptedFailures;

		public:
			SvnSslServerTrustEventArgs (SvnCertificateTrustFailures failures, String^ certificateCommonName, String^ certificateFingerprint, String^ certificateValidFrom,
				String^ certificateValidUntil, String^ certificateIssuer, String^ certificateValue, String^ realm, bool maySave)
				: SvnAuthorizationEventArgs(realm, maySave)
			{
				if(!certificateCommonName)
					throw gcnew ArgumentNullException("certificateCommonName");
				else if(!certificateFingerprint)
					throw gcnew ArgumentNullException("certificateFingerprint");
				else if(!certificateValidFrom)
					throw gcnew ArgumentNullException("certificateValidFrom");
				else if(!certificateValidUntil)
					throw gcnew ArgumentNullException("certificateValidUntil");
				else if(!certificateIssuer)
					throw gcnew ArgumentNullException("certificateIssuer");
				else if(!certificateValue)
					throw gcnew ArgumentNullException("certificateValue");

				_failures			= failures;
				_certCommonName		= certificateCommonName;
				_certFingerprint	= certificateFingerprint;
				_certValidFrom		= certificateValidFrom;
				_certValidUntil		= certificateValidUntil;
				_certIssuer			= certificateIssuer;
				_certValue			= certificateValue;
			}

		public:
			property SvnCertificateTrustFailures Failures
			{
				SvnCertificateTrustFailures get()
				{
					return _failures;
				}
			}

			property SvnCertificateTrustFailures AcceptedFailures
			{
				SvnCertificateTrustFailures get()
				{
					return _acceptedFailures;
				}

				void set(SvnCertificateTrustFailures value)
				{
					_acceptedFailures = (SvnCertificateTrustFailures)(value & SvnCertificateTrustFailures::MaskAllFailures);
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnSslServerTrustEventArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslServerTrustEventArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnSslServerTrustEventArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnSslClientCertificateEventArgs : public SvnAuthorizationEventArgs
		{
			String^ _certificateFile;
		public:
			SvnSslClientCertificateEventArgs(String^ realm, bool maySave)
				: SvnAuthorizationEventArgs(realm, maySave)
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnSslClientCertificateEventArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslClientCertificateEventArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnSslClientCertificateEventArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnSslClientCertificatePasswordEventArgs : public SvnAuthorizationEventArgs
		{
			String^ _password;
		public:
			SvnSslClientCertificatePasswordEventArgs(String^ realm, bool maySave)
				: SvnAuthorizationEventArgs(realm, maySave)
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnSslClientCertificatePasswordEventArgs^>
			{
			public:
				Wrapper(EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnSslClientCertificatePasswordEventArgs^>(handler, authentication)
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
			Object^ _parentWindow;
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
			property Object^ ParentWindow
			{
				Object^ get()
				{
					return _parentWindow;
				}

				void set(Object^ value)
				{
					if(!value || SharpSvn::UI::Authentication::SharpSvnGui::HasWin32Handle(value))
						_parentWindow = value;
					else
						throw gcnew ArgumentException(SharpSvnStrings::ParentWindowHasNoWindowHandle, "value");
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
					throw gcnew ArgumentException("HandlerIsNotRegisteredAtThisTime", "handler");
			}

		public:
			event EventHandler<SvnUsernamePasswordEventArgs^>^ UsernamePasswordHandlers
			{
				void add(EventHandler<SvnUsernamePasswordEventArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnUsernamePasswordEventArgs::Wrapper(e, this);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnUsernamePasswordEventArgs^>^ e)
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

			event EventHandler<SvnUsernameEventArgs^>^ UsernameHandlers
			{
				void add(EventHandler<SvnUsernameEventArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnUsernameEventArgs::Wrapper(e, this);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnUsernameEventArgs^>^ e)
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

			event EventHandler<SvnSslServerTrustEventArgs^>^ SslServerTrustHandlers
			{
				void add(EventHandler<SvnSslServerTrustEventArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnSslServerTrustEventArgs::Wrapper(e, this);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslServerTrustEventArgs^>^ e)
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

			event EventHandler<SvnSslClientCertificateEventArgs^>^ SslClientCertificateHandlers
			{
				void add(EventHandler<SvnSslClientCertificateEventArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificateEventArgs::Wrapper(e, this);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslClientCertificateEventArgs^>^ e)
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

			event EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ SslClientCertificatePasswordHandlers
			{
				void add(EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ e)
				{
					if(_readOnly)
						throw gcnew InvalidOperationException();
					else if(!e)
						throw gcnew ArgumentNullException("e");

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificatePasswordEventArgs::Wrapper(e, this);

					if(!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ e)
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
			static void ImpSubversionFileUsernameHandler(Object ^sender, SvnUsernameEventArgs^ e);
			static void ImpSubversionFileUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordEventArgs^ e);
			static void ImpSubversionWindowsFileUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordEventArgs^ e);
			static void ImpSubversionFileSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e);
			static void ImpSubversionFileSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateEventArgs^ e);
			static void ImpSubversionFileSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e);
			static void ImpSubversionWindowsSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e);

		private:
			static initonly EventHandler<SvnUsernameEventArgs^>^						_subversionFileUsernameHandler
				= gcnew EventHandler<SvnUsernameEventArgs^>(ImpSubversionFileUsernameHandler);
			static initonly EventHandler<SvnUsernamePasswordEventArgs^>^				_subversionFileUsernamePasswordHandler
				= gcnew EventHandler<SvnUsernamePasswordEventArgs^>(ImpSubversionFileUsernamePasswordHandler);
			static initonly EventHandler<SvnUsernamePasswordEventArgs^>^				_subversionWindowsUsernamePasswordHandler
				= gcnew EventHandler<SvnUsernamePasswordEventArgs^>(ImpSubversionWindowsFileUsernamePasswordHandler);
			static initonly EventHandler<SvnSslServerTrustEventArgs^>^					_subversionFileSslServerTrustHandler
				= gcnew EventHandler<SvnSslServerTrustEventArgs^>(ImpSubversionFileSslServerTrustHandler);
			static initonly EventHandler<SvnSslClientCertificateEventArgs^>^			_subversionFileSslClientCertificateHandler
				= gcnew EventHandler<SvnSslClientCertificateEventArgs^>(ImpSubversionFileSslClientCertificateHandler);
			static initonly EventHandler<SvnSslClientCertificatePasswordEventArgs^>^	_subversionFileSslClientCertificatePasswordHandler
				= gcnew EventHandler<SvnSslClientCertificatePasswordEventArgs^>(ImpSubversionFileSslClientCertificatePasswordHandler);
			static initonly EventHandler<SvnSslServerTrustEventArgs^>^					_subversionWindowsSslServerTrustHandler
				= gcnew EventHandler<SvnSslServerTrustEventArgs^>(ImpSubversionWindowsSslServerTrustHandler);


		public:
			/// <summary>Subversion UsernameHandler file backend (managed representation)</summary>
			static property EventHandler<SvnUsernameEventArgs^>^						SubversionFileUsernameHandler
			{
				EventHandler<SvnUsernameEventArgs^>^ get() { return _subversionFileUsernameHandler; } 
			}

			/// <summary>Subversion UsernamePasswordHandler file backend (managed representation)</summary>
			static property EventHandler<SvnUsernamePasswordEventArgs^>^				SubversionFileUsernamePasswordHandler
			{
				EventHandler<SvnUsernamePasswordEventArgs^>^ get() { return _subversionFileUsernamePasswordHandler; } 
			}

			/// <summary>Subversion UsernameHandler file backend using Windows CryptoStore (managed representation)</summary>
			/// <remarks>Should be added after <see cref="SubversionFileUsernamePasswordHandler" /></remarks>
			static property EventHandler<SvnUsernamePasswordEventArgs^>^				SubversionWindowsUsernamePasswordHandler
			{
				EventHandler<SvnUsernamePasswordEventArgs^>^ get() { return _subversionWindowsUsernamePasswordHandler; } 
			}

			/// <summary>Subversion SslServerTrust file backend (managed representation)</summary>
			static property EventHandler<SvnSslServerTrustEventArgs^>^				SubversionFileSslServerTrustHandler
			{
				EventHandler<SvnSslServerTrustEventArgs^>^ get() { return _subversionFileSslServerTrustHandler; } 
			}

			/// <summary>Subversion SslClientCertificate file backend (managed representation)</summary>
			static property EventHandler<SvnSslClientCertificateEventArgs^>^			SubversionFileSslClientCertificateHandler
			{
				EventHandler<SvnSslClientCertificateEventArgs^>^ get() { return _subversionFileSslClientCertificateHandler; } 
			}

			/// <summary>Subversion SslClientCertificatePassword file backend (managed representation)</summary>
			static property EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ SubversionFileSslClientCertificatePasswordHandler
			{
				EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ get() { return _subversionFileSslClientCertificatePasswordHandler; } 
			}

			/// <summary>Subversion CryptoApi Ssl Trust handler</summary>
			static property EventHandler<SvnSslServerTrustEventArgs^>^				SubversionWindowsSslServerTrustHandler
			{
				EventHandler<SvnSslServerTrustEventArgs^>^ get() { return _subversionWindowsSslServerTrustHandler; } 
			}

		private:
			static IntPtr GetParentHandle(Object ^sender);
			static void ImpDialogUsernameHandler(Object ^sender, SvnUsernameEventArgs^ e);
			static void ImpDialogUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordEventArgs^ e);
			static void ImpDialogSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e);
			static void ImpDialogSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateEventArgs^ e);
			static void ImpDialogSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e);

		public:
			/// <summary>Dialog based UsernameHandler implementation</summary>
			static property EventHandler<SvnUsernameEventArgs^>^						DialogUsernameHandler
			{
				EventHandler<SvnUsernameEventArgs^>^ get() { return gcnew EventHandler<SvnUsernameEventArgs^>(ImpDialogUsernameHandler); }
			}

			/// <summary>Dialog based UsernamePasswordHandler implementation</summary>
			static property EventHandler<SvnUsernamePasswordEventArgs^>^				DialogUsernamePasswordHandler
			{
				EventHandler<SvnUsernamePasswordEventArgs^>^ get() { return gcnew EventHandler<SvnUsernamePasswordEventArgs^>(ImpDialogUsernamePasswordHandler); }
			}

			/// <summary>Dialog based SslServerTrust implementation</summary>
			static property EventHandler<SvnSslServerTrustEventArgs^>^				DialogSslServerTrustHandler
			{
				EventHandler<SvnSslServerTrustEventArgs^>^ get() { return gcnew EventHandler<SvnSslServerTrustEventArgs^>(ImpDialogSslServerTrustHandler); }
			}

			/// <summary>Dialog based SslClientCertificate implementation</summary>
			static property EventHandler<SvnSslClientCertificateEventArgs^>^			DialogSslClientCertificateHandler
			{
				EventHandler<SvnSslClientCertificateEventArgs^>^ get() { return gcnew EventHandler<SvnSslClientCertificateEventArgs^>(ImpDialogSslClientCertificateHandler); }
			}

			/// <summary>Dialog based SslClientCertificatePassword implementation</summary>
			static property EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ DialogSslClientCertificatePasswordHandler
			{
				EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ get() { return gcnew EventHandler<SvnSslClientCertificatePasswordEventArgs^>(ImpDialogSslClientCertificatePasswordHandler); }
			}

		private:
			static void MaybePrintRealm(SvnAuthorizationEventArgs^ e);
			static String^ ReadPassword();
			static void ImpConsoleUsernameHandler(Object ^sender, SvnUsernameEventArgs^ e);
			static void ImpConsoleUsernamePasswordHandler(Object ^sender, SvnUsernamePasswordEventArgs^ e);
			static void ImpConsoleSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e);
			static void ImpConsoleSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateEventArgs^ e);
			static void ImpConsoleSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e);

		public:
			/// <summary>Console based UsernameHandler implementation</summary>
			static property EventHandler<SvnUsernameEventArgs^>^ ConsoleUsernameHandler
			{
				EventHandler<SvnUsernameEventArgs^>^ get() { return gcnew EventHandler<SvnUsernameEventArgs^>(ImpConsoleUsernameHandler); }
			}

			/// <summary>Console based UsernamePasswordHandler implementation</summary>
			static property EventHandler<SvnUsernamePasswordEventArgs^>^ ConsoleUsernamePasswordHandler
			{
				EventHandler<SvnUsernamePasswordEventArgs^>^ get() { return gcnew EventHandler<SvnUsernamePasswordEventArgs^>(ImpConsoleUsernamePasswordHandler); }
			}

			/// <summary>Console based SslServerTrust implementation</summary>
			static property EventHandler<SvnSslServerTrustEventArgs^>^ ConsoleSslServerTrustHandler
			{
				EventHandler<SvnSslServerTrustEventArgs^>^ get() { return gcnew EventHandler<SvnSslServerTrustEventArgs^>(ImpConsoleSslServerTrustHandler); }
			}

			/// <summary>Console based SslClientCertificate implementation</summary>
			static property EventHandler<SvnSslClientCertificateEventArgs^>^ ConsoleSslClientCertificateHandler
			{
				EventHandler<SvnSslClientCertificateEventArgs^>^ get() { return gcnew EventHandler<SvnSslClientCertificateEventArgs^>(ImpConsoleSslClientCertificateHandler); }
			}

			/// <summary>Console based SslClientCertificatePassword implementation</summary>
			static property EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ ConsoleSslClientCertificatePasswordHandler
			{
				EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ get() { return gcnew EventHandler<SvnSslClientCertificatePasswordEventArgs^>(ImpConsoleSslClientCertificatePasswordHandler); }
			}
		};
	}
}
