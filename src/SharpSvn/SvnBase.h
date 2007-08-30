
#pragma once

using namespace System;
using System::Collections::Generic::IList;
using System::Collections::Generic::ICollection;
using System::Collections::Generic::IDictionary;

namespace SharpSvn {
	ref class SvnTarget;

	namespace Apr {
		ref class AprPool;

		public ref class SvnBase : public System::MarshalByRefObject
		{
		private:
			// Initializes runtime
			static SvnBase();

		internal:
			static void EnsureLoaded();

		protected:
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

		internal:
			static String^ Utf8_PtrToString(const char *ptr);
			static String^ Utf8_PtrToString(const char *ptr, int length);
			static array<char>^ PtrToByteArray(const char* ptr, int length);

			static Object^ PtrToStringOrByteArray(const char* ptr, int length);

			static DateTime DateTimeFromAprTime(apr_time_t aprTime);
			static apr_time_t AprTimeFromDateTime(DateTime time);

			/// <summary>Gets a boolean indicating whether the path is a file path (and not a Uri)</summary>
			static bool IsNotUri(String ^path);

			static Uri^ CanonicalizeUri(Uri^ uri);
			static String^ CanonicalizePath(String^ path);

			static apr_array_header_t *AllocArray(ICollection<String^>^ strings, AprPool^ pool);
			static apr_array_header_t *AllocPathArray(ICollection<String^>^ strings, AprPool^ pool);
			static apr_array_header_t *AllocCopyArray(ICollection<SvnTarget^>^ targets, AprPool^ pool);
			static apr_array_header_t *AllocCopyArray(System::Collections::IEnumerable^ targets, AprPool^ pool);

			static IDictionary<String^, Object^>^ CreatePropertyDictionary(apr_hash_t* propHash, AprPool^ pool);
		};

		[SecurityPermission(SecurityAction::InheritanceDemand, UnmanagedCode=true), SecurityPermission(SecurityAction::LinkDemand, UnmanagedCode=true)]
		public ref class SvnHandleBase abstract : public System::Runtime::ConstrainedExecution::CriticalFinalizerObject
		{
		private:
			// Initializes runtime
			static SvnHandleBase();

		protected:
			SvnHandleBase();
		};
	}
}
