#pragma once

#include <memory>
#include <string>
#include <vector>

namespace manapi::net::http {
    class url_decode_stream {
    public:
        url_decode_stream();

        ~url_decode_stream();
        /**
         *
         * @param c Input Char
         * @return 0 on succes, <0 on error
         */
        int operator << (const char &c);
        /**
         *
         * @param c Input String
         * @return 0 on succes, <0 on error
         */
        int operator << (std::string_view data);

        std::vector<std::string> result ();

        int divided();
    private:
        int handle_char (const char &c);

        void cleanup_uri ();

        char m_hex_symbols[2];
        char m_hex_index;

        std::vector<std::string> m_result;
        int m_divided;
    };
}