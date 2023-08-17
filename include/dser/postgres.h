#ifndef __DSER_POSTGRES_H__
#define __DSER_POSTGRES_H__

#include <string>

#include <libpq-fe.h>

namespace dser::postgres
{

    PGconn* connect(std::string_view db);

    // path should be relative to db/queries directory
    std::string read_query_from_file(std::string_view path) noexcept(false);

}

#endif // __DSER_POSTGRES_H__

