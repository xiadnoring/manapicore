#pragma once

#include <memory>

#include "../ManapiUtils.hpp"
#include "../ManapiAsync.hpp"
#include "../ManapiMemoryPool.hpp"
#include "../std/ManapiStopToken.hpp"

namespace manapi {
    class logger;

    class context;

    class cthread;
}

namespace manapi::async {
    /* provides an async context */
    typedef std::shared_ptr<context> shared_ctx;
    /* provides a task pool */
    typedef std::shared_ptr<ethread_pool> shared_etaskpool;
    /* provides a task pool */
    typedef std::shared_ptr<mthread_pool> shared_mtaskpool;
    /* provides a logger */
    typedef std::shared_ptr<logger> shared_logger;
    /* provides a context by thread */
    typedef std::shared_ptr<cthread> shared_cthread;
    /* default after work callback */
    typedef std::move_only_function<void(std::exception_ptr err)> run_cb;
    /* after work callback with params */
    template<typename T>
    using run_cb_with_value = std::move_only_function<void(std::exception_ptr err, T *value)>;

    /**
     * Run an async task
     * @tparam T the type of the returned value
     * @param task the task
     * @param onfinish the after work callback with the result
     */
    template<typename T = void>
    requires(std::is_same_v<T, void>)
    void run(manapi::future<T> &&task, run_cb &&onfinish);

    /**
     * Run an async task
     *
     * @tparam T the type of the returned value
     * @param executor the task
     * @param onfinish the after work callback with the result
     */
    template<typename T = void>
    requires(std::is_same_v<T, void>)
    void run (auto && executor, run_cb&& onfinish );

    /**
     * Run an async task
     *
     * @tparam T the type of the returned value
     * @param task the task
     * @param onfinish the after work callback with the result
     */
    template<typename T>
    requires(!std::is_same_v<T, void>)
    void run(manapi::future<T> &&task, run_cb_with_value<T> &&onfinish);

    /**
     * Run an async task
     *
     * @tparam T the type of the returned value
     * @param executor the task
     * @param onfinish the after work callback with the result
     */
    template<typename T>
    requires(!std::is_same_v<T, void>)
    void run (auto && executor, run_cb_with_value<T> &&onfinish);

    /**
     * Run an async task
     * @tparam T the type of the returned value
     * @param task the task
     */
    template<typename T>
    void run(manapi::future<T> &&task);

    /**
     * Run an async task
     *
     * @param executor the task
     */
    void run (auto && executor);
}

namespace manapi {
#if defined (__unix__) || defined(__APPLE__)
    struct sigset_t : public ::sigset_t {};
#else
#   define MANAPIHTTP_SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))
    struct sigset_t {
        unsigned long int payload[MANAPIHTTP_SIGSET_NWORDS];
    };
#   undef MANAPIHTTP_SIGSET_NWORDS
#endif
    class object_pool;

    class event_loop;

    class timer_pool;

    class mthread_pool;

    class ethread_pool;
}

namespace manapi::async {
    /* provides a timer pool */
    typedef std::shared_ptr<timer_pool> shared_timerpool;
    /* provides a timer pool */
    typedef std::shared_ptr<mthread_pool> shared_mthreadpool;
    /* provides an event loop */
    typedef std::shared_ptr<event_loop> shared_eventloop;
    /* provides an event loop */
    typedef std::shared_ptr<object_pool> shared_memory_fabric;
}

#if defined (__unix__) || defined(__APPLE__)
#   include <pthread.h>
#endif

namespace manapi {
    /**
     * provides a context
     */
    class cthread {
    public:
        /**
         * initialize the context
         * @param eventloop the event loop
         * @param taskpool the task pool
         * @param timerpool the timer pool
         * @param logger the logger
         */
        cthread (async::shared_eventloop eventloop, async::shared_mthreadpool taskpool, async::shared_timerpool timerpool, async::shared_logger logger);

        /**
         * set as the default context in the thread
         * @param thr the context
         */
        static void current (std::shared_ptr<cthread> thr) MANAPIHTTP_NOEXCEPT;

        /**
         * deconstructor
         */
        virtual ~cthread();

        /**
         * get the event loop
         * @return the event loop
         */
        MANAPIHTTP_NODISCARD const async::shared_eventloop& eventloop() MANAPIHTTP_NOEXCEPT;

        MANAPIHTTP_NODISCARD const async::shared_mthreadpool &threadpool();

        /**
         * get the timer pool
         * @return the timer pool
         */
        MANAPIHTTP_NODISCARD const async::shared_timerpool &timerpool() MANAPIHTTP_NOEXCEPT;

        /**
         * get the event task pool
         * @return the event task pool
         */
        MANAPIHTTP_NODISCARD const async::shared_etaskpool &etaskpool () MANAPIHTTP_NOEXCEPT;

        /**
         * get the logger
         * @return the logger
         */
        MANAPIHTTP_NODISCARD const async::shared_logger &logger() MANAPIHTTP_NOEXCEPT;

        /**
         * get the memory fabric
         * @return the memory fabric
         */
        MANAPIHTTP_NODISCARD const async::shared_memory_fabric &memory_fabric () MANAPIHTTP_NOEXCEPT;

