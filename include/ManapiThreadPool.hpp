#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <coroutine>

#include "./ManapiUtils.hpp"
#include "./ManapiTime.hpp"
#include "./std/ManapiFunction.hpp"
#include "./std/ManapiChain.hpp"
#include "./std/ManapiLogger.hpp"

namespace manapi {
    enum task_types {
        TASK_TYPE_FUNC = 0,
        TASK_TYPE_HANDLE
    };

    class mthread_pool {
    public:
        struct data_t;

        typedef std::vector<chain<std::move_only_function<void()>>> tasks_by_thread_t;

        mthread_pool(std::size_t thread_num);

        ~mthread_pool();

        void resize (std::size_t thread_num);

        void clear();

        void join() MANAPICORE_NOEXCEPT;

        void for_all_threads (std::move_only_function<void(tasks_by_thread_t *)> &&cb);

        void stop () MANAPICORE_NOEXCEPT;

        void notify_all () MANAPICORE_NOEXCEPT;

        void start ();

        void append_task (std::move_only_function<void()>&& cb);

        MANAPICORE_NODISCARD std::size_t size() const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::size_t tasks_size() const MANAPICORE_NOEXCEPT;

        void reserve_tasks(manapi::task_types type, std::size_t sz);

        void release_task (std::move_only_function<void()>&& cb) MANAPICORE_NOEXCEPT;

        void task ();
    private:
        std::unique_ptr<data_t> m_data;
    };

    class ethread_pool {
    public:
        struct data_t;

        ethread_pool ();

        ~ethread_pool ();

        MANAPICORE_NODISCARD std::size_t tasks_size() const MANAPICORE_NOEXCEPT;

        void reserve_tasks (manapi::task_types type, std::size_t sz);

        void release_tasks(manapi::task_types type, std::size_t sz) MANAPICORE_NOEXCEPT;

        bool try_task ();

        void set_notify () MANAPICORE_NOEXCEPT;

        void set_notify_cb (std::move_only_function<void()> ontask) MANAPICORE_NOEXCEPT;

        void stop () MANAPICORE_NOEXCEPT;

        void start ();

        void append_task (std::move_only_function<void()> &&cb);

        void append_task(const std::coroutine_handle<>& handle);

        void join () MANAPICORE_NOEXCEPT;
    private:
        std::unique_ptr<data_t> m_data;
    };
}
