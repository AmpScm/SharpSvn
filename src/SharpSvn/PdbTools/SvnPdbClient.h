#pragma once
#include "SvnBase.h"

struct IDiaDataSource;
struct IDiaSession;
struct IDiaSymbol;

namespace SharpSvn {
    namespace Implementation
    {
        using namespace System;
        using namespace System::Collections::Generic;

        public ref class SvnPdbClient sealed : public Implementation::SvnBase
        {
        private:
            IDiaDataSource *_dataSource;
            IDiaSession *_session;
            IDiaSymbol *_scope;
            void Release();

        public:
            SvnPdbClient();

        public:
            property bool Available
            {
                bool get()
                {
                    return (_dataSource != nullptr);
                }
            }

        public:
            bool Open(String^ path);

        private:
            bool GetAllSourceFiles([Out] IList<String^> ^%files);

        public:
            IEnumerable<String^>^ GetAllSourceFiles();

        private:
            ~SvnPdbClient();
            !SvnPdbClient();
        };

    }
}