        /**
         * start working synchronously
         */
        virtual manapi::ev::status start ();

        /**
         * stop working
         * @return
         */
        virtual void send_stop ( manapi::stoken token ) MANAPIHTTP_NOEXCEPT;

        manapi::future<> stop ();
    protected:
        int m_flags;

        async::shared_eventloop m_eventloop;

        async::shared_timerpool m_timerpool;

        async::shared_mthreadpool m_taskpool;

        async::shared_logger m_logger;

        async::shared_memory_fabric m_memory_fabric;
    };

    typedef std::shared_ptr<cthread> shared_async_thread;

    class context : public cthread, public std::enable_shared_from_this<context> {
        /**
         * initialize the context
         * @param eventloop the event loop
         * @param taskpool the task pool
         * @param timerpool the timer pool
         * @param logger the logger
         */
        context (async::shared_eventloop eventloop, async::shared_mthreadpool taskpool, async::shared_timerpool timerpool, async::shared_logger logger);
    public:

        /**
         * deconstructor
         */
        ~context() override;

        /**
         * create a context and return it
         * @return
         */
        static manapi::status_or<std::shared_ptr<context>> create () MANAPIHTTP_NOEXCEPT;

        /**
         * create a context and return it
         * @param threadnum the additional threads
         * @return
         */
        static manapi::status_or<std::shared_ptr<context>> create (std::size_t threadnum) MANAPIHTTP_NOEXCEPT;

        /**
         * run the callback in all contexts
         * @param loops the count of copies of the context
         * @param callback the callback
         */
        manapi::status run (std::size_t loops, std::function<void(std::function<void()> bind)> callback) MANAPIHTTP_NOEXCEPT;

        /**
         * run the callback in the context
         * @param callback the callback
         */
        void run (std::function<void(std::function<void()> bind)> callback);

        /**
         * set the thread pool size for filesystem operations
         * @param cnt
         */
        static void threadpoolfs (std::size_t cnt = 4) MANAPIHTTP_NOEXCEPT;

        /**
         * create a blocked signals context
         * @return the blocked signals context
         */
        static std::unique_ptr<manapi::sigset_t> blockedsignals () MANAPIHTTP_NOEXCEPT;

        static manapi::sigset_t *gbs () MANAPIHTTP_NOEXCEPT;

        static void gbs (std::unique_ptr<manapi::sigset_t> n) MANAPIHTTP_NOEXCEPT;
    private:
        /**
         * global blocked signals storage
         */
        static std::unique_ptr<manapi::sigset_t> m_gbs;
        /* copies */
        std::vector<async::shared_cthread> m_loops;
    };

    template<typename T = void>
    struct async_task_t {
        manapi::future<T> task;
        char flags;
    };

    template<typename T>
    requires(!std::is_same_v<T, void>)
    manapi::future<T> blank_future(T &&n) { co_return std::forward<decltype(n)>(n); }

    template<typename T>
    requires(!std::is_same_v<T, void>)
    manapi::future<T> blank_future(const T &n) { co_return std::forward<decltype(n)>(n); }

    template<typename T>
    requires(std::is_same_v<T, void>)
    manapi::future<T> blank_future () { co_return; }
}


namespace manapi::async::internal {
    enum async_task_flags {
        ASYNC_TASK_FLAG_EXECUTED = 1
    };

    void run_prepare_error_ (std::exception_ptr err) MANAPIHTTP_NOEXCEPT;

    void run_prepare_std_exception_ (std::exception const &e) MANAPIHTTP_NOEXCEPT;

    void run_prepare_manapi_exception_ (manapi::exception &e) MANAPIHTTP_NOEXCEPT;

    /**
     * FOR INTERNAL USE ONLY
     * @param task_data the task data
     * @param onfinish the callback
     */
    inline void run_prepare_(std::unique_ptr<manapi::async_task_t<void>> &&task_data, run_cb &&onfinish) {

        task_data->task.onfinish([task = task_data.get(), onfinish = std::forward<decltype(onfinish)>(onfinish)] (std::exception_ptr err) mutable -> void {
            try { onfinish(std::move(err)); }
            catch (manapi::exception &e) { run_prepare_manapi_exception_(e); }
            catch (std::exception const &e) { run_prepare_std_exception_(e); }

            if (task->flags & ASYNC_TASK_FLAG_EXECUTED) { async::coro_finish(task->task.release()); delete task; }
            else { task->flags |= ASYNC_TASK_FLAG_EXECUTED; }
        });

        task_data->task();
        auto const rhs = task_data->flags & ASYNC_TASK_FLAG_EXECUTED;
        task_data->flags |= ASYNC_TASK_FLAG_EXECUTED;
        if (!rhs) task_data.release();
    }

