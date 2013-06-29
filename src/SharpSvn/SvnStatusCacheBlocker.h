#pragma once

namespace SharpSvn {
	namespace Implementation {

		using System::Collections::Generic::Stack;

		public ref class SvnStatusCacheBlocker sealed : public SvnBase, System::IDisposable
        {
            initonly String^ _fullPath;
            static initonly __int64 _sessionId;
            bool _blocked;

            static SvnStatusCacheBlocker();

        public:
            SvnStatusCacheBlocker(String^ path);

        private:
            static bool SendCommand(Byte cmd, String^ value);

        public:
            static bool Release(String^ path);
            static bool Crawl(String^ path);
            static bool Refresh();
            bool Release();

        public:
            property bool Blocked
            {
                bool get() { return _blocked; }
            }

        protected:
            static property __int64 WindowsLogonSessionId
            {
                __int64 get()
                {
                    return _sessionId;
                }
            }

        private:
            ~SvnStatusCacheBlocker();
        };
    }
}