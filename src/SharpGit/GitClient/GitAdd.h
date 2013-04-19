#pragma once

namespace SharpGit {

	public ref class GitAddArgs : public GitClientArgs 
	{
		bool _append;

	public:
		property bool Append
		{
			bool get()
			{
				return _append;
			}
			void set(bool value)
			{
				_append = value;
			}
		}
	};

}