    /**
     * FOR INTERNAL USE ONLY
     * @tparam T the type of the returned value
     * @param task_data the task data
     * @param onfinish the callback
     */
    template<typename T>
    void run_prepare_(std::unique_ptr<manapi::async_task_t<T>> &&task_data, run_cb_with_value<T> &&onfinish) {

        task_data->task.onfinish([task = task_data.get(), onfinish = std::forward<decltype(onfinish)>(onfinish)] (std::exception_ptr err, T *value) mutable -> void {
            try { onfinish(std::move(err), value); }
            catch (manapi::exception &e) { internal::run_prepare_manapi_exception_(e); }
            catch (std::exception const &e) { internal::run_prepare_std_exception_(e); }
            if (task->flags & ASYNC_TASK_FLAG_EXECUTED) { async::coro_finish(task->task.release()); delete task; }
            else { task->flags |= ASYNC_TASK_FLAG_EXECUTED; }
        });


        task_data->task();
        auto const rhs = task_data->flags & ASYNC_TASK_FLAG_EXECUTED;
        task_data->flags |= ASYNC_TASK_FLAG_EXECUTED;
        if (!rhs) task_data.release();
    }

    /**
     * FOR INTERNAL USE ONLY
     * @param task_data the task data
     */
    inline void run_prepare_(std::unique_ptr<manapi::async_task_t<void>> &&task_data) {

        task_data->task.onfinish([task = task_data.get()] (std::exception_ptr err) mutable -> void {
            if (err)
                run_prepare_error_(std::move(err));

            if (task->flags & ASYNC_TASK_FLAG_EXECUTED) { async::coro_finish(task->task.release()); delete task; }
            else { task->flags |= ASYNC_TASK_FLAG_EXECUTED; }
        });

        task_data->task();
        auto const rhs = task_data->flags & ASYNC_TASK_FLAG_EXECUTED;
        task_data->flags |= ASYNC_TASK_FLAG_EXECUTED;
        if (!rhs) task_data.release();
    }

    /**
     * FOR INTERNAL USE ONLY
     * @tparam T the type of the returned value
     * @param task_data the task data
     */
    template<typename T>
    void run_prepare_(std::unique_ptr<manapi::async_task_t<T>> &&task_data) {

        task_data->task.onfinish([task = task_data.get()] (std::exception_ptr err, T *) mutable -> void {
            if (err) internal::run_prepare_error_(std::move(err));
            if (task->flags & ASYNC_TASK_FLAG_EXECUTED) { async::coro_finish(task->task.release()); delete task; }
            else { task->flags |= ASYNC_TASK_FLAG_EXECUTED; }
        });


        task_data->task();
        auto const rhs = task_data->flags & ASYNC_TASK_FLAG_EXECUTED;
        task_data->flags |= ASYNC_TASK_FLAG_EXECUTED;
        if (!rhs) task_data.release();
    }
}

namespace manapi::async {
    /**
     * Safe lambda call
     * @tparam T the type of the returned value
     * @param executer the callback
     * @return the result
     */
    template<typename T>
    std::invoke_result_t <T> invoke (T executer) {
        co_return co_await executer();
    }

    /**
     * Safe lambda call
     * in the asynchronously context without memory leak
     * @tparam T the type of the returned value
     * @tparam Args types of args
     * @param executer the callback
     * @param args callback args
     * @return the result
     */
    template<typename T, typename ...Args>
    std::invoke_result_t <T, Args...> invoke (T executer, Args... args) {
        co_return co_await executer(std::move(args)...);
    }

    template<typename T>
    requires(std::is_same_v<T, void>)
    void run (auto && executor,  run_cb &&onfinish ) {
        async::run<T> (manapi::async::invoke(std::forward<decltype(executor)>(executor)), std::forward<decltype(onfinish)>(onfinish));
    }

    template<typename T>
    requires(!std::is_same_v<T, void>)
    void run (auto &&executor, run_cb_with_value<T> &&onfinish) {
        async::run<T> (manapi::async::invoke(std::forward<decltype(executor)>(executor)), std::forward<decltype(onfinish)>(onfinish));
    }

    template<typename T>
    requires(!std::is_same_v<T, void>)
    void run(manapi::future<T> &&task, run_cb_with_value<T> &&onfinish) {
        async_task_t<T>*  ptr = new async_task_t<T>(future<T>{nullptr});
        std::unique_ptr<async_task_t<T>> task_data(ptr);
        task_data->task = manapi::future<T>{task.release()};
        internal::run_prepare_<T>(std::move(task_data), std::forward<decltype(onfinish)>(onfinish));
    }

    template<typename T>
    requires(std::is_same_v<T, void>)
    void run(manapi::future<T> &&task, run_cb &&onfinish) {
        async_task_t<T>* ptr = new async_task_t<T>(future<T>{nullptr});
        std::unique_ptr<async_task_t<T>> task_data(ptr);
        task_data->task = manapi::future<T>{task.release()};
        internal::run_prepare_ (std::move(task_data), std::forward<decltype(onfinish)>(onfinish));
    }

    void run (auto && executor ) {
        async::run (manapi::async::invoke(std::forward<decltype(executor)>(executor)));
    }

    template<typename T>
    void run(manapi::future<T> &&task) {
        async_task_t<T>* ptr = new async_task_t<T>(future<T>{nullptr});
        std::unique_ptr<async_task_t<T>> task_data(ptr);
        task_data->task = manapi::future<T>{task.release()};
        internal::run_prepare_ (std::move(task_data));
    }
}
