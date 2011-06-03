#pragma once

namespace SharpSvn {
	namespace Delta {

		ref class SvnDeltaFileChangeEventArgs;

		public ref class SvnDeltaWindowEventArgs sealed : SvnEventArgs
		{
			svn_txdelta_window_t* _pWindow;
		internal:
			SvnDeltaWindowEventArgs(svn_txdelta_window_t* pWindow)
			{
				if (!pWindow)
					throw gcnew ArgumentNullException("pWindow");

				_pWindow = pWindow;
			}

			SvnDeltaWindowEventArgs(svn_checksum_t* const* sums)
			{
				if (!sums)
					throw gcnew ArgumentNullException("sums");

				_sums = sums;
			}

		protected public:
			/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
			/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
			virtual void Detach(bool keepProperties) override
			{
				try
				{
					if (keepProperties)
					{
					}
				}
				finally
				{
					_pWindow = nullptr;
					_sums = nullptr;

					SvnEventArgs::Detach(keepProperties);
				}
			}

		internal:
			svn_checksum_t* const* _sums;
		};

		public ref class SvnDeltaCompleteEventArgs sealed : SvnEventArgs
		{
			svn_checksum_t* const* _sums;
			String^ _inMd5;
			String^ _outMd5;

		internal:
			SvnDeltaCompleteEventArgs()
			{}
			SvnDeltaCompleteEventArgs(SvnDeltaWindowEventArgs^ from)
			{
				if (!from)
					throw gcnew ArgumentNullException("from");

				_sums = from->_sums;
			}

		public:
			property String^ BaseMD5
			{
				String^ get()
				{
					if (!_inMd5 && _sums && _sums[0])
					{
						AprPool tmp(SvnBase::SmallThreadPool);
						const char* c = svn_checksum_to_cstring(_sums[0], tmp.Handle);

						_inMd5 = SvnBase::Utf8_PtrToString(c);

						if (!_inMd5)
							_inMd5 = "";
					}

					return (_inMd5 && _inMd5->Length) ? _inMd5 : nullptr;
				}
			}

			property String^ ResultMD5
			{
				String^ get()
				{
					if (!_outMd5 && _sums && _sums[1])
					{
						AprPool tmp(SvnBase::SmallThreadPool);
						const char* c = svn_checksum_to_cstring(_sums[1], tmp.Handle);

						_outMd5 = SvnBase::Utf8_PtrToString(c);

						if (!_outMd5)
							_outMd5 = "";
					}

					return (_outMd5 && _outMd5->Length) ? _outMd5 : nullptr;
				}
			}

		protected public:
			/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
			/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
			virtual void Detach(bool keepProperties) override
			{
				try
				{
					if (keepProperties)
					{
					}
				}
				finally
				{
					_sums = nullptr;

					SvnEventArgs::Detach(keepProperties);
				}
			}
		};

		public ref class SvnDeltaTarget abstract : SvnBase
		{
			initonly AprBaton<SvnDeltaTarget^>^ _targetBaton;
			bool _disposed;
		internal:
			SvnDeltaTarget();
			~SvnDeltaTarget();
			virtual svn_error_t* apply_window(svn_txdelta_window_t *window, SvnDeltaWindowEventArgs^% e);
			virtual void final_window(SvnDeltaCompleteEventArgs^ e);

			void AllocHandler(svn_txdelta_window_handler_t& handler, void*& handlerBaton, AprPool^ pool);

		public:
			DECLARE_EVENT(SvnDeltaCompleteEventArgs^, DeltaComplete)

		protected:
			/// <summary>Called before the first <see cref="OnDeltaWindow" /> invocation to allow initializing</summary>
			virtual void OnFileChange(SvnDeltaFileChangeEventArgs^ e)
			{
				UNUSED_ALWAYS(e);
			}

			/// <summary>Called when a new delta window is ready for processing</summary>
			virtual void OnDeltaWindow(SvnDeltaWindowEventArgs^ e)
			{
				UNUSED_ALWAYS(e);
			}

			/// <summary>Called after the last delta window was successfully received</summary>
			virtual void OnDeltaComplete(SvnDeltaCompleteEventArgs^ e)
			{
				DeltaComplete(this, e);
			}

		protected public:
			property bool IsDisposed
			{
				bool get()
				{
					return _disposed;
				}
			}

		internal:
			void InvokeFileChange(SvnDeltaFileChangeEventArgs^ e)
			{
				OnFileChange(e);
			}
		};


		public ref class SvnDeltaFileTransform : SvnDeltaTarget
		{
			initonly String^ _baseFile;
			initonly String^ _toFile;
			initonly bool _verify;
			initonly bool _moveBack;
		public:
			/// <summary>Creates a new <see cref="SvnDeltaFileTransform" /> instance to transform
			/// <paramref name="baseFile" />, into <paramref name="toFile" /> while optionally verifying
			/// <paramref name="baseFile" /> if <paramref name="verifyBase" /> is <c>true</c> and
			/// verification data is available.</summary>
			SvnDeltaFileTransform(String^ baseFile, String^ toFile, bool verifyBase);
			SvnDeltaFileTransform(String^ baseFile, String^ toFile);

		private:
			~SvnDeltaFileTransform();

		internal:
			virtual svn_error_t* apply_window(svn_txdelta_window_t *window, SvnDeltaWindowEventArgs^% e) override;

		private:
			AprPool^ _pool;
			svn_stream_t* _inStream;
			svn_stream_t* _outStream;
			svn_txdelta_window_handler_t _handler;
			svn_checksum_t* const* _checkSums;
			void* _handler_baton;

		protected:
			virtual void OnFileChange(SvnDeltaFileChangeEventArgs^ e) override sealed;
			/// <summary>Called when a delta window is ready for processing</summary>
			virtual void OnDeltaWindow(SvnDeltaWindowEventArgs^ e) override sealed
			{
				UNUSED_ALWAYS(e);
				throw gcnew InvalidOperationException("Implemented at a lower layer");
			}
		};
	}
}
