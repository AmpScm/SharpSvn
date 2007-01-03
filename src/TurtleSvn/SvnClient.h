
#pragma once

#include "SvnClientContext.h"
#include "SvnUriTarget.h"
#include "SvnPathTarget.h"

namespace QQn {
	namespace Svn {
		/**
			<summary>Subversion client instance; main entrance to Subversion api</summary>
		*/
		public ref class SvnClient sealed : public SvnClientContext
		{
			AprPool^ _pool;
		public:
			/**
				<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
			*/
			SvnClient();
			/**
				<summary>Initializes a new <see cref="SvnClient" /> instance with default properties</summary>
			*/
			SvnClient(AprPool^ pool);

		public:
			property static Version^ Version
			{
				System::Version^ get()
				{
					const svn_version_t* version = svn_client_version();

					return gcnew System::Version(version->major, version->minor, version->patch);
				}
			}

			property static System::Version^ WrapperVersion
			{
				System::Version^ get()
				{
					return (gcnew System::Reflection::AssemblyName(SvnClient::typeid->Assembly->FullName))->Version;
				}
			}

		public:


		private:
			~SvnClient();
		};
	}
}