#ifndef __DSER_POSTGRES_APP_H__
#define __DSER_POSTGRES_APP_H__

#include "postgres.h"

namespace dser::postgres {

    class app {
        public:
            app();
            ~app();

            PGconn* conn() const noexcept { return this->_conn; }
            bool connection_ok() const;

        private:
            PGconn* _conn;
    };

}

#endif // __DSER_POSTGRES_APP_H__

