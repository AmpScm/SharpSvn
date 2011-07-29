#pragma once
#include "SvnBase.h"

struct IDiaDataSource;
struct IDiaSession;

namespace SharpSvn {
    namespace Implementation
    {

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

        public:
            bool GetAllSourceFiles([Out] IList<String^> ^%files);

        private:
            ~SvnPdbClient();
            !SvnPdbClient();
        };

    }
}