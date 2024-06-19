#include <cctype>
#include <cwctype>
#include <iostream>

#include <dser/json_parser2.h>
#include <stop_token>

namespace dser::v2
{

    namespace
    {

        using token_type = json_parser::token_type;

        constexpr const char* stringify_token_type(token_type type)
        {
            switch (type)
            {
                case token_type::INVALID: return "INVALID";
                case token_type::CURLY_LEFT: return "CURLY_LEFT";
                case token_type::CURLY_RIGHT: return "CURLY_RIGHT";
                case token_type::SQUARE_LEFT: return "SQUARE_LEFT";
                case token_type::SQUARE_RIGHT: return "SQUARE_RIGHT";
                case token_type::NUMERIC: return "NUMERIC";
                case token_type::INTEGER: return "INTEGER";
                case token_type::FLOAT: return "FLOAT";
                case token_type::COMMA: return "COMMA";
                case token_type::STRING: return "STRING";
                case token_type::OBJECT_KEY: return "OBJECT_KEY";
                case token_type::NIL: return "NIL";
                case token_type::TRUE: return "TRUE";
                case token_type::FALSE: return "FALSE";
                case token_type::VOID: return "VOID";
            }

            return "Unknown";
        }

    }

    /**
     * Updates the current and previous token in the context
     */
    void json_parser::ctx_begin_token(token_type type)
    {
        this->_ctx.prev_token = this->_ctx.last_token.type;
        this->_ctx.last_token.type = type;
        this->_ctx.last_token.begin = this->_pointer;
        this->_ctx.last_token.pos.line = this->_ctx.pos.line;
        this->_ctx.last_token.pos.col = this->_ctx.pos.col;
    }

    /**
     * Finalazes the current token in the context
     */
    void json_parser::ctx_end_token()
    {
        this->_ctx.last_token.length = this->_pointer - this->_ctx.last_token.begin;
    
        printf("Read token `%.*s` at %lu:%lu\n",
                static_cast<int>(this->_ctx.last_token.length),
                this->_ctx.last_token.begin,
                this->_ctx.pos.line,
                this->_ctx.pos.col);
    }

    /**
     * Initializes the context and reads all tokens until EOF is reached
     */
    int json_parser::parse(std::string_view data)
    {
        this->_pointer = data.data();
        this->_end = this->_pointer + data.size();

        token tk;
        while (!next_token(tk))
        {
            printf("Read token '%s' at %lu:%lu\n",
                    stringify_token_type(tk.type),
                    this->_ctx.last_token.pos.line,
                    this->_ctx.last_token.pos.col);
        }

        return 0;
    }

    void json_parser::increment_line()
    {
        ++this->_ctx.pos.line;
        this->_ctx.pos.col = 0;
    }
    
    void json_parser::increment_col()
    {
        ++this->_ctx.pos.col;
    }

    /**
     * Returns `false` iff EOF is reached
     */
    bool json_parser::increment_pointer()
    {
        if (*this->_pointer == '\n')
            increment_line();
        else
            increment_col();

        ++this->_pointer;
        return this->_pointer != this->_end;
    }

    /**
     * Reads the first character of the token and determines the
     * type of the token. The predicted type should be correct
     * if the token is valid. If the token is a string literal,
     * performs lookahead to determine wheterh it is a value or an object key
     */
    json_parser::token_type json_parser::predict_token()
    {
        if (this->_pointer == this->_end)
            return token_type::INVALID;

        char c = *this->_pointer;
        switch (c)
        {
            case '{': return token_type::CURLY_LEFT;
            case '}': return token_type::CURLY_RIGHT;
            case '[': return token_type::SQUARE_LEFT;
            case ']': return token_type::SQUARE_RIGHT;
            case ',': return token_type::COMMA;

            // TODO! add lookahead for object keys
            case '"': return token_type::STRING;
            case 'n': return token_type::NIL;
            case 't': return token_type::TRUE;
            case 'f': return token_type::FALSE;
        }

        if (std::isdigit(c)) return token_type::NUMERIC;
        return token_type::INVALID;
    }

    static bool is_token_special_character(token_type type)
    {
        switch (type)
        {
            case token_type::CURLY_LEFT: return true;
            case token_type::CURLY_RIGHT: return true;
            case token_type::SQUARE_LEFT: return true;
            case token_type::SQUARE_RIGHT: return true;
            case token_type::COMMA: return true;
            default: return false;
        }

        return false;
    }

    /**
     * Predicts the token and reads it. If the token is valid,
     * _pointer will point to the byte succeeding the token after token is read.
     */
    int json_parser::read_token()
    {
        skip_ws();

        const auto type = predict_token();
        ctx_begin_token(type);

        if (type == token_type::INVALID)
        {
            read_token_invalid();
            return -1;
        }
        else if (is_token_special_character(type))
        {
            read_token_special_character();
        }
        else
        {
            printf("Token %s is not yet implemented at %lu:%lu\n",
                    stringify_token_type(type),
                    this->_ctx.last_token.pos.line,
                    this->_ctx.last_token.pos.col);
            return -1;
        }

        return 0;
    }

    /**
     * Reads the next token and writes it to `tk`
     */
    int json_parser::next_token(token& tk)
    {
        int read_status = read_token();
        tk = this->_ctx.last_token;
        return read_status;
    }

    void json_parser::skip_ws()
    {
        const auto* last_ptr = this->_pointer;
        while (this->_pointer != this->_end && std::isspace(*this->_pointer))
        {
            ++this->_pointer;
            if (*this->_pointer == '\n')
                increment_line();
            else
                increment_col();
        }

        std::cout << "Skipped " << this->_pointer - last_ptr << " whitespaces" << std::endl;
    }

    static bool is_special_character(char c)
    {
        return c == '{' || c == '}' || c == '[' || c == ']' || c == ',';
    }

    void json_parser::read_token_special_character()
    {
        increment_pointer();
    }

    /**
     * Reads until a special character
     * or a whitespace is reached
     */
    void json_parser::read_token_invalid()
    {
        char c = *this->_pointer;
        while (!is_special_character(c) && !std::isspace(c))
        {
            if (!increment_pointer()) break;
            c = *this->_pointer;
        }
    }

}
