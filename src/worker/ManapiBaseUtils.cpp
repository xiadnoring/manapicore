//#include "worker/ManapiBaseUtils.hpp"
//
//int manapi::net::worker::prepared::event_flags(const shared_conn &conn) MANAPICORE_NOEXCEPT {
//    return event_flags(conn, conn->as<connection_prepared_base_t>());
//}
//
//int manapi::net::worker::prepared::event_flags(const shared_conn &conn, connection_prepared_base_t *data) MANAPICORE_NOEXCEPT {
//    return (data->flags) & base::CONN_MASK_GETTING;
//}
//
//ssize_t manapi::net::worker::prepared::sync_write(interface_worker *w, const shared_conn &conn, ev::buff_t *buff,uint32_t nbuff, bool finish) MANAPICORE_NOEXCEPT {
//    auto const connection = conn->as<connection_prepared_base_t>();
//    if (w->m_config->speed_limit_rate < connection->transfered) return 0;
//    return w->sync_write_ex(conn, buff, nbuff, finish, std::min<std::size_t>(w->m_config->speed_limit_rate - connection->transfered, w->m_config->max_buffer_size));
//}
//
//ssize_t manapi::net::worker::prepared::sync_write(interface_worker *w, const shared_conn &conn, connection_prepared_base_t *connection, ev::buff_t *buff, uint32_t nbuff, std::size_t limit_rate, bool finish) MANAPICORE_NOEXCEPT {
//    limit_rate = std::min<std::size_t>(limit_rate, w->m_config->speed_limit_rate);
//    if (limit_rate < connection->transfered) return 0;
//    return w->sync_write_ex(conn, buff, nbuff, finish, std::min<std::size_t>(limit_rate - connection->transfered, w->m_config->max_buffer_size));
//}
//
//void manapi::net::worker::prepared::waiting(const shared_conn &conn, connection_base2_t *data, bool state) MANAPICORE_NOEXCEPT {
//
//    if (state)
//        data->flags |= base::CONN_IO_WAITING;
//
//    else if (data->flags & base::CONN_IO_WAITING)
//        data->flags ^= base::CONN_IO_WAITING;
//}
//
//void manapi::net::worker::prepared::waiting(const shared_conn &conn, bool state) MANAPICORE_NOEXCEPT {
//    auto const data = conn->as<connection_base2_t>();
//    return waiting(conn, data, state);
//}
//
//int manapi::net::worker::prepared::flush_read( worker::interface_worker *w, const shared_conn &conn, connection_prepared_t *data) MANAPICORE_NOEXCEPT {
//    if (!data->top->recv.empty() && (data->flags & ev::READ)) {
//        assert (!data->ev_cb.empty());
//        int flags = ev::READ;
//
//        if (data->flags & base::CONN_RECV_END)
//            flags |= base::CONN_RECV_END;
//
//        manapi::slice_complex sc ( std::move( data->top->recv ) );
//
//        if (w->notify_cb(data->ev_cb.back().first, conn, flags, &sc)) {
//            return ERR_ABORTED;
//        }
//    }
//
//    return ERR_OK;
//}
//
//bool manapi::net::worker::prepared::is_writable(http::config *config,connection_prepared_t *data) MANAPICORE_NOEXCEPT {
//    return data->top->send.size() < config->max_buffer_size && data->transfered < config->speed_limit_rate;
//}
//
//void manapi::net::worker::prepared::top_buffer_clear(connection_prepared_t *s) MANAPICORE_NOEXCEPT {
//    if (s->top) {
//        s->top->send.clear();
//        s->top->recv.clear();
//    }
//}
//
//void manapi::net::worker::prepared::event_callback_clear(worker::interface_worker *w, const shared_conn &conn,connection_prepared_base_t *s) MANAPICORE_NOEXCEPT {
//    if (!s->ev_cb.empty()) {
//        if ( w->notify_cb ( s->ev_cb.back().first, conn, ev::DISCONNECT, nullptr)) {
//            /* pass */
//        }
//
//        assert (s->ev_cb.empty());
//    }
//}
//
//void manapi::net::worker::prepared::timer_clear(manapi::timer &&t) MANAPICORE_NOEXCEPT {
//    if (t) {
//        t.stop();
//        t.clear();
//        t = nullptr;
//    }
//}
//
//void manapi::net::worker::prepared::feed_event(worker::interface_worker *w, const shared_conn &conn, connection_prepared_t *data, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT {
//    if (flags & base::CONN_RECV_END) data->flags |= base::CONN_RECV_END;
//
//    if (flags & base::CONN_SEND_END) data->flags |= base::CONN_SEND_END;
//
//    if (flags & ev::READ && sc) {
//        auto sv = sc->to_slice().get_slice();
//        if (flags & base::CONN_TOP_READ) {
//            sv->push_back( std::move(data->top->recv) );
//            data->top->recv = std::move(*sv);
//        } else {
//            data->top->recv.push_back( std::move( * sv ) );
//        }
//
//        if (prepared::flush_read(w, conn, data)) {
//            w->close_connection(conn, CLOSE_CONN_ERR);
//        }
//    } else if (flags & data->flags && !data->ev_cb.empty()) {
//        if (w->notify_cb(data->ev_cb.back().first, conn, flags, sc)) {
//            w->close_connection(conn, CLOSE_CONN_ERR);
//        }
//    }
//}
//
//void manapi::net::worker::prepared::feed_event(worker::interface_worker *w, const shared_conn &conn, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT {
//    return feed_event(w, conn, conn->as<connection_prepared_t>(), flags, sc);
//}
//
//void manapi::net::worker::prepared::update_limit_rate_connection(const shared_conn &sconn, connection_prepared_base_t *data, worker::interface_worker *w, wrk_interface_global_t *global) MANAPICORE_NOEXCEPT {
//    if (data->transfered >= w->m_config->speed_limit_rate) {
//        data->transfered = 0;
//
//        if (!(data->flags & ev::DISCONNECT) && data->flags & ev::WRITE) {
//            if (w->notify_cb (data->ev_cb.back().first, sconn, ev::WRITE, nullptr)) {
//                w->close_connection(sconn, CLOSE_CONN_EOS);
//                return;
//            }
//        }
//    }
//    else {
//        data->transfered_k += data->transfered;
//
//        if (--data->speed_min_delay <= 0) {
//            if (data->flags & (base::CONN_IO_WAITING)
//                && (data->transfered_k < w->m_config->speed_check_delay)) {
//                if (data->flags & base::CONN_WAS_SHUTDOWN) {
//                    data->speed_min_delay = (w->m_config->speed_check_delay);
//                    w->close_connection(sconn, CLOSE_CONN_EOS);
//                } else {
//                    data->speed_min_delay = w->m_config->max_shutdown_time;
//                    w->close_connection(sconn, CLOSE_CONN_SHUTDOWN);
//                    data->flags |= base::CONN_WAS_SHUTDOWN;
//                }
//            } else {
//                data->speed_min_delay = (w->m_config->speed_check_delay);
//            }
//
//            data->transfered_k = 0;
//        }
//
//        data->transfered = 0;
//    }
//
//    if (sconn->wrk.flags & WRK_INTERFACE_CUSTOM_RATE_LIMIT) {
//        assert(!!global->update_limit_rate_cb);
//        global->update_limit_rate_cb (sconn, global);
//    }
//}
//
//void manapi::net::worker::prepared::event_on(worker::interface_worker *w, const shared_conn &conn , connection_prepared_base_t *data,worker_watcher_cb &&callback, int flags) MANAPICORE_NOEXCEPT {
//    assert (!!callback);
//
//    if (!data->ev_cb.empty() && data->ev_cb.back().second & net::worker::EVENT_ON_AUTO_REMOVE) {
//        data->ev_cb.back().first = std::forward<decltype(callback)>(callback);
//        data->ev_cb.back().second = flags;
//    }
//    else {
//        data->ev_cb.emplace_back(std::forward<decltype(callback)>(callback), flags);
//    }
//
//    if ( data->flags & worker::base::CONN_EVENT_LOCKED)
//        data->flags |= worker::base::CONN_EVENT_FORCE;
//
//    w->event_flags ( conn, flags );
//}
//
//bool manapi::net::worker::prepared::event_unsub(manapi::net::worker::interface_worker *w, const manapi::net::worker::shared_conn &conn, manapi::net::worker::connection_prepared_base_t *data) MANAPICORE_NOEXCEPT {
//    if (data->ev_cb.empty()) return false;
//
//    data->ev_cb.pop_back();
//
//    if (data->ev_cb.empty()) {
//        w->event_flags ( conn, 0 );
//    }
//    else {
//
//        if ( data->flags & worker::base::CONN_EVENT_LOCKED)
//            data->flags |= worker::base::CONN_EVENT_FORCE;
//
//        w->event_flags ( conn, data->ev_cb.back().second );
//
//        return true;
//    }
//
//    return false;
//}
//
//
//
//
//
//
//
//
//
