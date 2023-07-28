#include <cctype>
#include <cstring>
#include <ctype.h>
#include <cstdio>
#include <string>

#include <dser/json_parser.h>

bool is_symbol_empty(char c) {
    return c == ' ' || c == '\n';
}

static dser::json_parser_token predict_token_type(char c) {
    if (c == '{') return dser::json_parser_token::TK_CURLY_BRACKET_LEFT;
    if (c == '}') return dser::json_parser_token::TK_CURLY_BRACKET_RIGHT;
    if (c == '"') return dser::json_parser_token::TK_STRING;
    if (c == ':') return dser::json_parser_token::TK_COLON;
    if (c == ',') return dser::json_parser_token::TK_COMMA;
    if (isdigit(c)) return dser::json_parser_token::TK_NUMERIC;
    if (c == 'n') return dser::json_parser_token::TK_NULL;
    if (c == 't') return dser::json_parser_token::TK_TRUE;
    if (c == 'f') return dser::json_parser_token::TK_FALSE;
    if (c == '[') return dser::json_parser_token::TK_SQUARE_BRACKET_LEFT;
    if (c == ']') return dser::json_parser_token::TK_SQUARE_BRACKET_RIGHT;

    return dser::json_parser_token::TK_INVALID;
}

const char* skip_empty_symbols(const char* begin, const char* end, dser::json_context* context) {    
    const char* head = begin;
    while(head < end && is_symbol_empty(*head)) {
        if (*head == '\n') {
            ++context->line;
            context->column = 1;
        } else ++context->column;
        ++head;
    }
    printf("Empty space of length %d\n-  -  -  -  -  -  -\n", head - begin);
    return head;
}

const char* seek_next_json_token(
        const char* begin, const char* end,
        dser::json_parser_token_t* tk,
        dser::json_context* context)
{
    tk->begin = skip_empty_symbols(begin, end, context);
    tk->type = predict_token_type(*(tk->begin));
    return tk->begin;
}

const char* parse_string(dser::json_parser_token_t* tk, const char* end, dser::json_context* context) {
    char c;
    std::string parsed_value = "";
    ++tk->begin;
    const char* head = tk->begin;
    while(head < end && (c = *head) != '"')
    {
        parsed_value += c;
        ++head;
    }
    tk->end = head;
    return head + 1;
}

const char* parse_numeric(dser::json_parser_token_t* tk, const char* end, dser::json_context* context) {
    char c;
    std::string parsed_string = "";
    const char* head = tk->begin;
    while (head < end && (isdigit(c = *head) || c == '.')) {
        parsed_string += c;
        ++head;
    }
    return tk->end = head;
}

const char* parse_exact(
        dser::json_parser_token_t* tk, const char* end,
        const char* exact_str, dser::json_context* context)
{
    size_t str_len = strlen(exact_str);
    if (end - tk->begin < 4) {
        context->error = dser::json_parser_error::ERR_INVALID_TOKEN;
        return nullptr;
    }

    if (!strncmp(tk->begin, exact_str, str_len)) {
        return tk->end = tk->begin + str_len;
    }

    context->error = dser::json_parser_error::ERR_INVALID_TOKEN;
    return nullptr;
}

const char* parse_token(
        const char* end,
        dser::json_parser_token_t* tk,
        dser::json_context* context)
{
    if (tk->type == dser::json_parser_token::TK_STRING) return parse_string(tk, end, context);
    
    if (tk->type == dser::json_parser_token::TK_CURLY_BRACKET_LEFT) {
        ++context->depth;
        ++context->curly_bracket_sum;
        return tk->end = tk->begin + 1;
    }
    
    if (tk->type == dser::json_parser_token::TK_CURLY_BRACKET_RIGHT) {
        if (context->curly_bracket_sum == 1) {
            context->last_token_semantics =
                dser::json_token_semantics::SEM_END_OF_OBJECT |
                dser::json_token_semantics::SEM_END_OF_JSON;

            return tk->begin;
        }
        
        if (context->curly_bracket_sum == 0) {
            context->error = dser::json_parser_error::ERR_INVALID_BRACKET_SEQUENCE;
            return nullptr;
        }
        
        --context->depth;
        --context->curly_bracket_sum;
        return tk->end = tk->begin + 1;
    }
    
    if (tk->type == dser::json_parser_token::TK_SQUARE_BRACKET_LEFT) {
        ++context->depth;
        ++context->square_bracket_sum;
        return tk->end = tk->begin + 1;
    }
    
    if (tk->type == dser::json_parser_token::TK_SQUARE_BRACKET_RIGHT) {
        if (context->square_bracket_sum == 0) {
            context->error = dser::json_parser_error::ERR_INVALID_BRACKET_SEQUENCE;
            return nullptr;
        }

        --context->depth;
        --context->square_bracket_sum;
        return tk->end = tk->begin + 1;
    }
    
    if (tk->type == dser::json_parser_token::TK_NUMERIC) return parse_numeric(tk, end, context);
    if (tk->type == dser::json_parser_token::TK_COLON) {
        return tk->end = tk->begin + 1;
    }
    
    if (tk->type == dser::json_parser_token::TK_COMMA) {
        return tk->end = tk->begin + 1;
    }
    
    if (tk->type == dser::json_parser_token::TK_NULL) return parse_exact(tk, end, "null", context);
    if (tk->type == dser::json_parser_token::TK_TRUE) return parse_exact(tk, end, "true", context);
    if (tk->type == dser::json_parser_token::TK_FALSE) return parse_exact(tk, end, "false", context);
    
    context->error = dser::json_parser_error::ERR_INVALID_TOKEN;
    return nullptr;
}

namespace dser {

    json_parser::json_parser() {
        this->_context = {
            .depth = 0,
            .line = 1,
            .column = 1,
            .error = json_parser_error::ERR_NO_ERROR,
            .curly_bracket_sum = 0,
            .square_bracket_sum = 0,
            .last_token_semantics = json_token_semantics::SEM_UNKNOWN
        };
    }

    int json_parser::parse(const char* json_string, size_t n)
    {
        const char* head = json_string;
        const char* end = json_string + n;
        
        json_parser_token_t tk {
            .type = json_parser_token::TK_INVALID,
            .begin = nullptr,
            .end = nullptr
        };
        
        while(
                (head = seek_next_json_token(head, end, &tk, &this->_context)) &&
                tk.type != json_parser_token::TK_INVALID)
        {
            head = parse_token(end, &tk, &this->_context);
            if (this->_context.last_token_semantics & json_token_semantics::SEM_END_OF_JSON) {
                return this->_context.error;
            }
            if (!head) {
                return this->_context.error;
            }
            if (head == end) {
                return this->_context.error;
            }

            this->_context.column += tk.end - tk.begin;
            printf("TOKEN: %.*s\nline: %d, column: %d, length: %d\n--------------------------\n",
                    tk.end - tk.begin, tk.begin,
                    this->_context.line, this->_context.column, tk.end - tk.begin);
            // printf("Next token at %p at depth %d at line %d: \n\n",
            //        (void*)head, this->_context.depth, this->_context.line);
        }

        return 0;
    }

    const json_context& json_parser::context() const noexcept {
        return this->_context;
    }
}

