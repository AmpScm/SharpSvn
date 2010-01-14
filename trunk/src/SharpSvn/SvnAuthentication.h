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

#include "AprArray.h"
#include "AprBaton.h"

namespace SharpSvn {
	namespace Security {
		using namespace SharpSvn::Implementation;
		using System::Collections::Generic::Dictionary;
		using System::Collections::Generic::List;
		using System::Text::RegularExpressions::Regex;
		using System::Text::RegularExpressions::RegexOptions;
		using System::Net::ICredentials;
		using System::Net::NetworkCredential;

		ref class SvnAuthentication;

		public enum class SvnAuthenticationCacheType
		{
			None,
			UserName,
			UserNamePassword,
			SslServerTrust,
			SslClientCertificate,
			SslClientCertificatePassword,
		};

		public ref class SvnAuthenticationCacheItem sealed : public SvnBase
		{
			initonly String^ _filename;
			initonly SvnAuthenticationCacheType _type;
			initonly String^ _realm;
			Uri^ _realmUri;
		internal:
			SvnAuthenticationCacheItem(String^ filename, SvnAuthenticationCacheType type, String^ realm);

		public:
			/// <summary>Gets the type of the cached item</summary>
			property SvnAuthenticationCacheType CacheType
			{
				SvnAuthenticationCacheType get()
				{
					return _type;
				}
			}

			/// <summary>Gets the realm of the cached item</summary>
			property String^ Realm
			{
				String^ get()
				{
					return _realm;
				}
			}

			/// <summary>Gets the Uri part of the Realm string</summary>
			property Uri^ RealmUri
			{
				Uri^ get();
			}

			/// <summary>Gets a boolean indicating whether the credentials were deleted since creating this instance</summary>
			property bool IsDeleted
			{
				bool get()
				{
					return !System::IO::File::Exists(_filename);
				}
			}

			/// <summary>Deletes the externally cached credentials</summary>
			/// <remarks>Does /not/ delete in memory credentials. You should call ClearAuthenticationCache() to clear the in memory cache</remarks>
			void Delete();
		};

