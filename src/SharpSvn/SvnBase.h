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

#include <svn_types.h>

namespace SharpSvn {
    using namespace System;
    using System::Collections::Generic::IList;
    using System::Collections::Generic::ICollection;
    using System::Collections::Generic::IDictionary;

    ref class SvnTarget;
    ref class SvnClientContext;
    ref class SvnPropertyCollection;
    ref class SvnRevisionPropertyCollection;

    public enum class SvnNodeKind
    {
        None                    = svn_node_none,
        File                    = svn_node_file,
        Directory               = svn_node_dir,
        Unknown                 = svn_node_unknown,
        SymbolicLink            = svn_node_symlink
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
            static void InstallSslDialogHandler();
            static const char* _admDir;
            static String^ _clientName;
            static bool _aprInitialized;
            initonly static Object^ _ensurerLock = gcnew Object();
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

            generic<typename T> static __forceinline T FirstOf(System::Collections::Generic::IEnumerable<T>^ list)
            {
                for each (T i in list)
                {
                    return i;
                }

                throw gcnew InvalidOperationException();
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
            static String^ Utf8_PtrToString(const svn_string_t* ptr)
            {
                return Utf8_PtrToString(ptr->data, ptr->len);
            }
            static String^ Utf8_PathPtrToString(const char *ptr, AprPool^ pool);
            static Uri^ Utf8_PtrToUri(const char *str, SvnNodeKind nodeKind);
            static array<Byte>^ PtrToByteArray(const char* ptr, int length);

            static Object^ PtrToStringOrByteArray(const char* ptr, int length);

            static DateTime DateTimeFromAprTime(apr_time_t aprTime);
            static apr_time_t AprTimeFromDateTime(DateTime time);

            /// <summary>Gets a boolean indicating whether the path is a file path (and not a Uri)</summary>
            static bool IsNotUri(String^ path);
            static bool IsValidReposUri(Uri^ uri);
            static bool IsValidRelpath(String^ path);

            static Uri^ CanonicalizeUri(Uri^ uri);
            static Uri^ PathToUri(String^ path);
            //static String^ CanonicalizePath(String^ path);

            static String^ RemoveDoubleSlashes(String^ input);

            static apr_array_header_t *AllocArray(ICollection<String^>^ strings, AprPool^ pool);
            static apr_array_header_t *AllocUriArray(ICollection<String^>^ uris, AprPool^ pool);
            static apr_array_header_t *AllocRelpathArray(ICollection<String^>^ relpaths, AprPool^ pool);
            static apr_array_header_t *AllocDirentArray(ICollection<String^>^ paths, AprPool^ pool);
            generic<typename TSvnTarget> where TSvnTarget : SvnTarget
            static apr_array_header_t *AllocCopyArray(ICollection<TSvnTarget>^ targets, AprPool^ pool);

            generic<typename T>
            static array<T>^ ExtendArray(array<T>^ from, T value);

            generic<typename T>
            static array<T>^ ExtendArray(array<T>^ from, array<T>^ values);

            generic<typename T>
            static array<T>^ ExtendArray(array<T>^ from, ICollection<T>^ values);

            [DebuggerStepThrough]
            static apr_array_header_t *CreateChangeListsList(ICollection<String^>^ changelists, AprPool^ pool);

            [DebuggerStepThrough]
            static apr_hash_t *CreateRevPropList(SvnRevisionPropertyCollection^ revProps, AprPool^ pool);

            static SvnPropertyCollection^ CreatePropertyDictionary(apr_hash_t* propHash, AprPool^ pool);

            static String^ UriToString(Uri^ value);
            static String^ UriToCanonicalString(Uri^ value);

            static bool PathContainsInvalidChars(String^ path);

            static String^ EnsureSafeAprArgument(String^ argument, bool preferQuotes);

            static array<char>^ _invalidCharMap;

            static property array<char>^ InvalidCharMap
            {
                array<char>^ get();
            }

        static void GenerateInvalidCharMap();
        };

        [SecurityPermission(SecurityAction::InheritanceDemand, UnmanagedCode=true), SecurityPermission(SecurityAction::LinkDemand, UnmanagedCode=true)]
        private ref class SvnHandleBase abstract : public System::Runtime::ConstrainedExecution::CriticalFinalizerObject
        {
        private:
            // Initializes runtime
            static SvnHandleBase();

        internal:
            // Make sure the .Net 4.0 beta optimizer doesn't optimize our unmanaged initialization away
            static volatile apr_pool_t* _ultimateParentPool = nullptr;

        protected:
            SvnHandleBase();
        };
    }
}
