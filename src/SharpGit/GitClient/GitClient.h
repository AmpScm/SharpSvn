#pragma once

#include "GitClientContext.h"
#include "GitClientEventArgs.h"

namespace SharpGit {

	ref class GitStatusArgs;
	ref class GitAddArgs;

	namespace Plumbing {
		ref class GitRepository;
	}
	using ::SharpGit::Plumbing::GitRepository;

	public ref class GitClient : GitClientContext
	{
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