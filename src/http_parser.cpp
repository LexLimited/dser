#include <iostream>
#include <ranges>

#include <dser/http_parser.h>

using namespace dser;

static int parse_status_line(std::string_view str) {
    for (const auto& word : std::views::split(str, " ")) {
        std::cout << std::string_view{word.begin(), word.end()} << std::endl;
    }
}

int http_parser::parse_http_string(std::string& str) {
    
}
