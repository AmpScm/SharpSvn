#pragma once

namespace SharpSvn {

	public ref class SvnMergeSource
	{
		initonly System::Uri^ _uri;
	internal:
		SvnMergeSource(System::Uri^ uri)
		{
			_uri = uri;
		}


	public:
		property System::Uri^ Uri
		{
			System::Uri^ get()
			{
				return _uri;
			}
		}
	};

	public ref class SvnMergeSourcesCollection : public System::Collections::ObjectModel::Collection<SvnMergeSource^>
	{
	};

}