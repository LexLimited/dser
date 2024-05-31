#ifndef __DSER_POSTGRES_COMMAND_H__
#define __DSER_POSTGRES_COMMAND_H__

#include "postgres_app.h"
#include <iostream>
#include <libpq-fe.h>

namespace dser::postgres
{

    class cmd_result
    {
        public:
            cmd_result(PGresult* result): _result(result)
            {
                std::cout << "result: " << result << std::endl;
            }

            cmd_result(cmd_result&& other)
            {
                this->_result = other._result;
                other._result = nullptr;
            }

            ~cmd_result() { this->free(); }

            const PGresult* result() const noexcept { return this->_result; }
            
            void free() const
            {
                if (this->_result) PQclear(this->_result);
            }

            cmd_result& operator=(const cmd_result& other) = delete;
            
            cmd_result& operator=(cmd_result&& other)
            {
                this->_result = other._result;
                other._result = nullptr;
                return *this;
            }

        private:
            PGresult* _result;
    };

    cmd_result exec_cmd(postgres::app& app, std::string_view cmd);

}

#endif // __DSER_POSTGRES_COMMAND_H__

