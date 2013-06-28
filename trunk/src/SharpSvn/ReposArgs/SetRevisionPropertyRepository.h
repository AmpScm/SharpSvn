#pragma once

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnSetRevisionPropertyRepositoryArgs : public SvnRepositoryClientArgs
	{
		bool _callPreHook;
		bool _callPostHook;
		String^ _author;
	public:
		SvnSetRevisionPropertyRepositoryArgs()
		{
			_author = Environment::UserName;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

	public:
		/// <summary>Gets or sets a value indicating whether to call the hook before changing the revision property</summary>
		property bool CallPreRevPropChangeHook
		{
			bool get()
			{
				return _callPreHook;
			}
			void set(bool value)
			{
				_callPreHook = value;
			}
		}

		/// <summary>Gets or sets a value indicating whether to call the hook after changing the revision property</summary>
		property bool CallPostRevPropChangeHook
		{
			bool get()
			{
				return _callPostHook;
			}
			void set(bool value)
			{
				_callPostHook = value;
			}
		}

		/// <summary>Gets or sets the author passed to the hook scripts</summary>
		property String^ Author
		{
			String^ get()
			{
				return _author;
			}
			void set(String^ value)
			{
				_author = value;
			}
		}
	};
}