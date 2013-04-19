#pragma once

namespace SharpGit {

	public ref class GitAddArgs : public GitClientArgs 
	{
		bool _append;
		int _stage;

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

		property int Stage
		{
			int get()
			{
				return _stage;
			}
			void set(int value)
			{
				_stage = value;
			}
		}
	};

}