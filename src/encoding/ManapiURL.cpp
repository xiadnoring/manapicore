#include <fstream>
#include <iomanip>
#include <array>

#include "encoding/ManapiURL.hpp"
#include "encoding/ManapiUnicode.hpp"
#include "../include/ManapiUtils.hpp"

static constexpr auto manapi__url_allowed_symbols = [] {
    std::array<bool, 256> tbl{};
    tbl['-'] = tbl['_'] = tbl['.'] = tbl['~'] = tbl['!'] = tbl['*'] = true;
    tbl['\''] = tbl['*'] = tbl['('] = tbl[')'] = tbl[';'] = tbl['/'] = tbl['?'] = true;
    tbl[':'] = tbl['@'] = tbl['&'] = tbl['='] = tbl['+'] = tbl['$'] = tbl[','] = true;
    tbl['.'] = tbl['#'] = tbl['['] = tbl[']'] = tbl['%'] = true;

    for (uint8_t c = '0'; c <= '9'; c++) tbl[c] = true;
    for (uint8_t c = 'a'; c <= 'z'; c++) tbl[c] = true;
    for (uint8_t c = 'A'; c <= 'Z'; c++) tbl[c] = true;

    return tbl;
}();

void manapi::encoding::encode_url(std::string &dest, std::string_view str) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (const auto &c: str)
    {
        if (isalnum(c) || c == '_' || c == '-' || c == '.' || c == '~')
        {
            escaped << c;
            continue;
        }

        if (c == ' ')
        {
            escaped << '+';
            continue;
        }

        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int(static_cast<unsigned int> (c));
        escaped << std::nouppercase;
    }

    dest += escaped.str();
}

std::string manapi::encoding::encode_url(std::string_view str) {
    std::string dest;
    encode_url (dest, str);
    return std::move(dest);
}

void manapi::encoding::decode_url(std::string &dest, std::string_view str) {
    std::size_t i;

    for (i = 0; i < str.size(); i++){
        if (!encoding::url_allowed_symbol(str[i])) {
            throw manapi::exception(ERR_INVALID_ARGUMENT, "decode_url: invalid char");
        }

        if(str[i] != '%'){
            if(str[i] == '+') {
                dest += ' ';
            }
            else {
                dest += str[i];
            }
        }
        else{
            if (str.size() <= i + 2) {
                dest += str[i];
                continue;
            }
            if (!(isalnum(str[i+1]) && isalnum(str[i+2]))) {
                dest += str[i];
                continue;
            }
            dest.push_back(static_cast<char> (manapi::unicode::onehex2dec(static_cast<uint8_t>(str[i+1])) << 4 | manapi::unicode::onehex2dec(
                                     static_cast<uint8_t>(str[i+2]))));
            i = i + 2;
        }
    }
}

std::string manapi::encoding::decode_url(std::string_view str) {
    std::string dest;
    decode_url(dest, str);
    return std::move(dest);
}

bool manapi::encoding::url_allowed_symbol(char c) {
    return manapi__url_allowed_symbols[ static_cast<uint8_t> (c) ];
}
