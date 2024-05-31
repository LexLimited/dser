#include "dser/postgres_app.h"
#include <dser/postgres_command.h>
#include <libpq-fe.h>

using namespace dser;

postgres::cmd_result postgres::exec_cmd(app &app, std::string_view cmd)
{
    PGresult* result = PQexec(app.conn(), cmd.data());
    std::cout << "cmd_result::result: " << result << std::endl;
    return cmd_result(result);
}

