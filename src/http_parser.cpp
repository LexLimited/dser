#include "dser/http.h"
#include <iostream>
#include <iterator>
#include <ranges>

#include <dser/http_parser.h>
#include <string_view>
#include <vector>

using namespace dser;

enum class http_message_type {
    RESPONSE,
    REQUEST
};

static http::http_protocol parse_protocol(std::string_view sv) {
    if (sv.size() == 4 && sv.starts_with("HTTP")) return http::http_protocol::HTTP;
    if (sv.size() == 5 && sv.starts_with("HTTPS")) return http::http_protocol::HTTPS;
    return http::http_protocol::INVALID;
}

static http::http_protocol_version parse_protocol_version(std::string_view sv) {
    if (sv.size() != 3) return http::http_protocol_version::INVALID;
    if (sv.starts_with("1.0")) return http::http_protocol_version::V_1_0;
    if (sv.starts_with("1.1")) return http::http_protocol_version::V_1_1;
    if (sv.starts_with("2.0")) return http::http_protocol_version::V_2_0;
    return http::http_protocol_version::INVALID;
}

static http::http_method parse_http_method(std::string_view sv) {
#define DSER_TRY_METHOD(method) if (sv.starts_with(#method)) return http::http_method::method
    DSER_TRY_METHOD(GET);
    DSER_TRY_METHOD(PUT);
    DSER_TRY_METHOD(POST);
    DSER_TRY_METHOD(DELETE);
    DSER_TRY_METHOD(UPDATE);
#undef DSER_TRY_METHOD
    return http::http_method::INVALID;
}

template<std::ranges::range R>
std::string_view string_view_from_range(const R& iter) {
    return std::string_view{ iter.begin(), iter.end() };
}

static int parse_protocol_and_version(std::string_view sv, http::http& h) {
    auto proto_split = sv | std::views::split('/');
    if (std::distance(proto_split.begin(), proto_split.end()) != 2) return -1;

    auto prot_it = proto_split.begin();
    string_view_from_range(*prot_it);

    auto prot = parse_protocol(string_view_from_range(proto_split.front()));
    if (prot == http::http_protocol::INVALID) return -2;
    h.set_protocol(prot);
    
    auto prot_version = parse_protocol_version(string_view_from_range(*std::next(proto_split.begin())));
    if (prot_version == http::http_protocol_version::INVALID) return -3;
    h.set_protocol_vertsion(prot_version);

    return 0;
}

static int parse_status_line(std::string_view str, http::http& http) {
    auto split = str | std::views::split(' ');
    if (std::distance(split.begin(), split.end()) != 3) return -4;

    auto token = split.begin();
    std::string_view token_view = std::string_view { (*token).begin(), (*token).end() };

    if (token == split.end()) return -5;
    
    http_message_type type;
    if (token_view.starts_with("HTTP")) {
        type = http_message_type::RESPONSE;
    } else {
        type = http_message_type::REQUEST;
    }
   
    if (type == http_message_type::RESPONSE) {
        int err = parse_protocol_and_version(token_view, http);
        return err;
    } else {
        auto method = parse_http_method(token_view);
        if (method == http::http_method::INVALID) return -7;
        http.set_method(method);
    }

    ++token;
    token_view = string_view_from_range(*token);
    http.set_url(token_view);

    ++token;
    token_view = string_view_from_range(*token);

    if (type == http_message_type::REQUEST) {
        parse_protocol_and_version(token_view, http);
    } else {
        auto method = parse_http_method(token_view);
        if (method == http::http_method::INVALID) return -8;
        http.set_method(method);
    }

    return 0;
}

static int parse_headers(std::string_view str, http::http& http) {
    for (const auto& line : str | std::views::split('\r')) {
        auto split = std::views::split(line, ":");
        auto it_key = split.begin();
        if (it_key == split.end()) return -9;
        auto it_value = ++it_key;
        if (it_value == split.end()) return -10;
        http.set_header(
                std::string{(*it_key).begin(), (*it_key).end()},
                std::string{(*it_value).begin(), (*it_value).end()});
    }

    return 0;
}

int http_parser::parse_http_string(std::string& str, http::http& h) {
    unsigned end_of_status_line = str.find("\r", 0);
    int err = parse_status_line(std::string_view{ str.begin(), str.begin() + end_of_status_line }, h);
    if (err) return err;

    auto begin_of_part = str.begin() + end_of_status_line;
    auto next_double_delimiter = std::string_view{ begin_of_part, str.end() }.find("\r\n");
    
    err = parse_headers(std::string_view{ begin_of_part, begin_of_part + next_double_delimiter }, h);
    if (err) return err;
    
    begin_of_part += next_double_delimiter + 2;
    if (begin_of_part >= str.end()) return 0;

    h.set_body(std::string{ begin_of_part, str.end() });

    return 0;
}

