#include "ManapiEventLoop.hpp"
#include "ManapiTimerPool.hpp"
#include "ManapiInitTools.hpp"
#include "ManapiEventLoop.hpp"
#include "ManapiTimerPool.hpp"
#include "ManapiThreadPool.hpp"
#include "std/ManapiLogger.hpp"
#include "std/ManapiContext.hpp"

#include <thread>

#include "../include/ManapiUtils.hpp"
#include "../include/ManapiDefaultErrors.hpp"

std::unique_ptr<manapi::sigset_t> manapi::context::m_gbs = nullptr;

manapi::cthread::cthread(async::shared_eventloop eventloop, async::shared_mthreadpool taskpool, async::shared_timerpool timerpool, async::shared_logger logger) {
    this->m_eventloop = std::move(eventloop);
    this->m_taskpool = std::move(taskpool);
    this->m_timerpool = std::move(timerpool);
    this->m_logger = std::move(logger);
    this->m_memory_fabric = std::make_shared<object_pool>();
    this->m_flags = 0;
}

manapi::cthread::~cthread() = default;

manapi::ev::status manapi::cthread::start() {
    manapi::ev::status st;
    this->m_eventloop->taskpool()->start();
    if (!(st = this->m_timerpool->start())) return std::move(st);
    if (!(st = this->m_eventloop->run()) && st.code() != ERR_ABORTED) return std::move(st);
    return ev::status_ok();
}

void manapi::cthread::send_stop(manapi::stoken token) MANAPICORE_NOEXCEPT {
    this->m_eventloop->send_stop(std::move(token));
}

const std::shared_ptr<manapi::event_loop> & manapi::cthread::eventloop() MANAPICORE_NOEXCEPT {
    return this->m_eventloop;
}

const manapi::async::shared_mthreadpool& manapi::cthread::threadpool() {
    return this->m_taskpool;
}

const std::shared_ptr<manapi::timer_pool> & manapi::cthread::timerpool() MANAPICORE_NOEXCEPT {
    return this->m_timerpool;
}

const manapi::async::shared_etaskpool & manapi::cthread::etaskpool() MANAPICORE_NOEXCEPT {
    return this->m_eventloop->taskpool();
}

const std::shared_ptr<manapi::logger> & manapi::cthread::logger() MANAPICORE_NOEXCEPT {
    return this->m_logger;
}

const manapi::async::shared_memory_fabric & manapi::cthread::memory_fabric() MANAPICORE_NOEXCEPT {
    return this->m_memory_fabric;
}

manapi::future<> manapi::cthread::stop() {
    using promise = manapi::async::promise_sync<void>;
    co_await promise ( [this] (promise::resolve_t resolve) -> void {
        manapi::stoken token ([resolve = std::move(resolve)] ()
                                      -> void { resolve (); });
        this->send_stop( token );
    } );
}

manapi::context::context(async::shared_eventloop eventloop, async::shared_mthreadpool taskpool, async::shared_timerpool timerpool, async::shared_logger logger)
    : cthread(std::move(eventloop), std::move(taskpool), std::move(timerpool), std::move(logger))  {

}

manapi::status_or<manapi::async::shared_ctx> manapi::context::create(std::size_t threadnum) MANAPICORE_NOEXCEPT {
    try {
        auto logger = std::make_shared<manapi::logger>();
        auto taskpool = std::make_shared<manapi::mthread_pool>(threadnum);
        auto timerpool = manapi::timer_pool::create().unwrap();

        /* Main Event Loop */
        auto w = manapi::event_loop::create(0, taskpool, timerpool, logger).unwrap();

        auto ctx = std::shared_ptr<context> (new context (std::move(w), taskpool, timerpool, logger));

        manapi::context::current(ctx);

        return std::move(ctx);
    }
    catch (std::exception const &e) {
        manapi_log_error("%s due to %s", "ctx:create failed", e.what());
        return status_internal("ctx:create failed");
    }
}

