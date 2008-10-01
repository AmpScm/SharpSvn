// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

using namespace System;
using System::Collections::Generic::IList;
using System::Collections::Generic::ICollection;
using System::Collections::Generic::IDictionary;

namespace SharpSvn {
	ref class SvnTarget;
	ref class SvnClientContext;
	ref class SvnPropertyCollection;
	ref class SvnRevisionPropertyCollection;

	public enum class SvnNodeKind
	{
		None		= svn_node_none,
		File		= svn_node_file,
		Directory	= svn_node_dir,
		Unknown		= svn_node_unknown
	};

	namespace Implementation {
		ref class AprPool;

		/// <summary>Main class of Subversion api. This class is responsible for loading the unmanaged api</summary>
		/// <threadsafety static="true" instance="false"/>
		public ref class SvnBase : public System::MarshalByRefObject
		{
		private:
			// Initializes runtime
			static SvnBase();

		internal:
			static void EnsureLoaded();
			static void InstallAbortHandler();
			static const char* _admDir;
			static String^ _clientName;
			initonly static System::Collections::Generic::List<String^>^ _clientNames = gcnew System::Collections::Generic::List<String^>();

		internal:
			SvnBase();

		internal:
			generic<typename T> static __forceinline array<T>^ NewSingleItemArray(T value)
			{
				array<T>^ items = gcnew array<T>(1);
				items[0] = value;

				return items;
			}

			generic<typename T> static __forceinline ICollection<T>^ NewSingleItemCollection(T value)
			{
				return safe_cast<ICollection<T>^>(NewSingleItemArray(value));
			}

			[ThreadStatic]
			static AprPool^ _threadPool;

			/// <summary>Gets a small thread-local pool usable for small one shot actions</summary>
			/// <remarks>The memory allocated by the pool is only freed after the thread is closed; so use with care</remarks>
			static property AprPool^ SmallThreadPool
			{
				AprPool^ get();
			}

		internal:
			static String^ Utf8_PtrToString(const char *ptr);
			static String^ Utf8_PtrToString(const char *ptr, int length);
			static String^ Utf8_PathPtrToString(const char *ptr, AprPool^ pool);
			static Uri^ Utf8_PtrToUri(const char *str, SvnNodeKind nodeKind);
			static array<Byte>^ PtrToByteArray(const char* ptr, int length);

			static Object^ PtrToStringOrByteArray(const char* ptr, int length);

			static DateTime DateTimeFromAprTime(apr_time_t aprTime);
			static apr_time_t AprTimeFromDateTime(DateTime time);

			/// <summary>Gets a boolean indicating whether the path is a file path (and not a Uri)</summary>
			static bool IsNotUri(String^ path);
			static bool IsValidReposUri(Uri^ uri);

			static Uri^ CanonicalizeUri(Uri^ uri);
			static Uri^ PathToUri(String^ path);
			//static String^ CanonicalizePath(String^ path);

			static String^ RemoveDoubleSlashes(String^ input);

			static apr_array_header_t *AllocArray(ICollection<String^>^ strings, AprPool^ pool);
			static apr_array_header_t *AllocCanonicalArray(ICollection<String^>^ paths, AprPool^ pool);
			static apr_array_header_t *AllocPathArray(ICollection<String^>^ paths, AprPool^ pool);
			generic<typename TSvnTarget> where TSvnTarget : SvnTarget
			static apr_array_header_t *AllocCopyArray(ICollection<TSvnTarget>^ targets, AprPool^ pool);

			[DebuggerStepThrough]
			static apr_array_header_t *CreateChangeListsList(ICollection<String^>^ changelists, AprPool^ pool);

			[DebuggerStepThrough]
			static apr_hash_t *CreateRevPropList(SvnRevisionPropertyCollection^ revProps, AprPool^ pool);

			static SvnPropertyCollection^ CreatePropertyDictionary(apr_hash_t* propHash, AprPool^ pool);

			static String^ UriToString(Uri^ value);
			static String^ UriToCanonicalString(Uri^ value);

			static bool PathContainsInvalidChars(String^ path);			

			static array<Char>^ _invalidChars;
		};

		[SecurityPermission(SecurityAction::InheritanceDemand, UnmanagedCode=true), SecurityPermission(SecurityAction::LinkDemand, UnmanagedCode=true)]
		private ref class SvnHandleBase abstract : public System::Runtime::ConstrainedExecution::CriticalFinalizerObject
		{
		private:
			// Initializes runtime
			static SvnHandleBase();

		protected:
			SvnHandleBase();
		};
	}
}
