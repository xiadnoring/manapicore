//#include "worker/ManapiHttp3Worker.hpp"
//
//#define MANAPI_HTTP3_WORKER_AS_STREAM(n__) static_cast<manapi::net::worker::http_v3_stream_base_t *>(n__)
//
//int manapi::net::worker::http_v3_flush_recv(http::config *config, const manapi::net::worker::shared_conn &conn, manapi::net::worker::http_v3_stream_base_t *s, bool force) MANAPICORE_NOEXCEPT {
//    return prepared::flush_read2(config, conn, s, force);
//}
//
//manapi::net::worker::http_v3::http_v3(worker::base *w, http_v3_callbacks_t *callbacks) : m_w(w), m_callbacks(callbacks) {
//    this->m_worker_data = this->m_w->m_worker_data;
//    this->m_config = this->m_w->m_config;
//    this->m_site = this->m_site;
//}
//
//manapi::net::worker::http_v3::~http_v3() = default;
//
//manapi::net::worker::wrk_interface_global_t * manapi::net::worker::http_v3::wrk_global() MANAPICORE_NOEXCEPT {
//    return this->m_w->wrk_global();
//}
//
//void manapi::net::worker::http_v3::wrk_global(wrk_interface_global_t *data) MANAPICORE_NOEXCEPT {
//    this->m_w->wrk_global(data);
//}
//
//void manapi::net::worker::http_v3::waiting(const shared_conn &conn, bool state) MANAPICORE_NOEXCEPT {
//    return this->m_w->waiting(conn, state);
//}
//
//void manapi::net::worker::http_v3::feed_event(const shared_conn &conn, int flags, const char *buff, std::size_t size, ibuffpool_t *p) MANAPICORE_NOEXCEPT {
//    return prepared::feed_event(this, conn, MANAPI_HTTP3_WORKER_AS_STREAM(conn->wrk.data), flags, buff, size, p);
//}
//
//void manapi::net::worker::http_v3::close_connection(shared_conn conn, int flags) MANAPICORE_NOEXCEPT {
//    if (!conn)
//        return;
//
//    auto data = MANAPI_HTTP3_WORKER_AS_STREAM (conn->wrk.data);
//
//    if (data->flags & CONN_REMOVED) {
//        return;
//    }
//
//    manapi_log_trace(debug::LOG_TRACE_MEDIUM, "http3:close_connection() %p id=%zu flags=%d",
//        data, this->m_w->stream_id(conn), flags);
//
//    data->flags |= CONN_CLOSED;
//
//    if ((flags & CLOSE_CONN_SHUTDOWN) && data->top->send_size)
//        return;
//
//    data->flags |= CONN_REMOVED;
//
//    prepared::event_callback_clear(conn, data);
//
//    conn->cancellation.cancel();
//
//    if ((data->flags & CONN_RECV_END)
//        && (data->flags & CONN_SEND_END))
//        return;
//
//    /* got something wrong */
//    this->m_callbacks->http_v3_rst_stream(conn, HTTP3_ERROR_NO_ERROR);
//}
//
//int manapi::net::worker::http_v3::event_flags(const shared_conn &conn) MANAPICORE_NOEXCEPT {
//    return prepared::event_flags(conn, MANAPI_HTTP3_WORKER_AS_STREAM(conn->wrk.data));
//}
//
//int manapi::net::worker::http_v3::event_flags(const shared_conn &conn, int flags) MANAPICORE_NOEXCEPT {
//    auto const data = MANAPI_HTTP3_WORKER_AS_STREAM(conn->wrk.data);
//
//    MANAPICORE_WORKER_EVENT_LOOP_STREAM(data) {
//
//        if (data->flags & CONN_CLOSED) {
//            if (http_v3::notify_cb(data->ev_cb, conn, CONN_CLOSED, nullptr, 0, nullptr))
//                this->close_connection(conn, CLOSE_CONN_ERR);
//        }
//        else {
//            if (status & ev::WRITE) {
//                this->m_callbacks->http_v3_want_write(conn);
//            }
//
//            if (status & ev::READ) {
//                this->m_callbacks->http_v3_on_read_stream (conn);
//
//                if (data->flags & CONN_RECV_END) {
//                    if (manapi::net::worker::http_v3::notify_cb(data->ev_cb, conn,
//                        CONN_RECV_END, nullptr, 0, nullptr))
//                        this->close_connection(conn, CLOSE_CONN_ERR);
//                }
//            }
//        }
//
//        MANAPICORE_WORKER_EVENT_BREAK(data)
//    }
//
//    return prev;
//}
//
//manapi::net::worker::worker_watcher_cb manapi::net::worker::http_v3::event_on(const shared_conn &conn, worker_watcher_cb callback) MANAPICORE_NOEXCEPT {
//    return prepared::event_on(conn, MANAPI_HTTP3_WORKER_AS_STREAM(conn->wrk.data), std::move(callback));
//}
//
//manapi::future<manapi::status> manapi::net::worker::http_v3::init(std::size_t deep) {
//    co_return status_ok();
//}
//
//manapi::net::worker::connection::ipdata_t * manapi::net::worker::http_v3::ipdata(worker::connection *conn) MANAPICORE_NOEXCEPT {
//    return this->m_w->ipdata(conn);
//}
//
//bool manapi::net::worker::http_v3::is_writable(const shared_conn &conn) MANAPICORE_NOEXCEPT {
//    auto const s = conn->as<http_v3_stream_base_t>();
//    return this->m_callbacks->http_v3_is_writable(conn) && prepared::is_writable(this->m_config.get(), conn, s);
//}
//
//void manapi::net::worker::http_v3::stop(manapi::stoken token) {
//}
//
//ssize_t manapi::net::worker::http_v3::sync_write(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) MANAPICORE_NOEXCEPT {
//    return sync_write_ex (conn, buff, nbuff, 0 /* no need */, finish, (this->m_w->m_config->max_buffer_stack));
//}
//
//ssize_t manapi::net::worker::http_v3::sync_write_ex(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, std::size_t size, bool finish, std::size_t maxcnt) MANAPICORE_NOEXCEPT {
//    return this->m_callbacks->http_v3_write(conn, buff, nbuff, finish);
//}
//
//void manapi::net::worker::http_v3::update_limit_rate_stream(const shared_conn &conn) MANAPICORE_NOEXCEPT {
//    return prepared::update_limit_rate_connection(conn, MANAPI_HTTP3_WORKER_AS_STREAM(conn->wrk.data), this,
//              this->m_config.get(), this->m_config->speed_stream_check_delay, this->m_config->speed_stream_check_bytes, this->wrk_global());
//}
//
//std::size_t manapi::net::worker::http_v3::recv_count(const shared_conn &conn) const MANAPICORE_NOEXCEPT {
//    return prepared::recv_count(conn, MANAPI_HTTP3_WORKER_AS_STREAM(conn->wrk.data));
//}
//
//manapi::bytebuffer manapi::net::worker::http_v3::recv_first_buffer(const shared_conn &conn) MANAPICORE_NOEXCEPT {
//    return prepared::recv_first_buffer(conn, MANAPI_HTTP3_WORKER_AS_STREAM(conn->wrk.data));
//}
//
//
//