manapi::status manapi::context::run(std::size_t loops, std::function<void(std::function<void()> bind)> callback) MANAPICORE_NOEXCEPT {
    auto ctx = this->shared_from_this();
    if (!ctx) return manapi::status_not_found("context:not found");

    manapi::ev::shared_async w_waits {nullptr};

    try {

        if (loops > ctx->m_taskpool->size()) {
            loops = ctx->m_taskpool->size();
            manapi_log_info("not enough threads for the additional event loops. Reducing to %zu", ctx->m_taskpool->size());
        }

        ctx->m_loops.resize(ctx->m_taskpool->size());

        for (std::size_t i = 0; i < ctx->m_taskpool->size(); ++i) {
            int flags = 0;
            if ( i >= loops ) flags |= manapi::event_loop::FLAG_WORKER_THREAD;

            auto timerpool = manapi::timer_pool::create().unwrap();
            auto eventloop = manapi::event_loop::create(flags, ctx->m_taskpool, timerpool, ctx->m_logger).unwrap();

            ctx->m_loops[i] = std::make_shared<cthread> (eventloop, ctx->m_taskpool, timerpool, ctx->m_logger);
        }

        manapi::context::current(ctx);

        manapi::init_tools::ssl_library_init();
        manapi::init_tools::ev_library_init();
        manapi::init_tools::curl_library_init();

        std::atomic<std::size_t> loops_active = ctx->m_taskpool->size();

        std::function<void()> bind_cb = [ctx] () -> void {
            auto st = ctx->start();
            if (!st) manapi_log_error (st.msg().data());
            ctx->m_timerpool->stop();
            ctx->m_eventloop->wait_all(false);
        };

        ctx->m_taskpool->start();

        for (std::size_t i = 0; i < ctx->m_taskpool->size(); ++i) {
            ctx->m_taskpool->for_all_threads([&] (mthread_pool::tasks_by_thread_t *v)
                -> void {
                (*v)[i].push_back([callback, thr = ctx->m_loops[i], w_waits, &loops_active] ()
                    -> void {
                    std::function<void()> bind_cb = [thr]() -> void {
                        auto st = thr->start();
                        if (!st) manapi_log_error (st.msg().data());
                        thr->timerpool()->stop();
                        thr->eventloop()->wait_all(false);
                    };

                    cthread::current(thr);

                    manapi::init_tools::ssl_library_init();
                    manapi::init_tools::ev_library_init();
                    manapi::init_tools::curl_library_init();
                    manapi::init_tools::grpc_library_init();

                    try {
                        if (thr->eventloop()->is_worker()) {
                            bind_cb();
                        } else {
                            callback(std::move(bind_cb));
                        }
                    }
                    catch (std::exception const &e) {
                        manapi_log_error(e.what());
                    }

                    thr->timerpool()->stop();
#if MANAPICORE_GRPC_DEPENDENCY
                    thr->eventloop()->wait_all(false);
                    clear_tools::grpc_thread_clear();
                    thr->eventloop()->wait_all(true);
#else
                    thr->eventloop()->wait_all(true);
#endif

                    thr->eventloop()->m_etaskpool->stop();
                    thr->eventloop()->m_etaskpool->join();

                    manapi::context::current(nullptr);

                    clear_tools::clear_thread_all();

                    loops_active.fetch_sub(1);

                    int rhs;
                    if (w_waits && (rhs = w_waits->send())) {
                        manapi_log_error ("%s failed due to %s", "ctx:fatal error context", ev::strerror(rhs));
                    }
                });
            });
        }

        auto st = manapi::async::current()->timerpool()->append_interval_sync(15 * 60 * 1000,
            [] (const manapi::timer &t) -> void {
            manapi::async::current()->memory_fabric()->clear();
        });

        if (!st) {
            manapi_log_error("%s", st.message().data());
        }

        if (loops_active.load()) {
            w_waits = ctx->m_eventloop->create_watcher_async(
                    [&loops_active] (const manapi::ev::shared_async &w) mutable -> void {
                        if (!loops_active.load()) { w->unref(); }
                    }).unwrap();

            w_waits->unref();
        }

        ctx->m_eventloop->setup_handle_interrupt();

        try {
            callback(std::move(bind_cb));
        }
        catch (std::exception const &e) {
            manapi_log_error(e.what());
        }

        if (loops_active.load()) {
            w_waits->ref();
        }

        // notify worker event loops
        for (auto &p : ctx->m_loops) {
            int rhs;
            auto &pev = p->eventloop();
            if (pev->m_interrupted_watcher && (rhs = pev->m_interrupted_watcher->send())) {
                manapi_log_error(manapi::ev::strerror(rhs));
            }
        }

        ctx->m_taskpool->stop();

        ctx->m_eventloop->stop_watcher(std::move(w_waits));

        ctx->m_timerpool->stop();

#if MANAPICORE_GRPC_DEPENDENCY
        ctx->eventloop()->wait_all(false);
        clear_tools::grpc_thread_clear();
        ctx->eventloop()->wait_all(true);
#else
        ctx->m_eventloop->wait_all(true);
#endif

        ctx->eventloop()->m_etaskpool->stop();
        ctx->eventloop()->m_etaskpool->join();

        ctx->m_taskpool->join();

    }
    catch (std::exception const &e) {
        manapi_log_error("%s due to %s", "ctx:run failed", e.what());
    }

    manapi::context::current(nullptr);

    return status_ok();
}

