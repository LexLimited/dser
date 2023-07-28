#ifndef __DSER_JSON_H__
#define  __DSER_JSON_H__

#include <string>
#include <vector>
#include <unordered_map>

namespace dser {

    enum json_primitive_type {
        JSON_STRING,
        JSON_NUMBER,
        JSON_NULL
    };

    enum json_value_type {
        JSON_OBJECT,
        JSON_ARRAY,
        JSON_PRIMITIVE
    };

    class json_primitive {};

    class json_value {
        public:
            json_value_type get_type() const noexcept;

        private:
            json_value_type _type;
    };

    class json_object {
        public:
            const std::vector<object*>& get_children() const noexcept; 

        private:
            std::unordered_map<std::string, json_value> properties;
    };

    class json {
        public:
            

        private:
    };

}

#endif //  __DSER_JSON_H__

