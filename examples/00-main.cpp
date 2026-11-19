#include <iostream>
#include <format>

#include <manapicore/ManapiHttp.hpp>
#include <manapicore/fs/ManapiFilesystem.hpp>
#include <manapicore/ManapiInitTools.hpp>

int main () {
    manapi::init_tools::log_name_enable("manapicore", true);
    manapi::init_tools::log_name_enable("manapicore::fs", true);
    manapi::init_tools::log_trace_init (manapi::debug::LOG_TRACE_LOW);

    manapi::context::threadpoolfs(4);

    manapi::context::gbs (manapi::context::blockedsignals());

    auto ctx = manapi::context::create(0).unwrap();

    ctx->run(0, [] (std::function<void()> bind) mutable -> void {

        bind();
    }).unwrap();

    return 0;
}