#pragma once

#include <functional>

#include "./ManapiErrors.hpp"
#include "./ManapiUtils.hpp"
#include "./ManapiAsync.hpp"


namespace manapi {
    enum timer_types {
        TIMER_IMPORTANT = 0,
        TIMER_DEFAULT,
        TIMER_POOR
    };

    class cthread;

    class event_loop;

    class timer_pool;

    class timer {
    public:
        typedef std::move_only_function<manapi::future<>(const manapi::timer &data)> async_cb_t;

        typedef std::move_only_function<void(const manapi::timer &data)> sync_cb_t;

        struct timer_data_t;

        timer ();

        timer (std::nullptr_t);

        timer (std::shared_ptr<timer_data_t> data);

        static manapi::status_or<timer> create (bool interval,timer_types type,sync_cb_t sync_cb) MANAPICORE_NOEXCEPT;

        static manapi::status_or<timer> create (bool interval,timer_types type,async_cb_t async_cb) MANAPICORE_NOEXCEPT;

        timer (const timer &n);

        timer (timer &&n) MANAPICORE_NOEXCEPT;

        timer &operator=(timer &&n) MANAPICORE_NOEXCEPT;

        timer &operator=(const timer &n);

        timer &operator=(std::nullptr_t);

        ~timer ();

        explicit operator bool () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::size_t id () const MANAPICORE_NOEXCEPT;

        void clear () MANAPICORE_NOEXCEPT;

        void stop () MANAPICORE_NOEXCEPT;

        void callback_async (async_cb_t cb) MANAPICORE_NOEXCEPT;

        void callback_sync (sync_cb_t cb) MANAPICORE_NOEXCEPT;

        manapi::status again (std::size_t ms) MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::chrono::milliseconds interval () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::chrono::milliseconds remaning () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD bool is_async () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD bool is_sync () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD bool is_enabled () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD bool is_important () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::shared_ptr<timer_data_t> data () const MANAPICORE_NOEXCEPT;
    private:
        std::shared_ptr<timer_data_t> m_data;
    };
}
