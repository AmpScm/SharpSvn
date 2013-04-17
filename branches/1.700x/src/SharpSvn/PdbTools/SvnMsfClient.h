#pragma once
#include "SvnBase.h"

#ifndef NDEBUG
struct IDiaDataSource;
struct IDiaSession;

namespace SharpSvn {
    namespace Implementation
    {
        public ref class SvnMsfClient : public Implementation::SvnBase
        {
        private:
            HANDLE _file;
            HANDLE _mapping;
            Byte* _data;
            unsigned _size;
            void Release();
            array<array<DWORD>^>^ _streams;
            array<DWORD>^ _streamSizes;
            System::Collections::Generic::Dictionary<String^, int>^ _streamNames;

        private:
            int _pageBytes;
            int _flagPage;
            int _filePages;
            int _rootBytes;
            int _nStreams;

        public:
            SvnMsfClient();

        private:
            ~SvnMsfClient();
            !SvnMsfClient();

        public:
            bool Open(String^ path);
            System::IO::Stream^ GetStream(int index);
            System::IO::Stream^ GetStream(String^ name);

            ICollection<String^>^ GetStreamNames();

        public:
            property int StreamCount
            {
                int get() { return _nStreams; }
            }

        private:
            Exception^ VerifyFile();
            int PagesRequired(int size);
            void LoadNames();
        internal:
            __int64 Read(array<DWORD>^ pages, __int64 pos, __int64 size, array<Byte>^ buffer, int index, int length);
        };

        ref class SvnMsfStream sealed : public System::IO::Stream
        {
        private:
            initonly array<DWORD>^ _pages;
            initonly SvnMsfClient^ _client;
            initonly __int64 _size;
            __int64 _pos;

        public:
            SvnMsfStream(SvnMsfClient^ client, array<DWORD>^ pages, __int64 size);

        public:
            property bool CanRead
            {
                virtual bool get() override { return true; }
            }

            property bool CanSeek
            {
                virtual bool get() override { return true; }
            }

            property bool CanWrite
            {
                virtual bool get() override { return false; }
            }

        public:
            property __int64 Length
            {
                virtual __int64 get() override { return _size; }
            }

            property __int64 Position
            {
                virtual __int64 get() override { return _pos; }
                virtual void set(__int64 value) override { if (value != _pos) Seek(value, System::IO::SeekOrigin::Begin); }
            }

        public:
            virtual void Flush() override
            {}

            virtual __int64 Seek(__int64 offset, System::IO::SeekOrigin origin) override;

            virtual void SetLength(__int64 value) override
            {
                UNUSED_ALWAYS(value);
                throw gcnew NotImplementedException();
            }

            virtual int Read(array<Byte> ^data, int start,int length) override;

            virtual void Write(array<Byte> ^data, int start,int length) override
            {
                UNUSED_ALWAYS(data);
                UNUSED_ALWAYS(start);
                UNUSED_ALWAYS(length);
                throw gcnew NotImplementedException();
            }
        };
    }
}
#endif