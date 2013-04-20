#pragma once

#include "GitClientContext.h"
#include "GitClientEventArgs.h"

namespace SharpGit {

	ref class GitStatusArgs;
	ref class GitAddArgs;
	ref class GitDeleteArgs;
	ref class GitCloneArgs;

	namespace Implementation {
		ref class GitLibrary;
	}
	namespace Plumbing {
		ref class GitRepository;
	}

	using ::SharpGit::Implementation::GitLibrary;
	using ::SharpGit::Plumbing::GitRepository;
	using System::Collections::Generic::ICollection;

	public ref class GitClient : GitClientContext
	{
		static ICollection<GitLibrary^>^ _gitLibraries;
	public:
		GitClient() {}

	private:
		static void AssertNotBare(GitRepository ^repository);
		static void AssertNotBare(GitRepository %repository);

	public:
		bool Status(String ^path, EventHandler<GitStatusEventArgs^>^ status);
		bool Status(String ^path, GitStatusArgs ^args, EventHandler<GitStatusEventArgs^>^ status);

		bool Add(String ^path);
		bool Add(String ^path, GitAddArgs ^args);

		bool Delete(String ^path);
		bool Delete(String ^path, GitDeleteArgs ^args);

		bool Clone(System::Uri ^remoteRepository, String ^path);
		bool Clone(String ^localRepository, String ^path);

		bool Clone(System::Uri ^remoteRepository, String ^path, GitCloneArgs ^args);
		bool Clone(String ^localRepository, String ^path, GitCloneArgs ^args);

	private:
		bool CloneInternal(String ^rawRepository, String ^path, GitCloneArgs ^args);
	public:
		/// <summary>Gets the libgit2 version</summary>
		static property System::Version ^Version
		{
			System::Version^ get();
		}

		/// <summary>Gets the SharpGit version</summary>
		static property System::Version ^SharpGitVersion
		{
			System::Version^ get();
		}

		property static System::Collections::Generic::ICollection<GitLibrary^>^ GitLibraries
		{
			System::Collections::Generic::ICollection<GitLibrary^>^ get();
		}

		static property String^ AdministrativeDirectoryName
		{
			String^ get()
			{
				return ".git";
			}
		}

	private:
		~GitClient();
	};

}