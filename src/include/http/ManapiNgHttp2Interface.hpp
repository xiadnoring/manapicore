#pragma once

#include <memory>

#include "ManapiUtils.hpp"
#include "worker/ManapiHttp2Worker.hpp"
#include "../ManapiUtils.hpp"

#if MANAPICORE_NGHTTP2_DEPENDENCY && false

namespace manapi::net::worker {
    struct ng_wrk_http2_ctx_global_t;

    struct ng_wrk_http2_ctx_t;

    manapi::status ng_wrk_http2_global_init (manapi::net::worker::wrk_interface_global_t *global, manapi::net::worker::interface_worker *w) MANAPICORE_NOEXCEPT;
}

#endif