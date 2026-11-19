


#include "ManapiHttp.hpp"
#include "include/ManapiFetch2.hpp"
//#include "ext/pq/PostgreClient.hpp"
//#include "ext/pq/PostgrePool.hpp"

#include <cstring>

#include "crypto/ManapiAEAD.hpp"
#include "hash/ManapiSHA256.hpp"
#include "ManapiInitTools.hpp"
#include "ManapiMath.hpp"
#include "ManapiProcess.hpp"
#include "ManapiString.hpp"
#include "std/ManapiTimer.hpp"
#include "std/ManapiEasyCancelToken.hpp"
#include "ext/ManapiMustache.hpp"

#include "include/std/ManapiFunction.hpp"
//
#include "std/ManapiRef.hpp"

import test;

int main () {

    int stmax = 300;
    try { stmax = std::stoi(manapi::process::get_env("MANAPICORE_STMAX").unwrap()); }
    catch (...) {  }

    manapi::init_tools::max_coro_stack((size_t)stmax);

    int logtrace = 4;
    try { logtrace = std::stoi(manapi::process::get_env("MANAPICORE_LOGTRACE").unwrap()); }
    catch (...) {  }

    manapi::init_tools::log_trace_init((manapi::debug::trace_level)logtrace);

    size_t threads = 4;
    try { threads = (size_t)std::stoi(manapi::process::get_env("MANAPICORE_THREADS").unwrap()); }
    catch (...) {  }

    manapi::context::threadpoolfs(threads);
    manapi::context::gbs(manapi::context::blockedsignals());

    size_t loops = 0;
    try { loops = (size_t)std::stoi(manapi::process::get_env("MANAPICORE_LOOPS").unwrap()); }
    catch (...) {  }
    manapi::init_tools::log_name_enable("all", true);

    auto ctx = manapi::context::create(loops + 1).unwrap();

    std::atomic<int> a = 0;
    std::atomic<int> thrcnt = 0;

    ctx->run(loops, [] (const std::function<void()> &bind) -> void {

//            auto pool = manapi::ext::pq::pool::create().unwrap();
//            auto db = manapi::ext::pq::db::create().unwrap();
//
//            db->add_slave(pool).unwrap();
//
//            manapi::async::run([ pool] () mutable -> manapi::future<> {
//
//                manapi::unwrap(co_await pool->connect(3, "127.0.0.1", "5432", "storeuser", "1234", "manapistore"));
//
//                manapi_log_trace(manapi::debug::LOG_TRACE_HIGH, "http server has been started");
//            });

            int b = add (5, 5);
            std::cout << "verdict=" << b << "\n";

            bind();

    }).unwrap();

    manapi::clear_tools::clear_all();

    std::cout << "eventloop" << " " << ctx->eventloop().use_count() - 1 << "\n";
    std::cout << "ctx" << " " << ctx.use_count() - 1 << "\n";

    return 0;
}
