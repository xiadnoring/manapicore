#pragma once

#include <chrono>
#include <functional>
#include <set>
#include <map>

#include "./ManapiUtils.hpp"
#include "./ManapiAsync.hpp"
#include "./std/ManapiMutex.hpp"
#include "./ManapiTimerObject.hpp"
#include "./ManapiErrors.hpp"
#include "./ManapiEventStructures.hpp"

namespace manapi {
    class timer_pool {
    public:
        typedef std::pair <std::chrono::steady_clock::time_point, std::shared_ptr<timer::timer_data_t>> sorted_storage_key;
        struct sorted_tasks_compare_t {
            bool operator()(const sorted_storage_key &a, const sorted_storage_key &b) const MANAPICORE_NOEXCEPT;
        };

        typedef std::set <sorted_storage_key, sorted_tasks_compare_t> sorted_storage;

        struct data_t;

        timer_pool();

        /**
         * timer_pool based on libuv timer event
         */
        static manapi::status_or<std::shared_ptr<timer_pool>> create () MANAPICORE_NOEXCEPT;

        /* deconstructor */
        ~timer_pool();

        /* move */
        timer_pool (timer_pool &&n) MANAPICORE_NOEXCEPT;

        /* move */
        timer_pool &operator=(timer_pool &&n) MANAPICORE_NOEXCEPT;

        /* copy */
        timer_pool (const timer_pool &n);

        /* copy */
        timer_pool &operator=(const timer_pool &n);

        /**
         * Create a timeout event with synchronous callback
         * @param ms time in milliseconds
         * @param task synchronous callback
         * @return timer object on success, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_timer_sync (size_t ms, manapi::timer::sync_cb_t task) MANAPICORE_NOEXCEPT;


        /**
         * Create a timeout event with synchronous callback
         * @param ms time in milliseconds
         * @param task synchronous callback
         * @return timer object on success, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_timer_sync (size_t ms, timer_types type, manapi::timer::sync_cb_t task) MANAPICORE_NOEXCEPT;

        /**
         * Create a timeout event with asynchronous callback
         * @param ms time in milliseconds
         * @param task asynchronous callback
         * @return timer object on success, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_timer_async (size_t ms, manapi::timer::async_cb_t task) MANAPICORE_NOEXCEPT;


        /**
         * Create a timeout event with asynchronous callback
         * @param ms time in milliseconds
         * @param task asynchronous callback
         * @return timer object on success, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_timer_async (size_t ms, timer_types type, manapi::timer::async_cb_t task) MANAPICORE_NOEXCEPT;

        /**
         * FOR INTERNAL USE ONLY
         *
         * Remove a timer object from the pool using timer data
         * @param data timer data
         */
        void remove_timer (std::shared_ptr<timer::timer_data_t> data) MANAPICORE_NOEXCEPT;

        /**
         * Create an interval event with an asynchronous callback
         * @param ms period in milliseconds
         * @param task asychronous callback
         * @return timer object on success, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_interval_async (size_t ms, manapi::timer::async_cb_t task) MANAPICORE_NOEXCEPT;

        /**
         * Create an interval event with a synchronous callback
         * @param ms time in milliseconds
         * @param task sychronous callback
         * @return timer object on succes, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_interval_sync (size_t ms, manapi::timer::sync_cb_t task) MANAPICORE_NOEXCEPT;

        /**
         * Create an interval event with an asynchronous callback
         * @param ms period in milliseconds
         * @param task asychronous callback
         * @return timer object on success, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_interval_async (size_t ms, timer_types type, manapi::timer::async_cb_t task) MANAPICORE_NOEXCEPT;

        /**
         * Create an interval event with a synchronous callback
         * @param ms time in milliseconds
         * @param task sychronous callback
         * @return timer object on succes, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status_or<manapi::timer> append_interval_sync (size_t ms, timer_types type, manapi::timer::sync_cb_t task) MANAPICORE_NOEXCEPT;


        /**
         * set the timer again
         * @param data timer data
         * @return Ok on success, otherwise it returns InternalError, ResourceExhausted
         */
        manapi::status again_timer (std::shared_ptr<manapi::timer::timer_data_t> data) MANAPICORE_NOEXCEPT;

        manapi::ev::status start () MANAPICORE_NOEXCEPT;

        void stop () MANAPICORE_NOEXCEPT;

        void run_once () MANAPICORE_NOEXCEPT;

        void clear() MANAPICORE_NOEXCEPT;

        const std::shared_ptr<data_t> &data ();
    private:
        std::shared_ptr<data_t> m_data;
    };
}
