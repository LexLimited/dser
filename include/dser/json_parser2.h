#ifndef __DSER_INCLUDE_JSON_PARSER_2__
#define __DSER_INCLUDE_JSON_PARSER_2__

#include <string_view>

namespace dser::v2
{

    class json_parser
    {
        public:
            enum class token_type
            {
                INVALID = 0,
                CURLY_LEFT,
                CURLY_RIGHT,
                SQUARE_LEFT,
                SQUARE_RIGHT,

                // Used before determined whether int or float
                NUMERIC,

                INTEGER,
                FLOAT,
                COMMA,
                STRING,
                OBJECT_KEY,
                NIL,
                TRUE,
                FALSE,
                VOID
            };

            struct position
            {
                size_t line{ 0 };
                size_t col{ 0 };
            };

            struct token
            {
                token_type type{ token_type::VOID };
                const char* begin{ nullptr };
                size_t length{ 0 };
                position pos;
            };

            struct context
            {
                token last_token;
                token_type prev_token{ token_type::VOID };

                position pos;

                // Keep track of nested objects and arrays
                size_t curly_sum{ 0 };
                size_t square_sum{ 0 };
            };

            enum json_item_type
            {
                INTEGER,
                FLOAT,
                STRING,
                NIL,
                BOOLEAN,
                ARRAY,
                OBJECT
            };

            struct json_item
            {
                json_item_type type;
            };

        public:
            int parse(std::string_view data);

        private:
            void ctx_begin_token(token_type type);
            void ctx_end_token();
            bool increment_pointer();
            void increment_line();
            void increment_col();
            token_type predict_token();
            int read_token();
            int next_token(token&);
            void skip_ws();

            void read_token_special_character();
            void read_token_invalid();

            const char* _pointer;
            const char* _end;
            context _ctx;
    };

}

#endif // __DSER_INCLUDE_JSON_PARSER_2__

