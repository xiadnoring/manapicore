//#include <cstring>
//
//#include "ManapiString.hpp"
//#include "http/ManapiHttpResponse.hpp"
//#include "../include/http/ManapiHttp1Interface.hpp"
//#include "../include/http/ManapiHttp1.hpp"
//#include "../include/ManapiHttpInternal.hpp"
//#include "../include/ManapiUtils.hpp"
//
//#define HTTP_ALL_SWITCH_CASE(namecb__, cb__, ver__, name__) \
//case manapi::net::http::versions::ver__: \
//httpctx = static_cast<manapi::net::worker::wrk_http_ctx_global_t *> (global->data)->name__.get(); \
//return httpctx->cb__;
//
//#define HTTP_ALL_SWITCH_CASE2(namecb__, cb__, ver__, name__) \
//case manapi::net::http::versions::ver__: \
//httpctx = static_cast<manapi::net::worker::wrk_http_ctx_global_t *> (global->data)->name__.get(); \
//if (httpctx->namecb__) { return httpctx->cb__; } break;
//
//#define HTTP_ALL_SWITCH_DEFAULTCASE(name__, cb__) \
//default: assert(false && "unreachable code");
//
//#define HTTP_ALL_SWITCH(namecb, ...) \
//manapi::net::worker::wrk_interface_global_t * httpctx; \
//switch (conn->version) { \
//HTTP_ALL_SWITCH_CASE(namecb, namecb(__VA_ARGS__), HTTP_v0_9, http1) \
//HTTP_ALL_SWITCH_CASE(namecb, namecb(__VA_ARGS__), HTTP_v1_0, http1) \
//HTTP_ALL_SWITCH_CASE(namecb, namecb(__VA_ARGS__), HTTP_v1_1, http1) \
//HTTP_ALL_SWITCH_CASE(namecb, namecb(__VA_ARGS__), HTTP_v2, http2) \
//HTTP_ALL_SWITCH_CASE(namecb, namecb(__VA_ARGS__), HTTP_v3, http3) \
//HTTP_ALL_SWITCH_DEFAULTCASE(namecb, namecb(__VA_ARGS__)) } \
//httpctx = static_cast<manapi::net::worker::wrk_http_ctx_global_t *> (global->data)->http1.get(); \
//return httpctx->namecb(__VA_ARGS__);
//
//#define HTTP_ALL_SWITCH2(namecb, ...) \
//manapi::net::worker::wrk_interface_global_t * httpctx; \
//switch (conn->version) { \
//HTTP_ALL_SWITCH_CASE2(namecb, namecb(__VA_ARGS__), HTTP_v0_9, http1) \
//HTTP_ALL_SWITCH_CASE2(namecb, namecb(__VA_ARGS__), HTTP_v1_0, http1) \
//HTTP_ALL_SWITCH_CASE2(namecb, namecb(__VA_ARGS__), HTTP_v1_1, http1) \
//HTTP_ALL_SWITCH_CASE2(namecb, namecb(__VA_ARGS__), HTTP_v2, http2) \
//HTTP_ALL_SWITCH_CASE2(namecb, namecb(__VA_ARGS__), HTTP_v3, http3) } \
//return;
//
//enum http_v1_flags {
//    HTTP1_BODY_CHUNKED = 1
//};
//
//static int default_wrk_http_all_version (const manapi::net::worker::shared_conn & conn, manapi::net::worker::wrk_interface_global_t *global) MANAPICORE_NOEXCEPT {
//    auto httpctx = static_cast<manapi::net::worker::wrk_http_ctx_global_t *> (global->data);
//    if (httpctx->http1) return manapi::net::http::versions::HTTP_v1_1;
//    if (httpctx->http2) return manapi::net::http::versions::HTTP_v2;
//    if (httpctx->http3) return manapi::net::http::versions::HTTP_v3;
//    return 0;
//}
//
//static int default_wrk_http_all_init_stream (const manapi::net::worker::shared_conn & conn, const manapi::net::worker::shared_conn & stream, manapi::net::worker::wrk_interface_global_t *global, manapi::net::worker::base *w) MANAPICORE_NOEXCEPT {
//    if (!conn->version) conn->version = default_wrk_http_all_version(conn, global);
//    HTTP_ALL_SWITCH(init_stream_cb, conn, stream, httpctx, w);
//}
//
//static int default_wrk_http_all_init (const manapi::net::worker::shared_conn & conn, manapi::net::worker::wrk_interface_global_t *global, manapi::net::worker::base *w) MANAPICORE_NOEXCEPT {
//    if (!conn->version) conn->version = default_wrk_http_all_version(conn, global);
//    HTTP_ALL_SWITCH (init_cb, conn, httpctx, w);
//}
//
//static int default_wrk_http_all_cleanup (manapi::net::worker::connection * conn, manapi::net::worker::wrk_interface_global_t *global) MANAPICORE_NOEXCEPT {
//    if (!conn->version) return 0;
//    HTTP_ALL_SWITCH (cleanup_cb, conn, httpctx);
//}
//
//static void default_wrk_http_all_custom_read_cb(const manapi::net::worker::shared_conn &conn, manapi::slice_complex *sc, manapi::net::worker::wrk_interface_global_t *global) MANAPICORE_NOEXCEPT {
//    if (!conn->version) return;
//    HTTP_ALL_SWITCH (custom_read_cb, conn, sc, httpctx);
//}
//
//static void default_wrk_http_all_update_limit_rate_cb(const manapi::net::worker::shared_conn & conn, manapi::net::worker::wrk_interface_global_t *global) MANAPICORE_NOEXCEPT {
//    if (!conn->version) return;
//    HTTP_ALL_SWITCH (update_limit_rate_cb, conn, httpctx);
//}
//
//static int default_wrk_http_all_custom_cb (const manapi::net::worker::shared_conn &conn, manapi::net::worker::wrk_interface_global_t *global, void *data) MANAPICORE_NOEXCEPT {
//    HTTP_ALL_SWITCH(custom_cb, conn, global, data);
//}
//
//static int default_wrk_http_all_shutdown_conn(const manapi::net::worker::shared_conn &conn, manapi::net::worker::wrk_interface_global_t *global, bool force) MANAPICORE_NOEXCEPT {
//    if (!conn->version) return 1;
//    HTTP_ALL_SWITCH (shutdown_cb, conn, global, force);
//}
//
//
//static int default_wrk_http_all_cleanup_global_cb (manapi::net::worker::wrk_interface_global_t *data) MANAPICORE_NOEXCEPT {
//    auto wrk_global = static_cast<manapi::net::worker::wrk_http_ctx_global_t *>(data->data);
//
//    int rhs1=0,rhs2=0,rhs3=0;
//
//    if (wrk_global->http1)
//        rhs1=wrk_global->http1->cleanup_global_cb(wrk_global->http1.get());
//
//    if (wrk_global->http2)
//        rhs2=wrk_global->http2->cleanup_global_cb(wrk_global->http2.get());
//
//    if (wrk_global->http3)
//        rhs3=wrk_global->http3->cleanup_global_cb(wrk_global->http3.get());
//
//    delete wrk_global;
//    data->data = nullptr;
//
//    return std::max(abs(rhs1),
//        std::max(abs(rhs2), abs(rhs3)));
//}
//
//static uint64_t default_wrk_http_all_global_flags (const manapi::net::worker::shared_conn & conn, manapi::net::worker::wrk_interface_global_t *global) MANAPICORE_NOEXCEPT {
//    auto const data = static_cast<manapi::net::worker::wrk_http_ctx_global_t *> (global->data);
//    switch (conn->version) {
//        case manapi::net::http::versions::HTTP_v0_9:
//        case manapi::net::http::versions::HTTP_v1_0:
//        case manapi::net::http::versions::HTTP_v1_1:
//            return data->http1->flags;
//        case manapi::net::http::versions::HTTP_v2:
//            return data->http2->flags;
//        case manapi::net::http::versions::HTTP_v3:
//            return data->http3->flags;
//        default:
//            return 0;
//    }
//}
//
//static int default_wrk_http_all_global_alpn (manapi::net::worker::wrk_interface_global_t *global, char const *alpn, std::size_t alpn_size) MANAPICORE_NOEXCEPT {
//    std::string_view s {alpn, alpn_size};
//    if (s.starts_with("http/")) {
//        s = s.substr(sizeof ("http/") - 1);
//        if (s == "0.9")
//            return manapi::net::http::versions::HTTP_v0_9;
//        if (s == "1.0")
//            return manapi::net::http::versions::HTTP_v1_0;
//        if (s == "1.1")
//            return manapi::net::http::versions::HTTP_v1_1;
//    }
//    else {
//        if (s == "h2")
//            return manapi::net::http::versions::HTTP_v2;
//
//        if (s == "h3")
//            return manapi::net::http::versions::HTTP_v3;
//    }
//
//    return manapi::net::http::versions::HTTP_v1_1;
//}
//
//manapi::status manapi::net::worker::default_wrk_http_all_global_init(wrk_interface_global_t *global, worker::interface_worker *w) MANAPICORE_NOEXCEPT {
//    if (global->data)
//        return manapi::status_invalid_argument("global->data already exists");
//
//    global->data = new (std::nothrow) manapi::net::worker::wrk_http_ctx_global_t{};
//    if (!global->data)
//        return status_resource_exhausted();
//
//    global->alpn_cb = default_wrk_http_all_global_alpn;
//    global->flags_cb = default_wrk_http_all_global_flags;
//    global->init_stream_cb = default_wrk_http_all_init_stream;
//    global->init_cb = default_wrk_http_all_init;
//    global->cleanup_cb = default_wrk_http_all_cleanup;
//    global->update_limit_rate_cb = default_wrk_http_all_update_limit_rate_cb;
//    global->cleanup_global_cb = default_wrk_http_all_cleanup_global_cb;
//    global->custom_cb = default_wrk_http_all_custom_cb;
//    global->shutdown_cb = default_wrk_http_all_shutdown_conn;
//    global->custom_read_cb = default_wrk_http_all_custom_read_cb;
//
//    return manapi::status_ok();
//}
//
//manapi::status manapi::net::worker::default_wrk_http_all_global_add_version(wrk_interface_global_t *global, int version, std::unique_ptr<wrk_interface_global_t> http_t) MANAPICORE_NOEXCEPT {
//    if (!global)
//        return manapi::status_invalid_argument("global is null");
//
//
//    auto data = static_cast<worker::wrk_http_ctx_global_t *> (global->data);
//
//    switch(version) {
//        case http::versions::HTTP_v0_9:
//        case http::versions::HTTP_v1_0:
//        case http::versions::HTTP_v1_1: {
//            data->http1 = std::move(http_t);
//            break;
//        }
//        case http::versions::HTTP_v2: {
//            data->http2 = std::move(http_t);
//            break;
//        }
//        case http::versions::HTTP_v3: {
//            data->http3 = std::move(http_t);
//            break;
//        }
//        default:
//            return status_out_of_range("http version incorrect");
//    }
//
//    return status_ok();
//}
//
//static void default_wrk_http1_custom_read (const manapi::net::worker::shared_conn &conn, int flags, const char *buffer, std::size_t nsize, manapi::net::worker::ibuffpool_t *p, manapi::net::worker::wrk_interface_global_t *global, manapi::net::worker::base *w) MANAPICORE_NOEXCEPT {
//    auto wrk_ctx = static_cast<manapi::net::worker::wrk_http1_ctx_t *> (conn->wrk.data);
//
//    if (wrk_ctx->flgs & HTTP1_BODY_CHUNKED) {
//        auto rhs = manapi::net::http::http_v1_1_chunked_read(wrk_ctx->chunked_ctx.get(), conn, buffer, nsize);
//
//        switch (rhs) {
//            case manapi::net::http::EHTTP_V1_1_CHUNKED_OK: {
//                w->event_flags(conn, manapi::net::worker::base::CONN_RECV_END);
//                w->feed_event(conn, manapi::net::worker::base::CONN_RECV_END, nullptr);
//                break;
//            }
//            case manapi::net::http::EHTTP_V1_1_CHUNKED_ERR:
//                w->close_connection(conn, manapi::net::worker::CLOSE_CONN_ERR);
//            break;
//            case manapi::net::http::EHTTP_V1_1_CHUNKED_READ: break;
//            case manapi::net::http::EHTTP_V1_1_CHUNKED_WAIT: break;
//        }
//    }
//    else {
//        assert(false && "not implemented");
//    }
//}
//
//static int default_wrk_http1_cleanup (manapi::net::worker::connection* conn, manapi::net::worker::wrk_interface_global_t *global) MANAPICORE_NOEXCEPT {
//    auto wrk_data = static_cast<manapi::net::worker::wrk_http1_ctx_t *>(conn->wrk.data);
//    delete wrk_data;
//    conn->wrk.data = nullptr;
//    conn->wrk.flags = 0;
//    return manapi::ERR_OK;
//}
//
//static int default_wrk_http1_global_cleanup (manapi::net::worker::wrk_interface_global_t *data) MANAPICORE_NOEXCEPT {
//    auto wrk_global = static_cast<manapi::net::worker::wrk_http1_ctx_global_t *>(data->data);
//    delete wrk_global;
//    data->data = nullptr;
//    return manapi::ERR_OK;
//}
//
//static int default_wrk_http1(const manapi::net::worker::shared_conn &conn, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT {
//    auto wrk_data = static_cast<manapi::net::worker::wrk_http1_ctx_t *>(conn->wrk.data);
//    int status = manapi::net::http::BAD_REQUEST_400;
//
//    if (conn->wrk.flags & manapi::net::worker::WRK_INTERFACE_CONN_RETRY) {
//        status = manapi::net::http::TOO_MANY_REQUESTS_429;
//        goto send_error;
//    }
//
//    if (flags & manapi::ev::DISCONNECT)
//        goto err;
//
//    try {
//        if (flags & manapi::ev::READ) {
//            for (auto it = sc->base()->begin(); it != sc->base()->end(); ) {
//                auto buffer = static_cast<const char *>(it.buffer());
//                auto nsize = it.size();
//
//                switch (auto const state = manapi::net::http::http_v1_1_work(
//                    wrk_data->ctx.get(), &wrk_data->req, wrk_data->w->m_config.get(), &buffer, &nsize)) {
//                    case manapi::net::http::EHTTP_V1_1_PROTOCOL_PAYLOAD_TOO_LARGE:
//                        status = manapi::net::http::PAYLOAD_TOO_LARGE_413;
//                        goto send_error;
//
//                    case manapi::net::http::EHTTP_V1_1_PROTOCOL_OK: {
//                        status = manapi::net::http::OK_200;
//                        if (!wrk_data->w->m_config->contains_http_version(manapi::net::http::versions::http::HTTP_v1_1)) {
//                            status = manapi::net::http::UPGRADE_REQUIRED_426;
//                            goto send_error;
//                        }
//
//                        auto it_header = wrk_data->req.headers.find(manapi::net::http::H_EXPECT);
//                        if (it_header != wrk_data->req.headers.end()) {
//                            ssize_t const copy = sizeof ("HTTP/1.1 100 Continue\r\n\r\n") - 1;
//                            auto const rhs = wrk_data->w->sync_write_ex (conn, static_cast<const char *>("HTTP/1.1 100 Continue\r\n\r\n"),
//                                                               copy, true, std::numeric_limits<std::size_t>::max());
//                            if (copy != rhs) {
//                                goto err;
//                            }
//                        }
//
//                        std::vector<manapi::net::http::header_value_t> trailers_header{};
//
//                        it_header = wrk_data->req.headers.find(manapi::net::http::H_TRANSFER_ENCODING);
//                        if (it_header != wrk_data->req.headers.end()) {
//                            std::vector<manapi::net::http::header_value_t> values;
//                            {
//                                auto rhs = manapi::net::http::parse_header_value(it_header->second);;
//                                if (!rhs.ok()) goto send_error;
//                                values = rhs.unwrap();
//                            }
//                            for (const auto &v : values) {
//                                if (manapi::string::equals(v.value, "chunked", 0b10)) {
//                                    wrk_data->flgs |= HTTP1_BODY_CHUNKED;
//                                    conn->wrk.flags |= manapi::net::worker::WRK_INTERFACE_CUSTOM_READ;
//                                    wrk_data->chunked_ctx = std::make_unique<manapi::net::http::http_v1_1_chunked_t>();
//                                    wrk_data->chunked_ctx->top_sz = 0;
//
//                                    auto trailers_it = wrk_data->req.headers.find("trailer");
//                                    if (trailers_it != wrk_data->req.headers.end()) {
//                                        auto trailers_header_status = manapi::net::http::parse_header_value(trailers_it->second);
//                                        if (!trailers_header_status.ok()) {
//                                            manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s:%s failed due to %.*s", "http1", "chunked body",
//                                                             trailers_header_status.message().size(), trailers_header_status.message().data());
//                                            goto send_error;
//                                        }
//
//                                        trailers_header = trailers_header_status.unwrap();
//                                    }
//
//                                    continue;
//                                }
//
//                                goto send_error;
//                            }
//                        }
//
//                        it_header = wrk_data->req.headers.find(manapi::net::http::H_CONNECTION);
//                        if (it_header !=  wrk_data->req.headers.end()) {
//                            auto rhs = manapi::net::http::parse_header_value(it_header->second);
//                            if (!rhs.ok()) goto send_error;
//
//                            auto const values = rhs.unwrap();
//
//                            bool is_keep_alive = false;
//                            bool is_close = false;
//
//                            for (const auto &v : values) {
//                                if (manapi::string::equals(v.value, "keep-alive", 0b10)) {
//                                    is_keep_alive = true;
//                                } else if (manapi::string::equals(v.value, "close", 0b10)) {
//                                    is_close = true;
//                                } else {
//                                    goto send_error;
//                                }
//                            }
//
//                            if (is_keep_alive & is_close)
//                                goto send_error;
//
//                            if (is_keep_alive)
//                                conn->wrk.flags |= manapi::net::worker::WRK_INTERFACE_TCP_KEEP_ALIVE;
//                        }
//                        else {
//                            conn->wrk.flags |= manapi::net::worker::WRK_INTERFACE_TCP_KEEP_ALIVE;
//                        }
//
//                        auto cdata = std::make_unique<manapi::net::http::internal::handle_data_t>(conn,
//                                  wrk_data->w->shared_from_this(), &wrk_data->req, std::make_unique<manapi::net::http::internal::cont_callback_cb_t>(
//                            [conn] (bool ok)
//                                -> void {
//                                auto w = static_cast<manapi::net::worker::wrk_http1_ctx_t *>(conn->wrk.data)->w;
//                                w->close_connection (conn, ok ? manapi::net::worker::CLOSE_CONN_FINISHED : manapi::net::worker::CLOSE_CONN_ERR);
//                            }));
//
//                        wrk_data->w->event_unsub( conn );
//
//                        cdata->router = manapi::net::http::server::cast(wrk_data->w->m_site.get())->handler(&wrk_data->req);
//                        cdata->req_data->cdata = cdata.get();
//
//                        assert(cdata->router && cdata->router->handler);
//
//                        if (cdata->router->handler->trailers.size() < trailers_header.size()) {
//                            goto send_error;
//                        }
//
//                        for (auto &value : trailers_header) {
//                            for (auto c : value.value) c = static_cast<char> (std::tolower(c));
//
//                            if (cdata->router->handler->trailers.find(value.value) == cdata->router->handler->trailers.end()) {
//                                manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s:%s failed due to %s", "http1", "chunk body", "trailer not allowed");
//                                goto send_error;
//                            }
//                        }
//
//                        for (auto &value : trailers_header) {
//                            wrk_data->chunked_ctx->trailer_names.insert(std::move(value.value));
//                        }
//
//                        if (nsize) {
//                            sc->base()->shift_add( sc->base()->begin().size() - nsize ).unwrap();
//                            wrk_data->w->feed_event( conn, manapi::net::worker::base::CONN_READ|manapi::net::worker::base::CONN_TOP_READ, sc );
//                        }
//
//                        wrk_data->ctx.reset();
//
//                        wrk_data->w->waiting(conn, false);
//
//                        manapi::net::http::internal::handle_income_request(std::move(cdata), status);
//
//                        break;
//                    }
//                    case manapi::net::http::EHTTP_V1_1_PROTOCOL_UPGRADE: {
//                        wrk_data->w->event_unsub( conn );
//
//                        auto httpall = wrk_data->w->wrk_global();
//                        auto httpallctx = static_cast<manapi::net::worker::wrk_http_ctx_global_t *> (httpall->data);
//
//                        switch (wrk_data->ctx->http) {
//                            case manapi::net::http::versions::HTTP_v0_9: {
//                                goto send_error;
//                            }
//                            case manapi::net::http::versions::HTTP_v1_0: {
//                                goto send_error;
//                            }
//                            case manapi::net::http::versions::HTTP_v1_1: {
//                                goto send_error;
//                            }
//                            case manapi::net::http::versions::HTTP_v2: {
//                                if (httpallctx->http2) {
//                                    wrk_data->w->wrk_global()->cleanup_cb(conn.get(), wrk_data->w->wrk_global());
//                                    conn->version = manapi::net::http::versions::HTTP_v2;
//                                    httpallctx->http2->init_cb(conn, httpallctx->http2.get(), wrk_data->w);
//                                    manapi::slice_complex sc (manapi::slice{});
//                                    sc.get_slice()->push_back ( "PRI * HTTP/2.0", sizeof ("PRI * HTTP/2.0") - 1 ).unwrap();
//                                    wrk_data->w->feed_event(conn, manapi::net::worker::base::CONN_READ|manapi::net::worker::base::CONN_TOP_READ, &sc);
//                                    break;
//                                }
//                                goto send_error;
//                            }
//                            case manapi::net::http::versions::HTTP_v3: {
//                                /* no */
//                                goto send_error;
//                            }
//                            default: {
//                                goto send_error;
//                            }
//                        }
//
//                        if (nsize) {
//                            sc->base()->shift_add( sc->base()->begin().size() - nsize ).unwrap();
//                            wrk_data->w->feed_event(conn, manapi::net::worker::base::CONN_READ| manapi::net::worker::base::CONN_TOP_READ, sc);
//                        }
//
//                        break;
//                    }
//                    case manapi::net::http::EHTTP_V1_1_PROTOCOL_ERROR: {
//                        goto send_error;
//                    }
//                    case manapi::net::http::EHTTP_V1_1_PROTOCOL_WANT_READ: {
//                        /* skip */
//
//                        it++;
//
//                        sc->base()->shift_add( sc->base()->begin().size() ).unwrap();
//
//                        assert ( sc->base()->begin() == it );
//
//                        continue;
//                    }
//                    default: {
//                        assert(false && "An invalid state for http_v1_1_work()");
//                    }
//                }
//
//                break;
//            }
//        }
//
//        return manapi::ERR_OK;
//    }
//    catch (std::bad_alloc const &) {
//
//    }
//    catch (std::exception const &e) {
//        /* fatal error */
//        manapi_log_error("%s failed due to %s", "default_wrk_http1", e.what());
//    }
//
//
//    send_error: {
//        try {
//            auto msg = manapi::net::http::status_to_string(static_cast<uint16_t>(status));
//            if (msg.ok()) {
//                auto status_message = msg.unwrap();
//                auto const length = manapi::net::http::internal::generate_default_page_size (status, status_message);
//                manapi::slice sv;
//                auto httpv = manapi::net::http::config::stringify_http_version(conn->version);
//                char buff[128];
//
//                auto [ptr1, ec1] = std::to_chars ( buff, buff + sizeof (buff) - 1, static_cast<uint16_t>(status) );
//                if (ec1 != std::errc{}) {
//                    return manapi::ERR_UNKNOWN;
//                }
//
//                sv.push_back( "HTTP/", sizeof ("HTTP/") - 1 ).unwrap();
//                sv.push_back( httpv.data(), httpv.size() ).unwrap();
//                sv.push_back(" ", 1).unwrap();
//                sv.push_back(std::string_view ( buff, ptr1 )).unwrap();
//                sv.push_back(" ", 1).unwrap();
//                sv.push_back(status_message).unwrap();
//                sv.push_back( "\r\n", sizeof ("\r\n") - 1 ).unwrap();
//
//                auto [ptr2, ec2] = std::to_chars ( buff, buff + sizeof (buff) - 1, length );
//                if (ec2 != std::errc{}) {
//                    return manapi::ERR_UNKNOWN;
//                }
//
//                sv.push_back( "content-length: ", sizeof ("content-length: ") - 1 ).unwrap();
//                sv.push_back( std::string_view (buff, ptr2) ).unwrap();
//                sv.push_back( "\r\n", sizeof ("\r\n") - 1 ).unwrap();
//
//                sv.push_back( "connection: close\r\n\r\n", sizeof ("connection: close\r\n\r\n") - 1 ).unwrap();
//
//                auto const size = sv.size();
//                if (static_cast<ssize_t>(size) != wrk_data->w->sync_write_ex(conn, sv, true, std::numeric_limits<std::size_t>::max()))
//                    goto err;
//
//                wrk_data->w->close_connection (conn, manapi::net::worker::CLOSE_CONN_SHUTDOWN);
//            }
//        }
//        catch (std::bad_alloc const &) {
//            return manapi::ERR_RESOURCE_EXHAUSTED;
//        }
//        catch (std::exception const &e) {
//            manapi_log_error("%s failed due to %s", "default_wrk_http1", e.what());
//            return manapi::ERR_INTERNAL;
//        }
//    }
//    err: return manapi::ERR_ABORTED;
//}
//
//
//static int default_wrk_http1_init (const manapi::net::worker::shared_conn &conn, manapi::net::worker::wrk_interface_global_t *global, manapi::net::worker::base *w) MANAPICORE_NOEXCEPT {
//    try {
//        assert(!conn->wrk.data);
//
//        auto tp = std::make_unique<manapi::net::worker::wrk_http1_ctx_t>();
//        tp->ctx = std::make_unique<manapi::net::http::http_v1_1_t>();
//        tp->w = w;
//        conn->version = manapi::net::http::versions::HTTP_v1_1;
//        conn->wrk.data = tp.release();
//
//        w->event_on ( conn, [] ( const manapi::net::worker::shared_conn &conn, int flags, manapi::slice_complex *sc) -> void {
//
//            auto wrk_data = static_cast<manapi::net::worker::wrk_http1_ctx_t *>(conn->wrk.data);
//            if ( default_wrk_http1 ( conn, flags, sc ) ) {
//                wrk_data->w->close_connection( conn, manapi::net::worker::CLOSE_CONN_ERR );
//            }
//
//        }, manapi::net::worker::EVENT_ON_READ);
//    }
//    catch (std::bad_alloc const &) {
//        return manapi::ERR_RESOURCE_EXHAUSTED;
//    }
//    catch (std::exception const &e) {
//        manapi_log_error("%s failed due to %s", "default_wrk_http1_init", e.what());
//        return manapi::ERR_UNKNOWN;
//    }
//
//    return manapi::ERR_OK;
//}
//
//static int default_wrk_http1_send_response (const manapi::net::worker::shared_conn &conn, manapi::net::worker::wrk_interface_global_t *global, manapi::net::http::response* res, bool finish) {
//    auto wrk_data = static_cast < manapi::net::worker::wrk_http1_ctx_t * > (conn->wrk.data);
//    manapi::slice sv;
//    auto httpv = manapi::net::http::config::stringify_http_version(conn->version);
//    char buff[8];
//
//    auto [ptr, ec] = std::to_chars ( buff, buff + sizeof (buff) - 1, res->status_code() );
//    if (ec != std::errc{}) {
//        return manapi::ERR_UNKNOWN;
//    }
//
//    sv.push_back( "HTTP/", sizeof ("HTTP/") - 1 ).unwrap();
//    sv.push_back( httpv.data(), httpv.size() ).unwrap();
//    sv.push_back(" ", 1).unwrap();
//    sv.push_back(std::string_view ( buff, ptr )).unwrap();
//    sv.push_back(" ", 1).unwrap();
//    sv.push_back(res->status_message()).unwrap();
//    sv.push_back( "\r\n", sizeof ("\r\n") - 1 ).unwrap();
//
//    for (const auto &header: res->headers()) {
//        sv.push_back(header.first).unwrap();
//        sv.push_back(": ", sizeof (": ")).unwrap();
//        sv.push_back(header.second).unwrap();
//        sv.push_back( "\r\n", sizeof ("\r\n") - 1 ).unwrap();
//    }
//
//    sv.push_back( "\r\n", sizeof ("\r\n") - 1 ).unwrap();
//
//    auto const size = sv.size();
//    auto rhs = wrk_data->w->sync_write_ex (conn, sv, finish, std::numeric_limits<std::size_t>::max());
//    if (rhs != static_cast<ssize_t>(size)) return manapi::ERR_ABORTED;
//
//    return manapi::ERR_OK;
//}
//
//static int default_wrk_http1_custom_cb (const manapi::net::worker::shared_conn &conn, manapi::net::worker::wrk_interface_global_t *global, void *data) {
//
//    return default_wrk_http1_send_response ( conn, global,  );
//}
//
//manapi::status manapi::net::worker::default_wrk_http1_global_init (manapi::net::worker::wrk_interface_global_t *global, manapi::net::worker::interface_worker *w) MANAPICORE_NOEXCEPT {
//    if (global->data)
//        return manapi::status_invalid_argument("global->data already exists");
//
//    global->data = new (std::nothrow) manapi::net::worker::wrk_http1_ctx_global_t{};
//    if (!global->data) return manapi::status_resource_exhausted();
//
//    global->cleanup_cb = default_wrk_http1_cleanup;
//    global->cleanup_global_cb = default_wrk_http1_global_cleanup;
//    global->init_cb = default_wrk_http1_init;
//    global->update_limit_rate_cb = nullptr;
//    global->custom_cb = default_wrk_http1_custom_cb;
//
//    return manapi::status_ok();
//}