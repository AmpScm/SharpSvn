#pragma once

namespace SharpSvn {

	/// <summary>Base Extended Parameter container of <see cref="SvnLookClient" /> commands</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLookListArgs : public SvnLookClientArgs
	{
		SvnDepth _depth;

	public:
		DECLARE_EVENT(SvnLookListEventArgs^, List)

	protected:
		virtual void OnList(SvnLookListEventArgs^ e)
		{
			List(this, e);
		}

	internal:
		void InvokeList(SvnLookListEventArgs^ e)
		{
			OnList(e);
		}

	public:
		SvnLookListArgs()
		{
			_depth = SvnDepth::Children;
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

	public:
		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
			}
		}
	};
}