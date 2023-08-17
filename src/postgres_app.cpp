#include "dser/postgres.h"
#include <dser/postgres_app.h>
#include <libpq-fe.h>

using namespace dser::postgres;

app::app() {
    this->_conn = dser::postgres::connect("lex");
}

app::~app() {
    PQfinish(this->_conn);
}

bool app::connection_ok() const {
    ConnStatusType status = PQstatus(this->_conn);
    return status == CONNECTION_OK;
}

