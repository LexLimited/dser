#include "dser/http.h"
#include "dser/utils.h"
#include <iostream>
#include <iterator>
#include <ranges>

#include <dser/http_parser.h>
#include <string_view>
#include <vector>

using namespace dser::http;

enum class http_message_type
{
    RESPONSE,
    REQUEST
};

http_protocol http_parser::parse_protocol(std::string_view sv)
{
    if (sv.size() == 4 && sv.starts_with("HTTP")) return http_protocol::HTTP;
    if (sv.size() == 5 && sv.starts_with("HTTPS")) return http_protocol::HTTPS;
    return http_protocol::INVALID;
}

http_protocol_version http_parser::parse_protocol_version(std::string_view sv)
{
    if (sv.size() != 3) return http_protocol_version::INVALID;
    if (sv.starts_with("1.0")) return http_protocol_version::V_1_0;
    if (sv.starts_with("1.1")) return http_protocol_version::V_1_1;
    if (sv.starts_with("2.0")) return http_protocol_version::V_2_0;
    return http_protocol_version::INVALID;
}

http_method http_parser::parse_http_method(std::string_view sv)
{
#define DSER_TRY_METHOD(method) if (sv.starts_with(#method)) return http_method::method
    DSER_TRY_METHOD(GET);
    DSER_TRY_METHOD(PUT);
    DSER_TRY_METHOD(POST);
    DSER_TRY_METHOD(DELETE);
    DSER_TRY_METHOD(UPDATE);
#undef DSER_TRY_METHOD
    return http_method::INVALID;
}

template<std::ranges::range R>
std::string_view string_view_from_range(const R& iter)
{
    return std::string_view{ iter.begin(), iter.end() };
}

http_parser::http_parser_error http_parser::parse_protocol_and_version(std::string_view sv, http& h)
{
    auto proto_split = sv | std::views::split('/');
    if (std::distance(proto_split.begin(), proto_split.end()) != 2) return http_parser_error::UNKNOWN;

    auto prot_it = proto_split.begin();
    string_view_from_range(*prot_it);

    auto prot = parse_protocol(string_view_from_range(proto_split.front()));
    if (prot == http_protocol::INVALID) return http_parser_error::INVALID_PROTOCOL_VERSION;
    h.set_protocol(prot);
    
    auto prot_version = parse_protocol_version(string_view_from_range(*std::next(proto_split.begin())));
    if (prot_version == http_protocol_version::INVALID) return http_parser_error::INVALID_PROTOCOL_VERSION;
    h.set_protocol_vertsion(prot_version);

    return http_parser_error::NO_ERROR;
}

http_parser::http_parser_error http_parser::parse_status_line(std::string_view str, http& http)
{
    auto split = str | std::views::split(' ');
    if (std::distance(split.begin(), split.end()) != 3) return http_parser_error::MALFORMED_STATUS_LINE;

    auto token = split.begin();
    std::string_view token_view = std::string_view { (*token).begin(), (*token).end() };
    
    http_message_type type;
    if (token_view.starts_with("HTTP"))
    {
        type = http_message_type::RESPONSE;
    } else
    {
        type = http_message_type::REQUEST;
    }
   
    if (type == http_message_type::RESPONSE)
    {
        auto err = parse_protocol_and_version(token_view, http);
        return err;
    } else
    {
        auto method = parse_http_method(token_view);
        if (method == http_method::INVALID) return http_parser_error::INVALID_METHOD;
        http.set_method(method);
    }

    ++token;
    token_view = string_view_from_range(*token);
    http.set_url(token_view);

    ++token;
    token_view = string_view_from_range(*token);

    if (type == http_message_type::REQUEST)
    {
        parse_protocol_and_version(token_view, http);
    } else
    {
        auto method = parse_http_method(token_view);
        if (method == http_method::INVALID) return http_parser_error::INVALID_METHOD;
        http.set_method(method);
    }

    return http_parser_error::NO_ERROR;
}

http_parser::http_parser_error http_parser::parse_headers(std::string_view str, http& http)
{
    const auto lines = dser::split(str, "\r\n");
    for (const auto& line : lines)
    {
        const auto parts = dser::split_first(line, ":");
        if (!parts.first.size()) return http_parser_error::EMPTY_HEADER_KEY;
        if (!parts.second.size()) return http_parser_error::EMPTY_HEADER_VALUE;
        http.set_header(dser::trim(parts.first), dser::trim(parts.second));
    }

    return http_parser_error::NO_ERROR;
}

http_parser::http_parser_error http_parser::parse_http_string(std::string& str, http& h)
{
    http_parser_error err = http_parser_error::NO_ERROR;

    size_t end_of_status_line = str.find("\r\n");
    if (end_of_status_line == std::string_view::npos) return http_parser_error::MALFORMED_HTTP;

    err = this->parse_status_line({ str.begin(), str.begin() + end_of_status_line }, h);
    if (err != http_parser_error::NO_ERROR) return err;
    
    std::vector<std::string> parts = dser::split(dser::trim_any({ str.begin() + end_of_status_line, str.end() }, "\r\n"), "\r\n\r\n");

    if (parts.size() < 2) return http_parser_error::NO_ERROR;
    err = this->parse_headers(parts[0], h);
    if (err != http_parser_error::NO_ERROR) return err;

    if (parts.size() < 3) return http_parser_error::NO_ERROR;
    h.set_body(parts[1]);

    return http_parser_error::NO_ERROR;
}

