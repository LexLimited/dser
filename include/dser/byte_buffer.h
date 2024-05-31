#ifndef __DSER_BYTE_BUFFER_H__
#define __DSER_BYTE_BUFFER_H__

#include <iostream>
#include <string>
#include <cmath>
#include <vector>

namespace dser
{

    class byte_buffer
    {
        public:
            byte_buffer(size_t n): _bytes(n), _size(0) {}
        
            const std::vector<char>& bytes() const noexcept { return this->_bytes; }

            size_t size() const noexcept { return this->_size; }

            size_t capacity() const { return this->_bytes.size(); }

            size_t cap_remaining() const { return this->capacity() - this->_size; }
            
            void add_capacity(size_t n)
            {
                size_t sum = this->_size + n;
                size_t new_cap = this->capacity();
                while (new_cap < sum)
                {
                    new_cap *= this->_growth_rate;
                }
                this->resize(new_cap);
            }

            void add_bytes(size_t n)
            {
                this->add_capacity(n);
                this->_size += n;
            }

            bool should_resize() const { return this->_size >= this->_growth_threshold * this->capacity(); }
            
            void resize()
            {
                this->_bytes.resize(this->capacity() * _growth_rate);
            }

            void resize(size_t n)
            {
                this->_bytes.resize(n);
            }

            char* head()
            {
                return this->_bytes.data() + this->_size;
            }

            char* data() noexcept
            {
                return this->_bytes.data();
            }

            std::string string() noexcept
            {
                // return this->_bytes;
                std::string s;
                s.insert(s.begin(), this->_bytes.begin(), this->_bytes.end());
                return s;
            }

            void try_resize()
            {
                if (this->should_resize()) this->resize();
            }

        private:
            std::vector<char> _bytes;
            unsigned _growth_rate = 2;
            float _growth_threshold = 0.75f;
            size_t _size;
    };

}

#endif // __DSER_BYTE_BUFFER_H__

