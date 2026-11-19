#pragma once

#include "./../ManapiUtils.hpp"

#include <functional>

namespace manapi {
    enum stoken_flags {
        STOKEN_FLAG_IMMEDIATELY_CALL = 1<<0
    };
    class stoken {
    public:
        struct data_t;

        stoken ( std::move_only_function<void()> fin_cb, int flags = 0 );

        stoken ();

        ~stoken ();

        stoken (stoken &&n) MANAPICORE_NOEXCEPT;

        stoken &operator= (stoken &&n) MANAPICORE_NOEXCEPT;

        stoken (const stoken &n) MANAPICORE_NOEXCEPT;

        stoken &operator= (const stoken &n) MANAPICORE_NOEXCEPT;

        stoken* next ( const manapi::stoken &token ) MANAPICORE_NOEXCEPT;

        int ref () MANAPICORE_NOEXCEPT;

        void unref () MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::size_t count () const MANAPICORE_NOEXCEPT;

        void reset () MANAPICORE_NOEXCEPT;

        void clear () MANAPICORE_NOEXCEPT;

        operator bool () const MANAPICORE_NOEXCEPT;
    private:
        data_t *m_data;
    };
}