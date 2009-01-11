// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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

using System;
using System.Collections.Generic;
using System.Text;

namespace SharpSvn.UI.Authentication
{
	/// <summary>
	///
	/// </summary>
	public class ServerCertificateInfo
	{
		string _hostname;

		/// <summary>
		/// Gets or sets the hostname.
		/// </summary>
		/// <value>The hostname.</value>
		public string Hostname
		{
			get { return _hostname; }
			set { _hostname = value; }
		}
		string _fingerprint;

		/// <summary>
		/// Gets or sets the fingerprint.
		/// </summary>
		/// <value>The fingerprint.</value>
		public string Fingerprint
		{
			get { return _fingerprint; }
			set { _fingerprint = value; }
		}
		string _validFrom;

		/// <summary>
		/// Gets or sets the valid from.
		/// </summary>
		/// <value>The valid from.</value>
		public string ValidFrom
		{
			get { return _validFrom; }
			set { _validFrom = value; }
		}
		string _validTo;

		/// <summary>
		/// Gets or sets the valid to.
		/// </summary>
		/// <value>The valid to.</value>
		public string ValidTo
		{
			get { return _validTo; }
			set { _validTo = value; }
		}
		string _issuer;

		/// <summary>
		/// Gets or sets the issuer.
		/// </summary>
		/// <value>The issuer.</value>
		public string Issuer
		{
			get { return _issuer; }
			set { _issuer = value; }
		}
		string _certificate;

		/// <summary>
		/// Gets or sets the certificate.
		/// </summary>
		/// <value>The certificate.</value>
		public string Certificate
		{
			get { return _certificate; }
			set { _certificate = value; }
		}

		bool _noTrustedIssuer;

		/// <summary>
		/// Gets or sets a value indicating whether [no trusted issuer].
		/// </summary>
		/// <value><c>true</c> if [no trusted issuer]; otherwise, <c>false</c>.</value>
		public bool NoTrustedIssuer
		{
			get { return _noTrustedIssuer; }
			set { _noTrustedIssuer = value; }
		}
		bool _timeError;

		/// <summary>
		/// Gets or sets a value indicating whether [time error].
		/// </summary>
		/// <value><c>true</c> if [time error]; otherwise, <c>false</c>.</value>
		public bool TimeError
		{
			get { return _timeError; }
			set { _timeError = value; }
		}
		bool _invalidCommonName;

		/// <summary>
		/// Gets or sets a value indicating whether [invalid common name].
		/// </summary>
		/// <value><c>true</c> if [invalid common name]; otherwise, <c>false</c>.</value>
		public bool InvalidCommonName
		{
			get { return _invalidCommonName; }
			set { _invalidCommonName = value; }
		}
	}
}
