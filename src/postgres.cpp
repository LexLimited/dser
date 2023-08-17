#include <dser/postgres.h>
#include <dser/file.h>
#include <dser/exception.h>

#include <format>
#include <iostream>

namespace dser::postgres
{

    /*
     *
     *  Connect to postgres server
     *  Returns nullptr in case of failure
     *
     */
    PGconn* connect(std::string_view db)
    {
        return PQconnectdb(std::format("postgres://lex@localhost:5432/{}", db).c_str());
    }

    // path should be relative to db/queries directory
    std::string read_query_from_file(std::string_view path) noexcept(false)
    {
        dser::fs::file q_file;
        int err = q_file.open((std::string("/home/lex/projects/dser/db/queries/") + std::string(path)).data());
        if (err)
        {
            throw dser::exception { "Failed to read query file" };
        }

        return std::string(q_file.data());
    }
}

