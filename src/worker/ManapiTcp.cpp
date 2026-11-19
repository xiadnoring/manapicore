//#include "worker/ManapiTcp.hpp"
//
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
//#include <memory>
//#include <set>
//#include <future>
//
//#include "ManapiParams.hpp"
//#include "ManapiString.hpp"
//#include "ManapiDns.hpp"
//#include "ManapiTimerPool.hpp"
//#include "http/ManapiBaseHttp.hpp"
//#include "http/ManapiHttpRequest.hpp"
//#include "http/ManapiHttpResponse.hpp"
//#include "std/ManapiSocket.hpp"
//#include "worker/ManapiBaseUtils.hpp"
//#include "../include/http/ManapiHttp2.hpp"
//#include "../include/ManapiUtils.hpp"
//
//static void manapi__tcp_on_client_close (uv_handle_t *handle) {
//    delete static_cast<manapi::ev::tcp *> (handle->data);
//}
//
//
//manapi::net::worker::TCP::TCP()
//        : interface_worker (), m_sockaddrin() {
//    this->m_local = nullptr;
//    this->m_flags = 0;
//}
//
//manapi::net::worker::TCP::~TCP() {
//    if (this->m_local) {
//        ev::getaddrinfo::free(this->m_local);
//    }
//
//    if (this->m_limit_rate_timer) {
//        this->m_limit_rate_timer.stop();
//        this->m_limit_rate_timer = nullptr;
//    }
//
//
//    if (this->m_watcher_accept) {
//        if (manapi::async::eventloop()->has_stop_callback( this->m_watcher_accept )) {
//            manapi::async::eventloop()->stop_callback(this->m_watcher_accept, nullptr);
//            this->m_token.unref();
//        }
//        assert(!this->m_watcher_accept->read_stop());
//        manapi::async::eventloop()->stop_watcher( std::move(this->m_watcher_accept) );
//    }
//}
//
//manapi::future<manapi::status> manapi::net::worker::TCP::init(std::size_t deep) {
//    try {
//
//        {
//            auto st = co_await manapi::net::worker::interface_worker::init( deep + 1 );
//            if (!st.ok()) co_return std::move(st);
//        }
//
//        addrinfo hints = {
//                .ai_family      = PF_UNSPEC,
//                .ai_socktype    = SOCK_STREAM,
//                .ai_protocol    = IPPROTO_TCP
//        };
//
//        auto &address = this->m_config->address;
//        auto &port = this->m_config->port;
//
//        int rhs;
//        if ((rhs = co_await dns::getaddrinfo(address.data(), port.data(), &hints, &this->m_local)))
//            co_return status_internal("tcp:failed to resolve host");
//
//        this->m_config->server_len=static_cast<socklen_t>(this->m_local->ai_addrlen);
//        memcpy (&this->m_config->server_addr,this->m_local->ai_addr, this->m_local->ai_addrlen);
//
//        manapi_log_trace(debug::LOG_TRACE_HIGH, "TCP PORT USED: %.*s. %.*s:%.*s", port.size(), port.data(),
//                         address.size(), address.data(), port.size(), port.data());
//
//        /* every 1 second */
//        this->m_limit_rate_timer = manapi::async::current()->timerpool()->append_interval_sync(1000,manapi::TIMER_IMPORTANT,
//                                                                                               [this] (const manapi::timer& t) -> void { this->update_limit_rate(); }).unwrap();
//
//        this->m_watcher_accept = manapi::async::current()->eventloop()->create_watcher_tcp_accept(
//                [this] (const std::shared_ptr<ev::tcp> & w, int status)
//                        -> void { this->onaccept(w, status); }).unwrap();
//
//        manapi::async::current()->eventloop()
//                ->stop_callback(this->m_watcher_accept,
//                                [this](const ev::shared_tcp &w) mutable -> void { this->m_token.unref(); });
//
//        this->m_token.ref();
//
//        memset(&this->m_sockaddrin, '\0', sizeof (sockaddr));
//
//        if (this->m_local->ai_family == ev::IPv4) {
//            if ((rhs = this->m_watcher_accept->ip4_addr(this->m_config->address.data(), std::stoi(this->m_config->port), reinterpret_cast<sockaddr_in *>(&this->m_sockaddrin)))) {
//                manapi_log_error("tcp:couldn't set ipv4 addr due to result: %s", ev::strerror(rhs));
//                co_return status_internal("tcp:ip4_addr failed");
//            }
//        }
//        else if (this->m_local->ai_family == ev::IPv6) {
//            if ((rhs = this->m_watcher_accept->ip6_addr(this->m_config->address.data(), std::stoi(this->m_config->port), reinterpret_cast<sockaddr_in6 *>(&this->m_sockaddrin)))) {
//                manapi_log_error("tcp:couldn't set ipv6 addr due to result: %s", ev::strerror(rhs));
//                co_return status_internal("tcp:ip6_addr failed");
//            }
//        }
//
//        if ((rhs = this->m_watcher_accept->nodelay(this->m_config->tcp_no_delay))) {
//            manapi_log_error("tcp:couldn't set nodelay due to result: %s", ev::strerror(rhs));
//            co_return status_internal("tcp:nodelay failed");
//        }
//
//        if ((rhs = this->m_watcher_accept->simultaneous_accepts(this->m_config->simultaneous_accepts))) {
//            manapi_log_error ("tcp:couldn't set simultaneous_accepts due to result:%s",ev::strerror(rhs));
//            co_return status_internal("tcp:simultaneous_accepts failed");
//        }
//
//        if ((rhs = this->m_watcher_accept->keepalive(!!this->m_config->keep_alive, this->m_config->keep_alive))) {
//            manapi_log_error("tcp:couldn't set keep-alive due to result:%s", rhs);
//            co_return status_internal("tcp:keepalive failed");
//        }
//
//        {
//            uint32_t bind_flags = 0;
//#if defined(__unix__) && !defined(__APPLE__)
//            bind_flags |= ev::TCP_REUSEPORT;
//#endif
//            rhs = this->m_watcher_accept->s_bind(reinterpret_cast<sockaddr *> (&this->m_sockaddrin), bind_flags);
//            if (rhs) {
//                manapi_log_error("tcp:couldn't bind socket due to result:%s", ev::strerror(rhs));
//                co_return status_internal("tcp:s_bind failed");
//            }
//        }
//
//        if ((rhs = this->m_watcher_accept->listen(this->m_config->tcp_backlog))) {
//            manapi_log_error("tcp:couldn't listen socket due to result:%s", ev::strerror(rhs));
//            co_return status_internal("tcp:listen failed");
//        }
//
//
//        co_return status_ok();
//    }
//    catch (std::exception const &e) {
//        manapi_log_error("%s due to %s", "tcp: init failed", e.what());
//    }
//
//    co_return status_internal("tcp: init failed");
//}
//
//void manapi::net::worker::TCP::waiting(const shared_conn &conn, bool state) MANAPICORE_NOEXCEPT {
//    prepared::waiting(conn, state);
//}
//
//void manapi::net::worker::TCP::onaccept(const std::shared_ptr<ev::tcp> &watcher, int status) MANAPICORE_NOEXCEPT {
//    if (status) return;
//
//    shared_conn connection;
//
//    if (!(connection = this->accept(watcher))) {
//        manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s:%s failed", "tcp", "new_connection");
//        return;
//    }
//
//    if (this->init_conn(connection)) {
//        this->close_connection(connection, CLOSE_CONN_ERR);
//    }
//}
//
//void manapi::net::worker::TCP::onrecv(const std::shared_ptr<ev::tcp> &watcher, const worker::shared_conn &conn) MANAPICORE_NOEXCEPT {
//    auto const connection = conn->as<tcp_connection_t>();
//
//    manapi_log_trace_hard("tcp:recv conn=%p size=%zu", conn.get(), this->m_sc.base()->size());
//
//    connection->transfered += this->m_sc.base()->size();
//
//    if (connection->flags & WRK_INTERFACE_CUSTOM_READ) {
//        this->m_global.custom_read_cb ( conn, &this->m_sc, &this->m_global);
//    }
//
//    this->feed_event ( conn, ev::READ, &this->m_sc );
//
//    if (connection->transfered >= this->m_config->speed_limit_rate) {
//        this->read_stop(conn, connection);
//    }
//}
//
//std::shared_ptr<manapi::net::worker::TCP> manapi::net::worker::TCP::create() {
//    return std::make_shared<worker::TCP>();
//}
//
//
//manapi::net::worker::shared_conn manapi::net::worker::TCP::accept (const ev::shared_tcp &w, shared_conn (*init_cb) (void *user_data), void *user_data) MANAPICORE_NOEXCEPT {
//
//    auto const count = this->m_worker_data->count.load();
//
//    if (count > this->m_config->max_connections + this->m_config->max_connections) {
//        std::unique_ptr<ev::tcp> client (new (std::nothrow) ev::tcp{});
//        if (!client)
//            return nullptr;
//        auto const ptr = client.get();
//        auto const loop = manapi::async::current()->eventloop()->loop();
//        if (!loop)
//            return nullptr;
//        if (auto rhs = ptr->bind(loop)) {
//            return nullptr;
//        }
//        if (auto rhs = ptr->accept(w.get())) {
//            manapi_log_error("%s due to %d", "accept() failed", rhs);
//            return nullptr;
//        }
//        ptr->data(client.release());
//        if (auto const rhs = ptr->close_reset(manapi__tcp_on_client_close)) {
//            manapi_log_error("%s due to %d", "close_reset() failed", rhs);
//            return nullptr;
//        }
//
//        manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s due to %s", "tcp:conn closed", "limits");
//
//        return nullptr;
//    }
//
//    reference <worker::connection> connection;
//    std::array<char, 17> arr;
//    arr.fill('\0');
//
//    try {
//        if (!(connection = init_cb(user_data)))
//            return connection;
//
//        auto wres = manapi::async::current()->eventloop()->create_watcher_tcp_connection(
//                [this, conn_ptr = connection.get()] (const std::shared_ptr<ev::tcp> &w, ssize_t nread, const uv_buf_t *buf)
//                        -> void {
//                    auto connection = reference (conn_ptr);
//
//                    try {
//                        if (buf->base) this->m_sc = manapi::slice (buf);
//
//                        if (!nread) return;
//
//                        if (nread < 0) {
//                            if (nread == ev::ERR_AGAIN) return;
//
//                            /* maybe EOS */
//                            manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "EOR was received %p conn", connection.get());
//                            this->close_connection(connection, CLOSE_CONN_EOR);
//                            return;
//                        }
//
//
//                        this->m_sc.get_slice()->resize( static_cast<std::size_t>(nread) ).unwrap();
//
//                        this->onrecv(w, connection);
//                    }
//                    catch (std::exception const &e) {
//                        manapi::async::current()->logger()->ferror(manapi::ERR_INTERNAL, "tcp: onrecv(...) unexpected error: %s", e.what());
//                        this->close_connection(connection, CLOSE_CONN_ERR);
//                    }
//                }, [] (const std::shared_ptr<ev::tcp> &, size_t suggested_size, ev::buff_t *buff) MANAPICORE_NOEXCEPT -> void {
//                    auto res = manapi::async::memory_fabric()->buffer(suggested_size);
//                    if (res.ok()) {
//                        auto buffer = res.unwrap();
//                        buff->len = static_cast<decltype(buff->len)>(buffer.size());
//                        buff->base = static_cast<char *>(buffer.release());
//                    }
//                });
//
//        if (!wres) {
//            goto err;
//        }
//
//        auto client = wres.unwrap();
//
//        if (auto rhs = client->accept(w.get())) {
//            manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s due to %s", "tcp:accept", ev::strerror(rhs));
//            manapi::async::current()->eventloop()->stop_watcher(std::move(client));
//            return nullptr;
//        }
//
//
//        if (auto rhs = client->keepalive(!!this->m_config->keep_alive, this->m_config->keep_alive)) {
//            manapi_log_error("%s due to %s", "tcp:couldn't set keep-alive", ev::strerror(rhs));
//            manapi::async::log()->error(ERR_FAILED_PRECONDITION, "couldn't set keep-alive due to result - {}", rhs);
//            manapi::async::eventloop()->stop_watcher(std::move(client));
//            return nullptr;
//        }
//
//        auto conn = connection->as<tcp_connection_t>();
//
//        conn->worker = this;
//        conn->watcher = std::move(client);
//
//        if (count >= this->m_config->max_connections) {
//            connection->wrk.flags |= WRK_INTERFACE_CONN_RETRY;
//        }
//
//        this->m_worker_data->count.fetch_add(1);
//        this->m_token.ref();
//
//        connection->ipdata = std::make_unique<worker::connection::ipdata_t>();
//        connection->ipdata->len = 0;
//
//        int addrlen = sizeof (connection->ipdata->client.data);
//        if (auto rhs = conn->watcher->getpeername(reinterpret_cast <sockaddr *>(connection->ipdata->client.data), &addrlen)) {
//            manapi_log_error("getpeername() failed: %d", rhs);
//            return nullptr;
//        }
//        connection->ipdata->len = static_cast<uint32_t>(addrlen);
//
//        conn->top = std::make_unique<connection_io>();
//
//        auto const sn = reinterpret_cast <sockaddr *>(connection->ipdata->client.data);
//        arr[0] = static_cast<char>(http::version_ip_by_addr (sn));
//        http::ip_by_addr(sn, arr.data() + 1);
//
//        auto findex = std::string_view(arr.data(), arr.size());
//        auto it = this->m_ips.find(findex);
//        if (it == this->m_ips.end())
//            it = this->m_ips.insert_or_assign(std::string(findex), conn_by_port{}).first;
//
//        if (it->second.size() >= this->m_config->max_connections_by_ip) {
//            if (it->second.size() > this->m_config->max_connections_by_ip * 2)
//                return nullptr;
//
//            connection->wrk.flags |= WRK_INTERFACE_CONN_RETRY;
//        }
//
//        auto res = it->second.insert_or_assign(reinterpret_cast<uintptr_t>(connection.get()),
//                                               connection);
//    }
//    catch (std::exception const &e) {
//        manapi_log_error("tcp accept: failed due to %s", e.what());
//
//        goto err;
//    }
//
//    return std::move(connection);
//
//    err:
//    manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s:%s failed", "tcp", "accept");
//
//    if (connection) {
//        auto it = this->m_ips.find(std::string_view(arr.data(), arr.size()));
//        if (it != this->m_ips.end()) {
//            auto git = it->second.find(reinterpret_cast<uintptr_t>(connection.get()));
//            if (git != it->second.end())
//                git->second = nullptr;
//        }
//    }
//
//    return nullptr;
//}
//
//manapi::net::worker::shared_conn manapi::net::worker::TCP::accept(const ev::shared_tcp &w) MANAPICORE_NOEXCEPT {
//    return this->accept(w, connection_init_cb, this);
//}
//
//void manapi::net::worker::TCP::close_connection(shared_conn conn, int flags) MANAPICORE_NOEXCEPT {
//    if (!conn) return;
//
//    auto connection = conn->as<tcp_connection_t>();
//
//    if (flags & CLOSE_CONN_EOR || flags & CLOSE_CONN_EOS) {
//
//        conn->wrk.flags |= WRK_INTERFACE_IS_DRAINING;
//
//    } else if (connection->flags & CONN_WANT_CLOSE) {
//
//        if (!connection->ev_cb.empty() && !(manapi::net::worker::TCP::notify_cb(connection->ev_cb.back().first, conn, CONN_WANT_CLOSE, nullptr))) {
//
//            if (connection->flags & CONN_WANT_CLOSE)
//                return;
//
//        }
//    }
//
//    if (connection->flags & CONN_WANT_CLOSE) connection->flags ^= CONN_WANT_CLOSE;
//
//    /* Draining timeout is enabled, thus, connection is draining */
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
//    manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "TCP:close_conection() conn=%p flags=%d",
//                     conn.get(), flags);
//
//    if ((flags & CLOSE_CONN_FINISHED || flags & CLOSE_CONN_SHUTDOWN) && !(flags & CLOSE_CONN_EOS) && !(flags & CLOSE_CONN_ERR)) {
//        connection->flags |= CONN_RECV_END;
//        connection->flags |= CONN_SEND_END;
//
//        manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "TCP:closing the connection %p with CLOSE_CONN_FINISHED "
//                                                       "top->pending_send=%zu top->send=%zu", conn.get(), connection->top->pending_send, connection->top->send.size());
//
//        if (!connection->top->send.empty()) {
//            this->flush_write(conn, true);
//        }
//
//        if (connection->top->pending_send) {
//            // wait
//            return;
//        }
//    }
//
//    this->waiting(conn, true);
//
//    if (connection->flags & CONN_REMOVED)
//        return;
//
//    manapi_log_trace(debug::LOG_TRACE_LOW, "TCP:close_connection() %p flags=%d",conn.get(), flags);
//
//    conn->cancellation.cancel();
//
//    int terminate = 0;
//
//    if (flags & CLOSE_CONN_ERR || flags & CLOSE_CONN_EOS || flags & CLOSE_CONN_SHUTDOWN) {
//        terminate = 1;
//    }
//
//    else if (!(flags & CLOSE_CONN_FINISHED) || !this->m_config->keep_alive) {
//        terminate = 1;
//    }
//
//    else if (conn->wrk.flags & WRK_INTERFACE_IS_DRAINING || !(conn->wrk.flags & WRK_INTERFACE_TCP_KEEP_ALIVE)) {
//        terminate = 1;
//    }
//
//    if (terminate) {
//
//        connection->flags |= CONN_CLOSED;
//
//        if (conn->wrk.flags & WRK_INTERFACE_TCP_KEEP_ALIVE)
//            conn->wrk.flags ^= WRK_INTERFACE_TCP_KEEP_ALIVE;
//
//        connection->flags |= CONN_REMOVED;
//
//        prepared::timer_clear(std::move(connection->t));
//
//        prepared::event_callback_clear(this, conn, connection);
//
//        this->read_stop(conn, connection);
//
//        prepared::top_buffer_clear(connection);
//
//        std::array<char, 17> arr;
//        arr.fill('\0');
//        auto const sn = reinterpret_cast <sockaddr *>(conn->ipdata->client.data);
//        arr[0] = static_cast<char>(http::version_ip_by_addr (sn));
//        http::ip_by_addr(sn, arr.data() + 1);
//        auto it = this->m_ips.find(std::string_view(arr.data(), arr.size()));
//        if (it != this->m_ips.end()) {
//            auto git = it->second.find(reinterpret_cast<uintptr_t>(conn.get()));
//            if (git != it->second.end()) {
//                git->second = nullptr;
//            }
//        }
//    }
//    else {
//        manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "tcp renew connection %p", conn.get());
//        try {
//            if (this->m_global.cleanup_cb(conn.get(), &this->m_global))
//                manapi_log_trace("tcp:this->m_global.cleanup_cb failed");
//
//            conn->cancellation.reset();
//
//            prepared::timer_clear(std::move(connection->t));
//
//            connection->t = manapi::async::current()->timerpool()->append_interval_sync(
//                    this->m_config->keep_alive * 1000,
//                    manapi::TIMER_IMPORTANT,
//                    [conn] (manapi::timer t) mutable
//                            -> void {
//                        dynamic_cast <TCP*>(conn->as<tcp_connection_t>()->worker)->on_timeout(conn);
//                    }).unwrap();
//
//            if (connection->flags & CONN_SEND_END)
//                connection->flags ^= CONN_SEND_END;
//
//            if (connection->flags & CONN_RECV_END)
//                connection->flags ^= CONN_RECV_END;
//
//            this->event_on(conn,
//               [this] (const worker::shared_conn &conn, int flags, manapi::slice_complex *sc) mutable
//                       -> void {
//                   if (flags & ev::DISCONNECT) {
//                       this->close_connection(conn, CLOSE_CONN_EOS);
//                       return;
//                   }
//
//                   if (flags & ev::READ) {
//                       auto const data = conn->as<tcp_connection_t>();
//
//                       prepared::timer_clear(std::move(data->t));
//
//                       if (this->init_conn(conn)) {
//                           this->close_connection(conn, CLOSE_CONN_ERR);
//                           return;
//                       }
//
//                       assert(conn->wrk.data);
//                       this->feed_event(conn, flags|CONN_TOP_READ, sc);
//                   }
//               }, manapi::net::worker::EVENT_ON_READ|manapi::net::worker::EVENT_ON_AUTO_REMOVE);
//
//            return;
//        }
//        catch (std::exception const &e) {
//            manapi_log_error(e.what());
//        }
//
//        this->close_connection(conn, CLOSE_CONN_ERR);
//    }
//}
//
//void manapi::net::worker::TCP::stop(manapi::stoken token) {
//    manapi::async::current()->eventloop()->stop_watcher(std::move(this->m_watcher_accept));
//    manapi::net::worker::interface_worker::stop(token);
//}
//
//void manapi::net::worker::TCP::feed_event(const shared_conn &conn, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT {
//    prepared::feed_event(this, conn, flags, sc);
//}
//
//ssize_t manapi::net::worker::TCP::sync_write_ex(const manapi::net::worker::shared_conn &conn, manapi::ev::buff_t *buf, uint32_t buf_cnt, bool finish, std::size_t maxsize) MANAPICORE_NOEXCEPT {
//    auto connection = conn->as<tcp_connection_t>();
//
//    if (connection->flags & CONN_CLOSED) return CONN_IO_ERROR;
//
//    ssize_t rhs;
//    std::size_t shift = 0;
//
//    if (!connection->top->send.empty() || connection->top->pending_send) {
//        rhs = 0;
//    }
//    else {
//
//        rhs = connection->watcher->try_write (buf, static_cast<uint32_t>(buf_cnt));
//
//        if (rhs < 0)
//            if (rhs == ev::ERR_AGAIN) {
//                rhs = 0;
//            } else {
//                manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s:%s failed due to %s",
//                                 "TCP", "try_write", ev::strerror(static_cast<int>(rhs)));
//                return -1;
//            }
//        else {
//            connection->transfered += static_cast<std::size_t>(rhs);
//            shift = static_cast<std::size_t> (rhs);
//        }
//    }
//
//    if (maxsize > connection->top->send.size()) {
//        while (!buf_cnt && shift >= static_cast<std::size_t> (buf->len)) {
//            shift -= static_cast<std::size_t> (buf->len);
//            buf++;
//            buf_cnt--;
//        }
//
//        while (!buf_cnt && maxsize > connection->top->send.size()) {
//            auto copy = std::min <std::size_t>( static_cast<std::size_t>(buf->len - shift), maxsize - connection->top->send.size());
//
//            if (!connection->top->send.push_back ( buf->base + shift, copy ).ok()) {
//                return 0;
//            }
//
//            shift += copy;
//
//            rhs += static_cast<ssize_t> (copy);
//
//            if (shift == static_cast<std::size_t> (buf->len)) {
//                shift = 0;
//                buf++;
//                buf_cnt--;
//            }
//        }
//
//        if (this->flush_write(conn, finish)) return -1;
//    }
//
//    return rhs;
//}
//
//ssize_t manapi::net::worker::TCP::sync_write_ex(const worker::shared_conn &conn, manapi::slice &sv, bool finish, std::size_t maxsize) MANAPICORE_NOEXCEPT {
//    auto connection = conn->as<tcp_connection_t>();
//
//    if (connection->flags & CONN_CLOSED) return CONN_IO_ERROR;
//
//    ssize_t rhs;
//
//    if (!connection->top->send.empty() || connection->top->pending_send) {
//        rhs = 0;
//    }
//    else {
//
//        auto const count = std::min<std::size_t>(sv.slices_size(), 128);
//#ifdef _MSC_VER
//        ev::buff_t *slices = static_cast<ev::buff_t*>(alloca(sizeof (ev::buff_t) * count));
//#else
//        ev::buff_t slices[count];
//#endif
//        std::size_t slices_sz = 0;
//        sv.slices_buffs(slices, count, &slices_sz);
//
//        rhs = connection->watcher->try_write (slices, static_cast<uint32_t>(slices_sz));
//
//        if (rhs < 0)
//            if (rhs == ev::ERR_AGAIN) {
//                rhs = 0;
//            } else {
//                manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s:%s failed due to %s",
//                                 "TCP", "try_write", ev::strerror(static_cast<int>(rhs)));
//                return -1;
//            }
//        else {
//            connection->transfered += static_cast<std::size_t>(rhs);
//            sv.shift_add( static_cast<std::size_t>(rhs) ).unwrap();
//        }
//    }
//
//
//    if (!sv.empty() && maxsize > connection->top->send.size() + connection->top->pending_send) {
//        auto copy = std::min <std::size_t> ( sv.size(), maxsize - (connection->top->send.size() + connection->top->pending_send) );
//
//        auto zsv = sv.split ( 0,  copy );
//        if (!zsv.ok() || !connection->top->send.push_back ( zsv.unwrap() ).ok()) {
//            return -1;
//        }
//
//        rhs += static_cast<ssize_t> (copy);
//
//        if (this->flush_write(conn, finish)) return -1;
//    }
//
//    return rhs;
//}
//
//ssize_t manapi::net::worker::TCP::sync_write(const worker::shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) MANAPICORE_NOEXCEPT {
//    return prepared::sync_write(this, conn, buff, nbuff, finish);
//}
//
//void manapi::net::worker::TCP::event_on(const shared_conn & conn, worker_watcher_cb&& callback, int flags) MANAPICORE_NOEXCEPT {
//    return prepared::event_on(this, conn, conn->as<connection_prepared_base_t>(), std::forward<decltype(callback)>(callback), flags);
//}
//
//void manapi::net::worker::TCP::event_unsub(const manapi::net::worker::shared_conn &conn) MANAPICORE_NOEXCEPT {
//    prepared::event_unsub( this, conn, conn->as<connection_prepared_base_t>() );
//}
//
//int manapi::net::worker::TCP::event_flags(const shared_conn & conn, int flags) MANAPICORE_NOEXCEPT {
//    auto const data = conn->as<tcp_connection_t>();
//
//    MANAPICORE_WORKER_EVENT_LOOP(data) {
//        if (data->flags & ev::DISCONNECT) {
//            if (!data->ev_cb.empty())
//                this->notify_cb(data->ev_cb.back().first, conn, CONN_CLOSED, nullptr);
//        } else {
//            if (status & ev::READ) {
//                this->flush_read(conn, data);
//            }
//        }
//
//        MANAPICORE_WORKER_EVENT_BREAK(data)
//    }
//
//    return prev;
//}
//
//int manapi::net::worker::TCP::event_flags(const shared_conn & conn) MANAPICORE_NOEXCEPT {
//    return prepared::event_flags(conn);
//}
//
//void manapi::net::worker::TCP::try_read_start(const shared_conn &conn, tcp_connection_t *data) MANAPICORE_NOEXCEPT {
//    if (!data->watcher || data->watcher->is_active() || data->transfered > this->m_config->speed_limit_rate || data->top->recv.size() > this->m_config->max_buffer_size) {
//        return;
//    }
//    manapi_log_trace(debug::LOG_TRACE_LOW, "TCP:%p try_read_start()", conn.get());
//    auto res = data->watcher->read_start();
//    if (res) manapi_log_error("%s failed due to %s", "tcp:read_start", ev::strerror(res));
//}
//
//void manapi::net::worker::TCP::read_stop(const shared_conn &conn, tcp_connection_t *data) MANAPICORE_NOEXCEPT {
//    if (!data->watcher || !data->watcher->is_active()) {
//        return;
//    }
//    manapi_log_trace(debug::LOG_TRACE_LOW, "TCP:%p read_stop()", conn.get());
//    auto res = data->watcher->read_stop();
//    if (res) manapi_log_error("%s failed due to %s", "tcp:read_stop", ev::strerror(res));
//}
//
//int manapi::net::worker::TCP::flush_write(const worker::shared_conn &connection, bool flush) MANAPICORE_NOEXCEPT {
//    auto conn = connection->as<tcp_connection_t>();
//
//    if (!flush) flush = (conn->flags & CONN_SEND_END);
//
//    try {
//        if (conn->top) {
//            while ((conn->top->send.size() >= this->m_config->batch_size) || (flush && conn->top->send.slices_size())) {
//                ssize_t rhs;
//
//                if (!conn->top->pending_send) {
//#ifdef _MSC_VER
//                    ev::buff_t *s = static_cast<ev::buff_t*>(alloca(sizeof (ev::buff_t) * conn->top->send.slices_size()));
//#else
//                    ev::buff_t s[conn->top->send.slices_size()];
//#endif
//
//                    conn->top->send.slices_buffs(s, conn->top->send.slices_size());
//
//                    rhs = conn->watcher->try_write(s, conn->top->send.slices_size());
//                } else {
//                    rhs = 0;
//                }
//
//                if (rhs < 0) {
//                    if (rhs == ev::ERR_AGAIN) {
//                        rhs = 0;
//                    } else {
//                        manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "%s:%s failed due to %s",
//                                         "TCP", "try_write", ev::strerror(static_cast<int>(rhs)));
//
//                        return CONN_IO_ERROR;
//                    }
//                }
//
//                conn->top->send.shift_add( static_cast<std::size_t> (rhs) ).unwrap();
//
//                if ( ( conn->top->send.size() >= this->m_config->batch_size ) || ( !conn->top->send.empty() && flush ) ) {
//
//                    auto buffs_cnt = static_cast<uint32_t> ( conn->top->send.slices_size() );
//                    auto buffs = conn->top->send.slices_buffs( buffs_cnt );
//                    auto const buffs_ptr = buffs.get();
//                    auto pending_send = conn->top->send.size();
//
//                    manapi::async::eventloop()->create_watcher_write(conn->watcher.get(),
//                       [connection, send_sv = std::move(conn->top->send), buffs = std::move(buffs) ]
//                               (const ev::shared_write &w, int status) mutable -> void {
//
//                           auto conn = connection->as<tcp_connection_t>();
//                           auto wrk = dynamic_cast<TCP*> (conn->worker);
//
//                           conn->top->pending_send -= send_sv.size();
//
//                           if (status) {
//                               /* error */
//                               manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "write failed msg=%s %p conn",
//                                                ev::strerror(status), connection.get());
//                               conn->worker->close_connection(connection, CLOSE_CONN_EOS);
//                               return;
//                           }
//
//                           manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "TCP:%zu bytes were sent %p. now=%zu",
//                                            send_sv.size(), connection.get(), conn->top->pending_send);
//
//                           if (conn->flags & ev::DISCONNECT) {
//                               conn->worker->close_connection(connection, CLOSE_CONN_SHUTDOWN);
//                               return;
//                           }
//
//                           if (conn->flags & ev::WRITE) {
//                               assert (!conn->ev_cb.empty());
//                               if (wrk->notify_cb(conn->ev_cb.back().first, connection, ev::WRITE, nullptr)) {
//                                   conn->worker->close_connection(connection, CLOSE_CONN_ERR);
//                                   return;
//                               }
//                           }
//
//                           if (wrk->conn_after_write(connection)) {
//                               conn->worker->close_connection(connection, CLOSE_CONN_ERR);
//                               return;
//                           }
//                       }, buffs_ptr, buffs_cnt).unwrap();
//
//                    conn->top->pending_send += pending_send;
//                }
//            }
//        }
//
//        return CONN_IO_OK;
//    }
//    catch (std::bad_alloc const &) {
//        return CONN_IO_ERROR;
//    }
//    catch (std::exception const &e) {
//        manapi_log_error("%s failed due to %s", "flush_write", e.what());
//    }
//    return CONN_IO_ERROR;
//}
//
//void manapi::net::worker::TCP::flush_read(const shared_conn &conn, tcp_connection_t *data) MANAPICORE_NOEXCEPT {
//    prepared::flush_read(this, conn, data);
//    this->try_read_start(conn, data);
//}
//
//void manapi::net::worker::TCP::update_limit_rate() MANAPICORE_NOEXCEPT {
//    /* in the event loop */
//    for (auto it = this->m_ips.begin(); it != this->m_ips.end(); ) {
//        if (it->second.empty()) {
//            it = this->m_ips.erase(it);
//            continue;
//        }
//
//        for (auto nit = it->second.begin(); nit != it->second.end();) {
//            if (!nit->second) {
//                nit = it->second.erase(nit);
//                continue;
//            }
//
//            auto conn = nit->second;
//            this->update_limit_rate_connection(conn);
//            ++nit;
//        }
//
//        ++it;
//    }
//}
//
//void manapi::net::worker::TCP::on_timeout(shared_conn conn) MANAPICORE_NOEXCEPT {
//    auto const data = conn->as<tcp_connection_t>();
//
//    prepared::timer_clear(std::move(data->t));
//
//    if (conn->wrk.flags & WRK_INTERFACE_TCP_KEEP_ALIVE)
//        conn->wrk.flags ^= WRK_INTERFACE_TCP_KEEP_ALIVE;
//
//    this->close_connection(conn, CLOSE_CONN_SHUTDOWN);
//}
//
//void manapi::net::worker::TCP::update_limit_rate_connection(const shared_conn &sconn) MANAPICORE_NOEXCEPT {
//    prepared::update_limit_rate_connection(sconn, sconn->as<connection_prepared_base_t>(), this, &this->m_global);
//}
//
//bool manapi::net::worker::TCP::is_writable(const shared_conn &conn) MANAPICORE_NOEXCEPT {
//    auto const data = conn->as<tcp_connection_t>();
//    return prepared::is_writable (this->m_config.get(), data);
//}
//
//manapi::net::worker::shared_conn manapi::net::worker::TCP::connection_init_cb(void *user_data) MANAPICORE_NOEXCEPT {
//    try {
//        auto p = std::make_unique<tcp_connection_t>();
//
//        return reference (new worker::connection(p.release(),  connection_interface_eraser));
//    }
//    catch (std::exception const &) {
//        return nullptr;
//    }
//}
//
//void manapi::net::worker::TCP::connection_interface_eraser(worker::connection *ptr) MANAPICORE_NOEXCEPT {
//    if (!ptr)
//        return;
//
//    auto connection = std::unique_ptr<tcp_connection_t> (ptr->as<tcp_connection_t>());
//
//    if (connection->watcher) {
//        if (connection->watcher->is_active()) {
//            manapi_log_trace(debug::LOG_TRACE_LOW, "TCP:%p read_stop()", connection.get());
//            connection->watcher->read_stop();
//        }
//
//        manapi::async::current()->eventloop()->stop_watcher(std::move(connection->watcher));
//    }
//
//    manapi_log_trace(debug::LOG_TRACE_MEDIUM, "TCP:Free TCP %p conn", connection.get());
//
//    auto const wrk = dynamic_cast<TCP*> (connection->worker);
//    assert (wrk);
//    if (wrk->m_global.cleanup_cb(ptr, &wrk->m_global))
//        manapi_log_trace("tcp:this->m_global.cleanup_cb failed");
//
//    wrk->m_worker_data->count.fetch_sub(1);
//    wrk->m_token.unref();
//}
//
//int manapi::net::worker::TCP::init_conn(const worker::shared_conn &conn) MANAPICORE_NOEXCEPT {
//    if (this->m_global.init_cb(conn, &this->m_global, this))
//        return -1;
//    return 0;
//}
//
//
//int manapi::net::worker::TCP::conn_after_write(const worker::shared_conn &conn) MANAPICORE_NOEXCEPT {
//    auto cdata = conn->as<tcp_connection_t>();
//
//    if ((cdata->flags & CONN_SEND_END) && (cdata->flags & CONN_RECV_END)) {
//        manapi_log_trace(manapi::debug::LOG_TRACE_LOW, "TCP:data sent after closing conn %p", conn.get());
//        cdata->worker->close_connection(conn, CLOSE_CONN_FINISHED);
//    }
//
//    return manapi::ERR_OK;
//}