		public ref class SvnAuthenticationEventArgs abstract: public SvnEventArgs
		{
			initonly bool _maySave;
			initonly String^ _realm;
			bool _save;
			bool _cancel;
			bool _break;
			Uri^ _realmUri;

		internal:
			static initonly Regex^ _reRealmUri = gcnew Regex("^\\<(?<server>[a-z]+://[^ >]+)\\> (?<realm>.*)$", RegexOptions::ExplicitCapture | RegexOptions::Singleline);

		protected:
			SvnAuthenticationEventArgs(String^ realm, bool maySave)
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

			property Uri^ RealmUri
			{
				Uri^ get();
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

		interface class ISvnAuthWrapper : public IDisposable
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

		ref class SvnAuthProviderMarshaller sealed : public IItemMarshaller<ISvnAuthWrapper^>
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
		where T : SvnAuthenticationEventArgs
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
				if (!handler)
					throw gcnew ArgumentNullException("handler");
				else if (!authentication)
					throw gcnew ArgumentNullException("authentication");

				_handler = handler;
				_baton = gcnew AprBaton<SvnAuthWrapper<T>^>(this);
				_authentication = authentication;
				_retryLimit = 128;
			}

			~SvnAuthWrapper()
			{
				delete _baton;
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

		public ref class SvnUserNamePasswordEventArgs : public SvnAuthenticationEventArgs
		{
			initonly String ^_initialUserName;
			String ^_username;
			String ^_password;
		public:
			SvnUserNamePasswordEventArgs(String^ initialUserName, String^ realm, bool maySave)
				: SvnAuthenticationEventArgs(realm, maySave)
			{
				_initialUserName = initialUserName;
				_username = initialUserName ? initialUserName : "";
				_password = "";
			}

			/// <summary>Default username; can be NULL</summary>
			property String^ InitialUserName
			{
				String^ get()
				{
					return _initialUserName;
				}
			}

			/// <summary>The username to authorize with</summary>
			property String^ UserName
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnUserNamePasswordEventArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUserNamePasswordEventArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnUserNamePasswordEventArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		public ref class SvnUserNameEventArgs : public SvnAuthenticationEventArgs
		{
			String ^_username;
		public:
			SvnUserNameEventArgs(String^ realm, bool maySave)
				: SvnAuthenticationEventArgs(realm, maySave)
			{
				_username = "";
			}

			/// <summary>The username to authorize with</summary>
			property String^ UserName
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
			ref class Wrapper sealed : public SvnAuthWrapper<SvnUserNameEventArgs^>
			{
			public:
				Wrapper(EventHandler<SvnUserNameEventArgs^>^ handler, SvnAuthentication^ authentication)
					: SvnAuthWrapper<SvnUserNameEventArgs^>(handler, authentication)
				{
				}

				virtual svn_auth_provider_object_t *GetProviderPtr(AprPool^ pool) override;
			};
		};

		[Flags]
		public enum class SvnCertificateTrustFailures
		{
			None						=	0,
			CertificateNotValidYet		=	SVN_AUTH_SSL_NOTYETVALID,
			CertificateExpired			=	SVN_AUTH_SSL_EXPIRED,
			CommonNameMismatch			=	SVN_AUTH_SSL_CNMISMATCH,
			UnknownCertificateAuthority =	SVN_AUTH_SSL_UNKNOWNCA,

			UnknownSslProviderFailure	=	SVN_AUTH_SSL_OTHER,

			MaskAllFailures				=	CertificateNotValidYet | CertificateExpired | CommonNameMismatch | UnknownCertificateAuthority | UnknownSslProviderFailure
		};


		public ref class SvnSslServerTrustEventArgs : public SvnAuthenticationEventArgs
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
				: SvnAuthenticationEventArgs(realm, maySave)
			{
				if (!certificateCommonName)
					throw gcnew ArgumentNullException("certificateCommonName");
				else if (!certificateFingerprint)
					throw gcnew ArgumentNullException("certificateFingerprint");
				else if (!certificateValidFrom)
					throw gcnew ArgumentNullException("certificateValidFrom");
				else if (!certificateValidUntil)
					throw gcnew ArgumentNullException("certificateValidUntil");
				else if (!certificateIssuer)
					throw gcnew ArgumentNullException("certificateIssuer");
				else if (!certificateValue)
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

		public ref class SvnSslClientCertificateEventArgs : public SvnAuthenticationEventArgs
		{
			String^ _certificateFile;
		public:
			SvnSslClientCertificateEventArgs(String^ realm, bool maySave)
				: SvnAuthenticationEventArgs(realm, maySave)
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

		public ref class SvnSslClientCertificatePasswordEventArgs : public SvnAuthenticationEventArgs
		{
			String^ _password;
		public:
			SvnSslClientCertificatePasswordEventArgs(String^ realm, bool maySave)
				: SvnAuthenticationEventArgs(realm, maySave)
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
	}

	namespace Implementation {
		using namespace SharpSvn::Security;
		using System::Net::ICredentials;
		using System::Net::NetworkCredential;

		ref class SvnCredentialWrapper sealed
		{
			initonly ICredentials^ _credentials;

		public:
			SvnCredentialWrapper(ICredentials^ credentials)
			{
				if (!credentials)
					throw gcnew ArgumentNullException("credentials");

				_credentials = credentials;
			}

			void OnUserNamePassword(Object^ sender, SvnUserNamePasswordEventArgs^ e)
			{
				UNUSED_ALWAYS(sender);
				NetworkCredential^ nc = _credentials->GetCredential(e->RealmUri, "SVN");

				e->UserName = nc->UserName;
				e->Password = nc->Password;
			}

			void OnUserName(Object^ sender, SvnUserNameEventArgs^ e)
			{
				UNUSED_ALWAYS(sender);
				NetworkCredential^ nc = _credentials->GetCredential(e->RealmUri, "SVN");

				e->UserName = nc->UserName;
			}
		};
	}

	ref class SvnClientContext;
	namespace Security {

		/// <summary>Container for all subversion authentication settings on a client</summary>
		public ref class SvnAuthentication : public SvnBase
		{
			initonly SvnClientContext^ _clientContext;
			initonly Dictionary<Delegate^, ISvnAuthWrapper^>^ _wrappers;
			initonly List<ISvnAuthWrapper^>^ _handlers;
			initonly AprPool^ _authPool;
			initonly AprPool^ _parentPool;
			ICredentials^ _defaultCredentials;
			SvnCredentialWrapper^ _credentialWrapper;
			bool _readOnly;
			int _cookie;
			svn_auth_baton_t *_currentBaton;

		internal:
			SvnAuthentication(SvnClientContext^ context, AprPool^ pool);

			property int Cookie
			{
				int get()
				{
					return _cookie;
				}
			}

			/// <summary>Retrieves an authorization baton allocated in the specified pool; containing the current authorization settings</summary>
			svn_auth_baton_t *GetAuthorizationBaton(int% cookie);

		private:
			static apr_hash_t* get_cache(svn_auth_baton_t* baton);
			static apr_hash_t* clone_credentials(apr_hash_t *from, apr_hash_t *to, AprPool^ pool);

			/*internal:
			apr_hash_t *CloneCredentials();*/

		public:
			generic<typename TSvnAuthenticationEventArgs> where TSvnAuthenticationEventArgs : SvnAuthenticationEventArgs
				void SetRetryLimit(EventHandler<TSvnAuthenticationEventArgs>^ handler, int limit)
			{
				if (!handler)
					throw gcnew ArgumentNullException("handler");
				else if (limit < 0)
					throw gcnew ArgumentOutOfRangeException("limit");

				ISvnAuthWrapper^ wrapper;

				if (_wrappers->TryGetValue(handler, wrapper))
				{
					wrapper->RetryLimit = limit;
					_cookie++;
				}
				else
					throw gcnew ArgumentException("HandlerIsNotRegisteredAtThisTime", "handler");
			}

		public:
			/// <summary>Copies the in-memory cache from the specified client to this client; merging and overwriting existing credentials</summary>
			void CopyAuthenticationCache(SvnClientContext^ client);
			/// <summary>Clears the in-memory authentication cache of this client</summary>
			void ClearAuthenticationCache();

		public:
			event EventHandler<SvnUserNamePasswordEventArgs^>^ UserNamePasswordHandlers
			{
				void add(EventHandler<SvnUserNamePasswordEventArgs^>^ e)
				{
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ handler = gcnew SvnUserNamePasswordEventArgs::Wrapper(e, this);

					if (!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnUserNamePasswordEventArgs^>^ e)
				{
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ wrapper;

					if (_wrappers->TryGetValue(e, wrapper))
					{
						_cookie++;
						_wrappers->Remove(e);
						_handlers->Remove(wrapper);
					}
				}
			}

			event EventHandler<SvnUserNameEventArgs^>^ UserNameHandlers
			{
				void add(EventHandler<SvnUserNameEventArgs^>^ e)
				{
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ handler = gcnew SvnUserNameEventArgs::Wrapper(e, this);

					if (!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnUserNameEventArgs^>^ e)
				{
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ wrapper;

					if (_wrappers->TryGetValue(e, wrapper))
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
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ handler = gcnew SvnSslServerTrustEventArgs::Wrapper(e, this);

					if (!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslServerTrustEventArgs^>^ e)
				{
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ wrapper;

					if (_wrappers->TryGetValue(e, wrapper))
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
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificateEventArgs::Wrapper(e, this);

					if (!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslClientCertificateEventArgs^>^ e)
				{
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ wrapper;

					if (_wrappers->TryGetValue(e, wrapper))
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
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ handler = gcnew SvnSslClientCertificatePasswordEventArgs::Wrapper(e, this);

					if (!_wrappers->ContainsKey(e))
					{
						_cookie++;
						_wrappers->Add(e, handler);
						_handlers->Add(handler);
					}
				}

				void remove(EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ e)
				{
					if (_readOnly)
						throw gcnew InvalidOperationException();
					else if (!e)
						return;

					ISvnAuthWrapper^ wrapper;

					if (_wrappers->TryGetValue(e, wrapper))
					{
						_cookie++;
						_wrappers->Remove(e);
						_handlers->Remove(wrapper);
					}
				}
			}

		public:
			/// <summary>Removes all currently registered providers and caching data</summary>
			void Clear();

			/// <summary>Adds all default subversion-configuration-based handlers</summary>
			void AddSubversionFileHandlers();

			/// <summary>Adds all default console handlers</summary>
			void AddConsoleHandlers();

		public:
			/// <summary>Gets a list of subversion cached credentials of the specified type</summary>
			/// <param name="type">The type of credentials to retrieve</param>
			/// <remarks>This list only contains credentials cached by Subversion; not by external authentication stores</remarks>
			Collection<SvnAuthenticationCacheItem^>^ GetCachedItems(SvnAuthenticationCacheType type);

		public:
			/// <summary>Simple credential handler to provide a static credential</summary>
			/// <remarks>When set implements a <see cref="UserNameHandlers" /> and <see cref="UserNamePasswordHandlers" />
			/// instance over the specified credential instance</remarks>
			property ICredentials^ DefaultCredentials
			{
				ICredentials^ get()
				{
					return _defaultCredentials;
				}
				void set(ICredentials^ value)
				{
					if (_defaultCredentials == value)
						return;

					if (_credentialWrapper)
					{
						UserNameHandlers -= gcnew EventHandler<SvnUserNameEventArgs^>(_credentialWrapper, &SvnCredentialWrapper::OnUserName);
						UserNamePasswordHandlers -= gcnew EventHandler<SvnUserNamePasswordEventArgs^>(_credentialWrapper, &SvnCredentialWrapper::OnUserNamePassword);
						_credentialWrapper = nullptr;
					}

					if (value)
					{
						_defaultCredentials = value;
						_credentialWrapper = gcnew SvnCredentialWrapper(value);

						UserNameHandlers += gcnew EventHandler<SvnUserNameEventArgs^>(_credentialWrapper, &SvnCredentialWrapper::OnUserName);
						UserNamePasswordHandlers += gcnew EventHandler<SvnUserNamePasswordEventArgs^>(_credentialWrapper, &SvnCredentialWrapper::OnUserNamePassword);

						SetRetryLimit(gcnew EventHandler<SvnUserNameEventArgs^>(_credentialWrapper, &SvnCredentialWrapper::OnUserName), 1);
						SetRetryLimit(gcnew EventHandler<SvnUserNamePasswordEventArgs^>(_credentialWrapper, &SvnCredentialWrapper::OnUserNamePassword), 1);
					}
				}
			}

		private:
			static void ImpSubversionFileUserNameHandler(Object ^sender, SvnUserNameEventArgs^ e);
			static void ImpSubversionFileUserNamePasswordHandler(Object ^sender, SvnUserNamePasswordEventArgs^ e);
			static void ImpSubversionWindowsFileUserNamePasswordHandler(Object ^sender, SvnUserNamePasswordEventArgs^ e);
			static void ImpSubversionFileSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e);
			static void ImpSubversionWindowsSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e);
			static void ImpSubversionFileSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateEventArgs^ e);
			static void ImpSubversionFileSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e);
			static void ImpSubversionWindowsSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e);


		private:
			static initonly EventHandler<SvnUserNameEventArgs^>^						_subversionFileUserNameHandler
				= gcnew EventHandler<SvnUserNameEventArgs^>(ImpSubversionFileUserNameHandler);
			static initonly EventHandler<SvnUserNamePasswordEventArgs^>^				_subversionFileUserNamePasswordHandler
				= gcnew EventHandler<SvnUserNamePasswordEventArgs^>(ImpSubversionFileUserNamePasswordHandler);
			static initonly EventHandler<SvnUserNamePasswordEventArgs^>^				_subversionWindowsUserNamePasswordHandler
				= gcnew EventHandler<SvnUserNamePasswordEventArgs^>(ImpSubversionWindowsFileUserNamePasswordHandler);
			static initonly EventHandler<SvnSslServerTrustEventArgs^>^					_subversionFileSslServerTrustHandler
				= gcnew EventHandler<SvnSslServerTrustEventArgs^>(ImpSubversionFileSslServerTrustHandler);
			static initonly EventHandler<SvnSslServerTrustEventArgs^>^					_subversionWindowsSslServerTrustHandler
				= gcnew EventHandler<SvnSslServerTrustEventArgs^>(ImpSubversionWindowsSslServerTrustHandler);
			static initonly EventHandler<SvnSslClientCertificateEventArgs^>^			_subversionFileSslClientCertificateHandler
				= gcnew EventHandler<SvnSslClientCertificateEventArgs^>(ImpSubversionFileSslClientCertificateHandler);
			static initonly EventHandler<SvnSslClientCertificatePasswordEventArgs^>^	_subversionFileSslClientCertificatePasswordHandler
				= gcnew EventHandler<SvnSslClientCertificatePasswordEventArgs^>(ImpSubversionFileSslClientCertificatePasswordHandler);
			static initonly EventHandler<SvnSslClientCertificatePasswordEventArgs^>^	_subversionWindowsSslClientCertificatePasswordHandler
				= gcnew EventHandler<SvnSslClientCertificatePasswordEventArgs^>(ImpSubversionWindowsSslClientCertificatePasswordHandler);


		public:
			/// <summary>Subversion UserNameHandler file backend (managed representation)</summary>
			static property EventHandler<SvnUserNameEventArgs^>^						SubversionFileUserNameHandler
			{
				EventHandler<SvnUserNameEventArgs^>^ get() { return _subversionFileUserNameHandler; }
			}

			/// <summary>Subversion UserNamePasswordHandler file backend (managed representation)</summary>
			static property EventHandler<SvnUserNamePasswordEventArgs^>^				SubversionFileUserNamePasswordHandler
			{
				EventHandler<SvnUserNamePasswordEventArgs^>^ get() { return _subversionFileUserNamePasswordHandler; }
			}

			/// <summary>Subversion UserNameHandler file backend using Windows CryptoStore (managed representation)</summary>
			/// <remarks>Should be added after <see cref="SubversionFileUserNamePasswordHandler" /></remarks>
			static property EventHandler<SvnUserNamePasswordEventArgs^>^				SubversionWindowsUserNamePasswordHandler
			{
				EventHandler<SvnUserNamePasswordEventArgs^>^ get() { return _subversionWindowsUserNamePasswordHandler; }
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

			/// <summary>Subversion SslClientCertificatePassword file backend using Windows CryptoStore (managed representation)</summary>
			static property EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ SubversionWindowsSslClientCertificatePasswordHandler
			{
				EventHandler<SvnSslClientCertificatePasswordEventArgs^>^ get() { return _subversionWindowsSslClientCertificatePasswordHandler; }
			}

			/// <summary>Subversion CryptoApi Ssl Trust handler</summary>
			static property EventHandler<SvnSslServerTrustEventArgs^>^				SubversionWindowsSslServerTrustHandler
			{
				EventHandler<SvnSslServerTrustEventArgs^>^ get() { return _subversionWindowsSslServerTrustHandler; }
			}

		private:
			static void MaybePrintRealm(SvnAuthenticationEventArgs^ e);
			static String^ ReadPassword();
			static void ImpConsoleUserNameHandler(Object ^sender, SvnUserNameEventArgs^ e);
			static void ImpConsoleUserNamePasswordHandler(Object ^sender, SvnUserNamePasswordEventArgs^ e);
			static void ImpConsoleSslServerTrustHandler(Object ^sender, SvnSslServerTrustEventArgs^ e);
			static void ImpConsoleSslClientCertificateHandler(Object ^sender, SvnSslClientCertificateEventArgs^ e);
			static void ImpConsoleSslClientCertificatePasswordHandler(Object ^sender, SvnSslClientCertificatePasswordEventArgs^ e);

		public:
			/// <summary>Console based UserNameHandler implementation</summary>
			static property EventHandler<SvnUserNameEventArgs^>^ ConsoleUserNameHandler
			{
				EventHandler<SvnUserNameEventArgs^>^ get() { return gcnew EventHandler<SvnUserNameEventArgs^>(ImpConsoleUserNameHandler); }
			}

			/// <summary>Console based UserNamePasswordHandler implementation</summary>
			static property EventHandler<SvnUserNamePasswordEventArgs^>^ ConsoleUserNamePasswordHandler
			{
				EventHandler<SvnUserNamePasswordEventArgs^>^ get() { return gcnew EventHandler<SvnUserNamePasswordEventArgs^>(ImpConsoleUserNamePasswordHandler); }
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
