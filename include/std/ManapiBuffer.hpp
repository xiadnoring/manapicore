#pragma once

#include <typeinfo>
#include <cstdint>

#include "../ManapiEventStructures.hpp"

#include "../ManapiErrors.hpp"

namespace manapi {
    class bytebuffer {
        struct slices_data {
            uv_buf_t *slices;
            int slices_cnt;
        };
        union buff_data {
            slices_data slices;
            uint8_t *src;
        };

    public:
        bytebuffer ();

        bool operator==(const std::nullptr_t &) const;

        bytebuffer (void *src, std::size_t size);

        bytebuffer (void *src, std::size_t size, std::size_t reserved);

        ~bytebuffer ();

        bytebuffer (bytebuffer &&n) MANAPICORE_NOEXCEPT;

        bytebuffer &operator=(bytebuffer &&n) MANAPICORE_NOEXCEPT;

        char *data ();

        char &operator[] (std::size_t i_);

        char &at (std::size_t i_);

        MANAPICORE_NODISCARD const char *data () const;

        operator bool () const;

        MANAPICORE_NODISCARD std::size_t size () const;

        MANAPICORE_NODISCARD std::size_t realsize () const;

        manapi::status realresize (std::size_t s) MANAPICORE_NOEXCEPT;

        manapi::status resize (std::size_t s) MANAPICORE_NOEXCEPT;

        manapi::status resize_max (std::size_t s) MANAPICORE_NOEXCEPT;

        void reset_shift () MANAPICORE_NOEXCEPT;

        void clear () MANAPICORE_NOEXCEPT;

        void *release ();

        MANAPICORE_NODISCARD std::size_t shift () const;

        void shift (std::size_t n);

        void shift_add (std::size_t n);

        MANAPICORE_NODISCARD bool empty () const;
    private:
        uint32_t m_shift;
        uint32_t m_s;
        uint32_t m_reserved;
        uint8_t *m_src;
    };
}