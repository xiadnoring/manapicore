//#include <iostream>
//#include <csignal>
//#include <utility>
//#include <vector>
//#include <memory.h>
//#include <filesystem>
//#include <chrono>
//#include <thread>
//#include <unordered_map>
//#include <fcntl.h>
//#include <set>
//
//#include "ManapiParams.hpp"
//#include "ManapiTimerPool.hpp"
//#include "ManapiEventLoop.hpp"
//#include "ManapiInitTools.hpp"
//#include "std/ManapiSocket.hpp"
//#include "worker/ManapiTlsOverTcp.hpp"
//
//#include "worker/ManapiBaseUtils.hpp"
//#include "../include/ManapiUtils.hpp"
//
//
//struct ssl_bio_deleter_t;
//
//enum conn_tls_flags  {
//    CONN_TLS_SHUTDOWN = manapi::net::worker::base::CONN_MAX_CODE << 1,
//    CONN_TLS_EARLY_DATA = CONN_TLS_SHUTDOWN << 1,
//    CONN_TLS_EARLY_FINISHED = CONN_TLS_EARLY_DATA<<1
//};
//
//manapi::net::worker::TLS::TLS() : TCP() {
//    this->m_ssl_session_ctx_id = 1;
//    this->m_ctx = nullptr;
//}
//
//manapi::net::worker::TLS::~TLS() = default;
//
//manapi::future<manapi::status> manapi::net::worker::TLS::init(std::size_t deep) {
//    return TCP::init(deep + 1);
//}
//
//manapi::net::worker::shared_conn manapi::net::worker::TLS::accept(const ev::shared_tcp &w) MANAPICORE_NOEXCEPT {
//    auto connection = TCP::accept(w, connection_init_cb, this);
//
//    if (!connection) return nullptr;
//
//    auto data = connection->as<tls_connection_t>();
//    this->try_read_start(connection, data);
//
//    return std::move(connection);
//}
//
//void manapi::net::worker::TLS::close_connection(shared_conn conn, int flags) MANAPICORE_NOEXCEPT {
//    if (!conn)
//        return;
//
//    auto const connection = conn->as<tls_connection_t>();
//    bool keep_alive_disabled;
//
//    if (connection->flags & CONN_REMOVED)
//        return;
//
//    manapi_log_trace(debug::LOG_TRACE_LOW, "TLS:close_connection() %p flags=%d", conn.get(), flags);
//
//    if ((connection->flags & CONN_TLS_SHUTDOWN)) {
//        if ((flags & (CLOSE_CONN_EOS)) || ((flags & CLOSE_CONN_EOR)))
//            goto eof;
//
//        return;
//    }
//
//    /* Draining timeout is enabled, thus, connection is draining */
//    if (flags & CLOSE_CONN_EOR || flags & CLOSE_CONN_EOS) {
//        conn->wrk.flags |= WRK_INTERFACE_IS_DRAINING;
//    }
//    else if (connection->flags & CONN_WANT_CLOSE) {
//        if (!connection->ev_cb.empty() && !(manapi::net::worker::TLS::notify_cb(connection->ev_cb.back().first, conn, CONN_WANT_CLOSE, nullptr))) {
//            if (connection->flags & CONN_WANT_CLOSE)
//                return;
//        }
//    }
//
//    if (connection->flags & CONN_WANT_CLOSE)
//        connection->flags ^= CONN_WANT_CLOSE;
//
//    if ((flags & CLOSE_CONN_EOR)) {
//        connection->flags |= CONN_RECV_END;
//
//        if (connection->flags == CLOSE_CONN_EOR) {
//            if (!connection->t && !(connection->flags & CONN_READ)) {
//                return;
//            }
//        }
//    }
//
//    this->waiting(conn, true);
//
//    keep_alive_disabled = (!this->m_config->keep_alive || !(conn->wrk.flags & WRK_INTERFACE_TCP_KEEP_ALIVE));
//    if (!this->m_config->force_conn_shutdown && ((flags & (CLOSE_CONN_SHUTDOWN|CLOSE_CONN_ERR|CLOSE_CONN_EOR)) || (!flags && keep_alive_disabled))) {
//        connection->flags |= CONN_TLS_SHUTDOWN;
//
//        if (conn->wrk.flags & WRK_INTERFACE_TCP_KEEP_ALIVE)
//            conn->wrk.flags ^= WRK_INTERFACE_TCP_KEEP_ALIVE;
//
//        prepared::event_callback_clear( this, conn, connection );
//
//        this->waiting(conn, true);
//
//        this->shutdown_async(conn);
//
//        return;
//    }
//
//eof:
//    if (flags & (CLOSE_CONN_EOR|CLOSE_CONN_EOS)) {
//        connection->flags |= CONN_TLS_SHUTDOWN;
//
//        this->shutdown_async(conn);
//
//        manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "TLS:conn %p was destroyed "
//                "CLOSE_CONN_EOR=%d CLOSE_CONN_EOS=%d", conn.get(),
//            CLOSE_CONN_EOR & flags, CLOSE_CONN_EOS & flags);
//        this->ssl_set_shutdown(connection->ssl,
//            this->m_ssl_recv_shutdown|this->m_ssl_send_shutdown);
//        /* connection is destroyed */
//        flags |= CLOSE_CONN_SHUTDOWN;
//    }
//
//    TCP::close_connection(conn, flags);
//
//    if (connection->flags & CONN_REMOVED) {
//        connection->flags |= CONN_TLS_SHUTDOWN;
//    }
//}
//
//ssize_t manapi::net::worker::TLS::sync_write_ex(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool flush, std::size_t maxsize) MANAPICORE_NOEXCEPT {
//    auto connection = conn->as<tls_connection_t>();
//
//    if (connection->flags & (ev::DISCONNECT)) return -1;
//
//    std::size_t total = 0;
//    std::size_t current = 0;
//
//    while (nbuff) {
//        if (connection->top->send.size() + connection->top->pending_send > maxsize) break;
//
//        int rhs;
//
//        if (current == static_cast<std::size_t> (buff->len)) {
//            total += current;
//            connection->transfered += current;
//
//            nbuff--;
//            buff++;
//
//            current = 0;
//
//            continue;
//        }
//
//        if (connection->flags & CONN_TLS_EARLY_DATA) {
//            std::size_t writebytes;
//            auto res = this->ssl_write_early_data(connection->ssl, buff->base + current, static_cast<std::size_t>(buff->len) - current, &writebytes);
//            if (!res) {
//                rhs = 0;
//            } else {
//                rhs = static_cast<int>(writebytes);
//            }
//        } else {
//            rhs = this->ssl_write(connection->ssl, buff->base + current, static_cast<int>(static_cast<std::size_t>(buff->len) - current));
//        }
//
//        if (rhs > 0) {
//            current += static_cast<std::size_t>(rhs);
//        } else {
//            rhs = this->ssl_get_error(connection->ssl, rhs);
//
//            if (rhs) {
//                if (rhs == this->m_ssl_error_want_read || rhs == this->m_ssl_error_want_write) {
//
//                    if (auto zrhs = this->ssl_bio_flush_write(conn, connection, maxsize, false)) {
//                        return CONN_IO_ERROR;
//                    }
//
//                    if (rhs == this->m_ssl_error_want_read) {
//                        break;
//                    }
//
//                    continue;
//                }
//
//                return CONN_IO_ERROR;
//            }
//
//            if (rhs == this->m_ssl_error_zero_return) {
//                this->close_connection(conn, CLOSE_CONN_SHUTDOWN);
//                return CONN_IO_ERROR;
//            }
//
//            if (rhs == this->m_ssl_error_ssl) {
//                this->close_connection(conn, CLOSE_CONN_ERR);
//                return CONN_IO_ERROR;
//            }
//        }
//    }
//
//    total += current;
//    connection->transfered += current;
//
//    return static_cast<ssize_t>(total);
//}
//
//ssize_t manapi::net::worker::TLS::sync_write(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) MANAPICORE_NOEXCEPT {
//    return prepared::sync_write(this, conn, buff, nbuff, finish);
//}
//
//int manapi::net::worker::TLS::event_flags(const shared_conn & conn, int flags) MANAPICORE_NOEXCEPT {
//    auto const data = conn->as<tls_connection_t>();
//    MANAPICORE_WORKER_EVENT_LOOP(data) {
//        if (data->flags & ev::DISCONNECT) {
//            if (!data->ev_cb.empty()) {
//                manapi::net::worker::TLS::notify_cb(data->ev_cb.back().first, conn, CONN_CLOSED, nullptr);
//            }
//        } else {
//            if (status & ev::READ) {
//                this->flush_read (conn, data);
//            } else {
//                this->read_stop(conn, data);
//            }
//        }
//
//        MANAPICORE_WORKER_EVENT_BREAK(data)
//    }
//
//    return prev;
//}
//
////void manapi::net::worker::TLS::update_limit_rate_connection(const shared_conn &sconn) MANAPICORE_NOEXCEPT {
////    auto const data = sconn->as<tcp_connection_t>();
////
////    if (data->transfered >= this->m_config->speed_limit_rate) {
////        data->transfered = 0;
////
////        this->read_start(sconn, data);
////
////        if (!(data->flags & ev::DISCONNECT) && (data->flags & ev::WRITE)) {
////            if (this->notify_cb(data->ev_cb.back().first, sconn, ev::WRITE, nullptr)) {
////                this->close_connection(sconn, CLOSE_CONN_EOS);
////                return;
////            }
////        }
////    }
////    else {
////        data->transfered_k += data->transfered;
////
////        if (--data->speed_min_delay <= 0) {
////            if (data->flags & (base::CONN_IO_WAITING)
////                && (data->transfered_k < this->m_config->speed_check_bytes)) {
////                if (data->flags & CONN_WAS_SHUTDOWN) {
////                    data->speed_min_delay = this->m_config->speed_check_delay;
////                    this->close_connection(sconn, CLOSE_CONN_EOS);
////                } else {
////                    data->speed_min_delay = this->m_config->max_shutdown_time;
////                    this->close_connection(sconn, CLOSE_CONN_SHUTDOWN);
////                    data->flags |= CONN_WAS_SHUTDOWN;
////                }
////            } else {
////                data->speed_min_delay = this->m_config->speed_check_delay;
////            }
////
////            data->transfered_k = 0;
////        }
////
////        data->transfered = 0;
////    }
////
////    if (sconn->wrk.flags & WRK_INTERFACE_CUSTOM_RATE_LIMIT) {
////        assert(!!this->m_global.update_limit_rate_cb);
////        this->m_global.update_limit_rate_cb(sconn, &this->m_global);
////    }
////}
//
//void manapi::net::worker::TLS::connection_interface_eraser(worker::connection *ptr) MANAPICORE_NOEXCEPT {
//    auto connection = std::unique_ptr<tls_connection_t> (ptr->as<tls_connection_t>());
//    auto w = (dynamic_cast<TLS*>(connection->worker));
//
//    prepared::timer_clear(std::move(connection->accept_timer));
//
//    if (connection->watcher) {
//        if (connection->watcher->is_active()) {
//            manapi_log_trace(debug::LOG_TRACE_LOW, "TCP:%p read_stop()", connection.get());
//            connection->watcher->read_stop();
//        }
//        manapi::async::current()->eventloop()->stop_watcher(std::move(connection->watcher));
//    }
//
//    if (connection->ssl) {
//        auto ssl = std::exchange(connection->ssl, nullptr);
//        manapi_log_trace(debug::LOG_TRACE_MEDIUM, "TLS:Free SSL %p conn", connection.get());
//        if (w)
//            w->ssl_free(ssl);
//    }
//
//    auto const wrk = dynamic_cast<TLS*> (connection->worker);
//    assert (wrk) ;
//    if (wrk->m_global.cleanup_cb(ptr, &wrk->m_global))
//        manapi_log_trace("tcp:this->m_global.cleanup_cb failed");
//
//    wrk->m_worker_data->count.fetch_sub(1);
//    wrk->m_token.unref();
//}
//
//void manapi::net::worker::TLS::shutdown_async(shared_conn conn) {
//    auto s = conn->as<tls_connection_t>();
//
//    this->try_read_start(conn, s);
//
//    if (!this->ssl_is_init_fininshed(s->ssl)) {
//        this->manapi_do_handshake(conn, s);
//        return;
//    }
//
//    prepared::timer_clear(std::move(s->accept_timer));
//    auto accept_timer_res = manapi::async::current()->timerpool()->append_timer_sync(this->m_config->tls_shutdown_timeout, [conn = conn.get()] (manapi::timer t)
//        -> void { auto w = conn->as<tcp_connection_t>(); w->worker->close_connection(conn, CLOSE_CONN_ERR); });
//    if (accept_timer_res)
//        s->accept_timer = accept_timer_res.unwrap();
//    else {
//        this->close_connection(conn, CLOSE_CONN_ERR);
//        return;
//    }
//
//    bool flg = false;
//    while (true) {
//        auto rhs = this->ssl_shutdown(s->ssl);
//        manapi_log_trace(debug::LOG_TRACE_LOW, "TLS:Shutdown %p = %d", conn.get(), rhs);
//
//
//        if (!rhs) {
//            this->try_read_start(conn, s);
//            goto write;
//        }
//
//        if (rhs==this->m_ssl_shutdown_success) {
//            TCP::close_connection(conn, CLOSE_CONN_FINISHED);
//            return;
//        }
//
//        rhs = this->ssl_get_error(s->ssl, rhs);
//
//        if (rhs == this->m_ssl_error_syscall) {
//            auto const prev = std::exchange(flg, true);
//            if (prev) goto err;
//            continue;
//        }
//
//        if (rhs == this->m_ssl_error_want_read || rhs == this->m_ssl_error_none) {
//            goto write;
//        }
//
//        if (rhs == this->m_ssl_error_want_write) {
//            if (this->ssl_bio_flush_write(conn, s, this->m_config->max_buffer_size, false)) {
//                goto err;
//            }
//
//            if (!s->top->pending_send && s->top->send.empty()) {
//                flg = !flg;
//                if (flg) continue;
//                return;
//            }
//
//            this->event_on(conn,
//                [this, conn2 = conn] (const shared_conn & conn, int flags, manapi::slice_complex *sc)
//                    -> void {
//                    if (flags & ev::DISCONNECT) {
//
//                    }
//                    else {
//                        try {
//                            if (flags & ev::WRITE) {
//                                this->shutdown_async(conn);
//                            }
//
//                            return;
//                        }
//                        catch (std::exception const &e) {
//                            manapi_log_error(e.what());
//                        }
//                    }
//
//                    TCP::close_connection(conn, CLOSE_CONN_ERR);
//            }, manapi::net::worker::EVENT_ON_WRITE|manapi::net::worker::EVENT_ON_AUTO_REMOVE);
//
//            return;
//        }
//        goto err;
//    }
//
//write:
//    if (this->ssl_bio_flush_write(conn, s, this->m_config->max_buffer_size, false)) {
//        goto err;
//    }
//
//    return;
//err:
//    s->flags ^= CONN_TLS_SHUTDOWN;
//    TCP::close_connection(conn, CLOSE_CONN_ERR);
//}
//
//void manapi::net::worker::TLS::onrecv(const std::shared_ptr<ev::tcp> &watcher, const shared_conn &conn, ibuffpool_t &&buffer) MANAPICORE_NOEXCEPT {
//    auto buff = buffer.data();
//    auto size = buffer.size();
//    auto const data = conn->as<tls_connection_t>();
//    int rhs;
//
//    data->transfered += size;
//
//    manapi_log_trace_hard("TLS:recv %p flags=%d size=%zu", conn.get(), data->flags, buffer.size());
//
//    while (size) {
//        rhs = this->ssl_bio_write(data->rbio, buff, static_cast<int>(size));
//        if (rhs <= 0) {
//            /* error */
//            goto err;
//        }
//
//        buff += rhs;
//        size -= static_cast<uint32_t>(rhs);
//
//        assert(!(data->flags & CONN_TLS_SHUTDOWN) || !(data->flags & CONN_CLOSED));
//
//        while (!(data->flags & CONN_CLOSED)) {
//            if (this->ssl_is_init_fininshed(data->ssl)) [[likely]] {
//                rhs = this->manapi_do_process (conn, data);
//                if (rhs == CONN_IO_ERROR) {
//                    goto err;
//                }
//            }
//            else {
//                if (data->flags & CONN_TLS_EARLY_DATA && !(data->flags & CONN_TLS_EARLY_FINISHED)) {
//                    assert (!!conn->wrk.data);
//
//                    rhs = this->manapi_do_process (conn, data);
//                    if (rhs == CONN_IO_ERROR) {
//                        goto err;
//                    }
//
//                } else {
//                    rhs = this->manapi_do_handshake (conn, data);
//                    switch (rhs) {
//                        case CONN_IO_OK:
//                            continue;
//                        case CONN_IO_AGAIN:
//                            break;
//                        case CONN_IO_ERROR:
//                            default: goto err;
//                    }
//                }
//            }
//
//            break;
//        }
//    }
//
//    return;
//
//    err: {
//        conn->cancellation.cancel();
//
//        int flags;
//
//        if (rhs < 0) {
//            flags = CLOSE_CONN_EOS;
//        } else {
//            flags = CLOSE_CONN_ERR;
//        }
//
//        this->close_connection(conn, flags);
//    }
//}
//
//int manapi::net::worker::TLS::conn_after_write(const worker::shared_conn &conn) MANAPICORE_NOEXCEPT {
//    if (auto rhs = TCP::conn_after_write(conn)) {
//        return rhs;
//    }
//
//    auto const m = conn->as<tls_connection_t>();
//
//    if (auto rhs = this->ssl_bio_flush_write(conn, m, this->m_config->max_buffer_size, false)) {
//        return rhs;
//    }
//
//    if (m->flags & CONN_TLS_SHUTDOWN || !ssl_is_init_fininshed(m->ssl)) {
//        if (this->flush_write(conn, true)) {
//            return ERR_UNAVAILABLE;
//        }
//    }
//
//    return ERR_OK;
//}
//
//manapi::net::worker::shared_conn manapi::net::worker::TLS::connection_init_cb(void *user_data) MANAPICORE_NOEXCEPT {
//    auto const w = static_cast<TLS *> (user_data);
//    try {
//        auto p = std::make_unique<tls_connection_t>();
//        auto ms = manapi::reference <worker::connection> (new worker::connection(p.release(), connection_interface_eraser));
//
//        auto connection = ms->as<tls_connection_t>();
//
//        connection->ssl = w->ssl_new(w->m_ctx);
//
//        if (!connection->ssl)
//            return nullptr;
//
//        if (w->recv_setup_connection (ms, connection))
//            return nullptr;
//
//        auto rhs = manapi::async::current()->timerpool()->append_timer_sync(w->m_config->tls_accept_timeout,
//            [ms = ms.get()] (const manapi::timer &t) mutable
//            -> void {
//                auto w = ms->as<tcp_connection_t>();
//                w->worker->close_connection(manapi::reference (ms), CLOSE_CONN_ERR);
//        });
//
//        if (!rhs.ok())
//            return nullptr;
//
//        prepared::timer_clear(std::move(connection->accept_timer));
//        connection->accept_timer = rhs.unwrap();
//
//        return std::move(ms);
//    }
//    catch (std::bad_alloc const &) {
//        /* skip */
//    }
//    catch (std::exception const &e) {
//        manapi_log_error("%s failed due to %s", "accept", e.what());
//    }
//    return nullptr;
//}
//
//int manapi::net::worker::TLS::manapi_do_process(const shared_conn &conn, tls_connection_t *data) {
//    if (data->flags & CONN_TLS_SHUTDOWN) {
//        this->shutdown_async(conn);
//    } else {
//        if (data->accept_timer) prepared::timer_clear(std::move(data->accept_timer));
//
//        if (auto const res = this->ssl_bio_flush_read(conn, data, std::numeric_limits<std::size_t>::max())) {
//            return CONN_IO_ERROR;
//        }
//    }
//
//    return CONN_IO_OK;
//}
//
//int manapi::net::worker::TLS::manapi_do_handshake(const shared_conn &conn, tls_connection_t *data) {
//
//    if (!(data->flags & (CONN_TLS_EARLY_DATA|CONN_TLS_EARLY_FINISHED)) && this->ssl_early_data_is_enabled(this->m_ctx)) {
//        data->flags |= CONN_TLS_EARLY_DATA;
//        manapi_log_trace(debug::LOG_TRACE_LOW, "TLS:Try early data %p", conn.get());
//        return CONN_IO_OK;
//    }
//
//    int rhs = this->ssl_accept(data->ssl);
//    manapi_log_trace(debug::LOG_TRACE_LOW, "TLS:Handshake() %p = %d", conn.get(), rhs);
//
//    if (rhs == 1) {
//        assert (!!conn->wrk.data);
//    } else {
//        auto const status = this->ssl_get_error(data->ssl, rhs);
//
//        if (status == this->m_ssl_error_want_read || status == this->m_ssl_error_want_write) {
//            /* force write all data */
//            if (this->ssl_bio_flush_write(conn, data, this->m_config->max_buffer_size, false)) {
//                return CONN_IO_ERROR;
//            }
//        } else {
//            return CONN_IO_ERROR;
//        }
//    }
//
//    if (ssl_is_init_fininshed (data->ssl)) {
//        if (data->flags & CONN_TLS_EARLY_DATA) {
//            data->flags ^= CONN_TLS_EARLY_DATA;
//        }
//
//        prepared::timer_clear(std::move(data->accept_timer));
//
//        if (data->flags & CONN_TLS_SHUTDOWN) {
//            this->shutdown_async(conn);
//        }
//
//        return CONN_IO_OK;
//    }
//    return CONN_IO_AGAIN;
//}
//
//int manapi::net::worker::TLS::ssl_bio_flush_write(const shared_conn &conn, tls_connection_t *m, std::size_t maxsize, bool fin) MANAPICORE_NOEXCEPT {
//
//
//    try {
//        int rhs;
//
//        if ((m->top->send.size() + m->top->pending_send < maxsize) || fin) {
//            auto last = m->top->send.slices_rbegin();
//            std::size_t last_shift;
//            if (last) last_shift = static_cast<std::size_t>(last->buff.len) - m->top->send.rshift();
//            std::size_t left = this->ssl_bio_ctrl_pending(m->wbio);
//            if (!m->top->send.resize(m->top->send.size() + left).ok()) return CONN_IO_ERROR;
//            if (!last) { last = m->top->send.slices_begin(); last_shift = 0; }
//
//            if (!!last && left) {
//                for (;;) {
//
//                    std::size_t len = static_cast<std::size_t>(last->buff.len) - last_shift;
//
//                    if (!len) {
//                        last_shift = 0;
//                        assert(left >= last_shift);
//                        left -= last_shift;
//                        last = last->next;
//
//                        if (!!last) break;
//
//                        continue;
//                    }
//
//                    rhs = this->ssl_bio_read(m->wbio, last->buff.base + last_shift, static_cast<int>(len));
//
//                    if (rhs > 0) {
//                        last_shift += static_cast<std::size_t>(rhs);
//                    } else {
//                        if (!this->ssl_bio_should_retry(m->wbio)) {
//                            return CONN_IO_ERROR;
//                        }
//
//                        break;
//                    }
//                }
//            }
//
//            assert(left >= last_shift);
//            left -= last_shift;
//
//            if (!m->top->send.resize( m->top->send.size() - left ).ok()) return CONN_IO_ERROR;
//
//        }
//
//        this->flush_write(conn, fin);
//
//        return CONN_IO_OK;
//    }
//    catch (std::exception const &e) {
//        manapi::async::log()->error(manapi::ERR_INTERNAL, "TLS::ssl_bio_flush_write(...): {}", e.what());
//    }
//
//    return CONN_IO_ERROR;
//}
//
//int manapi::net::worker::TLS::ssl_bio_flush_read(const shared_conn &conn, tls_connection_t *m, std::size_t maxsize) {
//    int rhs;
//
//    try {
//        if (maxsize > m->top->recv.size()) {
//            if (m->flags & CONN_TLS_EARLY_DATA) [[unlikely]] {
//                for (;;) {
//                    std::size_t readbytes;
//                    char buffer[4096];
//                    rhs = this->ssl_read_early_data(m->ssl, buffer, sizeof (buffer), &readbytes);
//                    if (rhs == this->m_early_data_read_error) {
//                        break;
//                    } else {
//
//                        m->top->recv.push_back( buffer, readbytes ).unwrap();
//
//                        if (rhs == this->m_early_data_read_finish) {
//
//                            m->flags |= CONN_TLS_EARLY_FINISHED;
//
//                            rhs = this->manapi_do_handshake(conn, m);
//                            switch (rhs) {
//                                case CONN_IO_OK:
//                                    m->flags ^= CONN_TLS_EARLY_DATA;
//                                    manapi_log_trace(debug::LOG_TRACE_LOW, "TLS:Early data was read %p", conn.get());
//                                    break;
//                                case CONN_IO_ERROR: return CONN_IO_ERROR;
//                                case CONN_IO_AGAIN: break;
//                                default: return CONN_IO_ERROR;
//                            }
//
//                            break;
//                        }
//                    }
//                }
//            }
//
//
//            auto last = m->top->recv.slices_rbegin();
//            std::size_t last_shift;
//            if (last) last_shift = static_cast<std::size_t>(last->buff.len) - m->top->recv.rshift();
//            std::size_t left = this->ssl_pending (m->rbio);
//            m->top->recv.resize(m->top->recv.size() + left).unwrap();
//            if (!last) { last = m->top->recv.slices_begin(); last_shift = 0; }
//
//            if (!!last && left) {
//
//                for (;;) {
//
//                    auto len = static_cast<std::size_t> (last->buff.len) - last_shift;
//
//                    if (!len) {
//                        last_shift = 0;
//                        assert(left >= last_shift);
//                        left -= last_shift;
//                        last = last->next;
//
//                        if (!!last) break;
//
//                        continue;
//                    }
//
//                    rhs = this->ssl_read(m->ssl, last->buff.base + last_shift,
//                             static_cast<int>(len));
//
//                    if (rhs > 0) {
//
//                        last_shift += static_cast<std::size_t> (rhs);
//
//                    } else {
//                        rhs = this->ssl_get_error(m->ssl, rhs);
//
//                        if (rhs == this->m_ssl_error_ssl) {
//                            return CONN_IO_ERROR;
//                        }
//
//                        if (rhs == this->m_ssl_error_syscall) {
//                            break;
//                        }
//
//                        if (rhs == this->m_ssl_error_want_read || rhs == this->m_ssl_error_want_write) {
//                            /* force write all data */
//                            if (this->ssl_bio_flush_write(conn, m, this->m_config->max_buffer_size, true)) {
//                                return CONN_IO_ERROR;
//                            }
//                        }
//
//                        if (rhs == this->m_ssl_error_zero_return) {
//                            this->close_connection(conn, CLOSE_CONN_SHUTDOWN);
//                        }
//
//                        break;
//                    }
//                }
//            }
//
//            this->flush_read( conn, m );
//        }
//
//        return CONN_IO_OK;
//    }
//    catch (std::exception const &e) {
//        manapi::async::current()->logger()->error(manapi::ERR_INTERNAL, "TLS::ssl_bio_flush_read(...): {}", e.what());
//    }
//
//    return CONN_IO_ERROR;
//}