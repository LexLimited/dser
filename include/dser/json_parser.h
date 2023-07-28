#ifndef __DSER_JSON_PARSER_H__
#define __DSER_JSON_PARSER_H__

#include <cstddef>

namespace dser {

enum json_parser_token {
  TK_STRING,
  TK_NUMERIC,
  TK_CURLY_BRACKET_LEFT,
  TK_CURLY_BRACKET_RIGHT,
  TK_SQUARE_BRACKET_LEFT,
  TK_SQUARE_BRACKET_RIGHT,
  TK_COMMA,
  TK_NULL,
  TK_FALSE,
  TK_TRUE,
  TK_COLON,
  TK_INVALID
};

constexpr const char *stringify_json_parser_token(int tk) {
  if (tk == json_parser_token::TK_STRING)
    return "string";
  if (tk == json_parser_token::TK_NUMERIC)
    return "numeric";
  if (tk == json_parser_token::TK_CURLY_BRACKET_LEFT)
    return "left curly bracket";
  if (tk == json_parser_token::TK_CURLY_BRACKET_RIGHT)
    return "right curly bracket";
  if (tk == json_parser_token::TK_SQUARE_BRACKET_LEFT)
    return "left square bracket";
  if (tk == json_parser_token::TK_SQUARE_BRACKET_RIGHT)
    return "right square bracket";
  if (tk == json_parser_token::TK_COMMA)
    return "comma";
  if (tk == json_parser_token::TK_NULL)
    return "null";
  if (tk == json_parser_token::TK_FALSE)
    return "true";
  if (tk == json_parser_token::TK_TRUE)
    return "false";
  if (tk == json_parser_token::TK_COLON)
    return "colon";
  if (tk == json_parser_token::TK_INVALID)
    return "invalid";
  return "Unhandled token case";
}

enum json_token_semantics {
    SEM_KEY = 0x1,
    SEM_VALUE = 0x2,
    SEM_DELIMITER = 0x4,
    SEM_START_OF_OBJECT = 0x8,
    SEM_END_OF_OBJECT = 0x10,
    SEM_START_OF_ARRAY = 0x20,
    SEM_END_OF_ARRAY = 0x40,
    SEM_END_OF_JSON = 0x80,
    SEM_UNKNOWN = 0x100,
};

struct json_context {
  int depth;
  int line;
  int column;
  int error;
  int curly_bracket_sum;
  int square_bracket_sum;
  int last_token_semantics;
};

enum json_parser_error {
  ERR_UNHANDLED_TOKEN,
  ERR_NO_ERROR,
  ERR_INVALID_TOKEN,
  ERR_INVALID_BRACKET_SEQUENCE,
  ERR_UNKNOWN
};

constexpr const char *stringify_json_parser_error(int err) {
  if (err == json_parser_error::ERR_UNHANDLED_TOKEN)
      return "unhandled token";
  if (err == json_parser_error::ERR_NO_ERROR)
      return "no error";
  if (err == json_parser_error::ERR_INVALID_TOKEN)
      return "invalid token";
  if (err == json_parser_error::ERR_INVALID_BRACKET_SEQUENCE)
      return "invalid bracket sequence";
  return "unknown";
}

struct json_parser_token_t {
    json_parser_token type;
    const char* begin;
    const char* end;
};

class json_parser {
    public:
        json_parser();
        int parse(const char *json_string, size_t n);
        const json_context& context() const noexcept;

    private:
        json_context _context;
};

} // namespace dser

#endif // __DSER_JSON_PARSER_H__