void manapi::context::run(std::function<void(std::function<void()> bind)> callback) {
    this->run(0, std::move(callback));
}

void manapi::context::threadpoolfs(std::size_t cnt) MANAPICORE_NOEXCEPT {
    auto s = std::to_string(cnt);
    if (auto rhs = uv_os_setenv("UV_THREADPOOL_SIZE", s.data())) {
        manapi_log_trace("%s failed due to %s", "ctx:set UV_THREADPOOL_SIZE", ev::strerror(rhs));
    }
}

std::unique_ptr<manapi::sigset_t> manapi::context::blockedsignals() MANAPICORE_NOEXCEPT {
    auto blocked_signals = std::make_unique<manapi::sigset_t>();
#if defined (__unix__) || defined(__APPLE__)
    sigemptyset(blocked_signals.get());
    sigaddset(blocked_signals.get(), SIGPIPE);
    pthread_sigmask(SIG_BLOCK, blocked_signals.get(), nullptr);
#endif
    return std::move(blocked_signals);
}

manapi::sigset_t *manapi::context::gbs () MANAPICORE_NOEXCEPT {
    return manapi::context::m_gbs.get();
}

void manapi::context::gbs(std::unique_ptr<manapi::sigset_t> n) MANAPICORE_NOEXCEPT {
    manapi::context::m_gbs = std::move(n);
}

const manapi::async::shared_cthread &manapi::async::current() MANAPICORE_NOEXCEPT {
    assert(manapi::async::context_exists() && "context doesn't exist in that thread");
    return manapi::async::try_current();
}

const std::shared_ptr<manapi::event_loop> & manapi::async::eventloop() MANAPICORE_NOEXCEPT {
    return manapi::async::current()->eventloop();
}

const std::shared_ptr<manapi::timer_pool> & manapi::async::timerpool() MANAPICORE_NOEXCEPT {
    return manapi::async::current()->timerpool();
}

const std::shared_ptr<manapi::ethread_pool> & manapi::async::etaskpool() MANAPICORE_NOEXCEPT {
    return manapi::async::current()->etaskpool();
}

const manapi::async::shared_mthreadpool & manapi::async::mtaskpool() MANAPICORE_NOEXCEPT {
    return manapi::async::current()->threadpool();
}

const std::shared_ptr<manapi::object_pool>& manapi::async::memory_fabric() MANAPICORE_NOEXCEPT {
    return manapi::async::current()->memory_fabric();
}

const std::shared_ptr<manapi::logger> & manapi::async::log() MANAPICORE_NOEXCEPT {
    return manapi::async::current()->logger();
}

manapi::context::~context() = default;

manapi::status_or<std::shared_ptr<manapi::context>> manapi::context::create() MANAPICORE_NOEXCEPT {
    return context::create(std::thread::hardware_concurrency());
}

void manapi::async::internal::run_prepare_error_(std::exception_ptr err) MANAPICORE_NOEXCEPT {
    try {
        int errnum = manapi::ERR_OK;
        char errmsg[256];
        std::size_t errmsg_size = sizeof (errmsg);

        manapi::extract_exception_ptr(std::move(err), &errnum, errmsg, &errmsg_size);
        manapi_log_error("ctx: unhandled exception: %d, %.*s", errnum, errmsg_size, errmsg);
    }
    catch (std::exception const &ex) {
        manapi_log_error("%s due to %s", "logger failed", ex.what());
    }
}


void manapi::async::internal::run_prepare_std_exception_(std::exception const &e) MANAPICORE_NOEXCEPT {
    manapi_log_error("ctx: unhandled exception: %d, %s", ERR_UNKNOWN, e.what());
}


void manapi::async::internal::run_prepare_manapi_exception_(manapi::exception &e) MANAPICORE_NOEXCEPT {
    manapi_log_error("ctx: unhandled exception: %d, %s", static_cast<int>(e.err_num()), e.what());
}


