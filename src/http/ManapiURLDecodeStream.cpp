#include <utility>

#include "ManapiDebug.hpp"
#include "http/ManapiURLDecodeStream.hpp"
#include "encoding/ManapiURL.hpp"
#include "encoding/ManapiUnicode.hpp"
#include "../include/ManapiUtils.hpp"

manapi::net::http::url_decode_stream::url_decode_stream() : m_hex_symbols{'\0','\0'} {
    this->m_hex_index = -1;
    this->m_divided = -1;
}

manapi::net::http::url_decode_stream::~url_decode_stream() = default;

int manapi::net::http::url_decode_stream::operator<<(const char &c) {
    return this->handle_char(c);
}

int manapi::net::http::url_decode_stream::operator<<(std::string_view data) {
    for (auto const &c : data) {
        if (auto rhs = this->handle_char(c)) {
            return rhs;
        }
    }
    return 0;
}

std::vector<std::string> manapi::net::http::url_decode_stream::result() {
    if (this->m_hex_index != -1)
        throw manapi::exception (ERR_INVALID_ARGUMENT, "url_decode_stream:failed");
    this->cleanup_uri();
    return std::move(this->m_result);
}

int manapi::net::http::url_decode_stream::divided() {
    return std::exchange(this->m_divided, -1);
}

int manapi::net::http::url_decode_stream::handle_char(const char &c) {
    if (!encoding::url_allowed_symbol(c)) {
        return -1;
    }

    if (this->m_divided == -1) {
        if (this->m_hex_index >= 0) {
            this->m_hex_symbols[this->m_hex_index] = c;

            if (this->m_hex_index == 1) {
                char x = static_cast<char> (manapi::unicode::onehex2dec(static_cast<uint8_t>(this->m_hex_symbols[0])) << 4 | manapi::unicode::onehex2dec(
                                     static_cast<uint8_t>(this->m_hex_symbols[1])));

                if (isalnum(this->m_hex_symbols[0]) && isalnum(this->m_hex_symbols[1])) {
                    this->m_result.back() += x;
                }
                else {
                    this->m_result.back() += '%';
                    this->m_result.back() += this->m_hex_symbols;
                }

                this->m_hex_index = -1;

                return 0;
            }

            this->m_hex_index++;

            return 0;
        }

        if (c == '%' && !this->m_result.empty()) {
            this->m_hex_index = 0;

            return 0;
        }

        if (c == '/') {
            if (this->m_result.empty() || !this->m_result.back().empty()) {
                this->m_result.emplace_back("");
            }
            return 0;
        }

        if (c == '?' || c == '#') {
            this->cleanup_uri ();
            this->m_divided = static_cast<int>(this->m_result.size());
            this->m_result.emplace_back(std::string{c});
            return 0;
        }
    }

    if (!this->m_result.empty()) {
        this->m_result.back().push_back(c);
    }
    return 0;
}

void manapi::net::http::url_decode_stream::cleanup_uri() {
    if (this->m_divided != -1) { return; }
    std::size_t i = this->m_result.size();
    while (i) {
        i--;

        if (this->m_result.operator[](i).empty()) {
            this->m_result.pop_back();
        }
        else {
            break;
        }
    }
}