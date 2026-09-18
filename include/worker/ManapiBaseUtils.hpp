#pragma once

//#include "./ManapiInterfaceWorker.hpp"
//#include "./ManapiBaseWorker.hpp"
//#include "./ManapiTimerObject.hpp"
//
//#define MANAPIHTTP_WORKER_EVENT_LOOP(n__) n__->speed_min_delay = (this->m_config->speed_check_delay); \
//auto const prev = std::exchange(n__->flags, ((n__->flags & (~CONN_MASK_UPDATE))) | (flags & CONN_MASK_UPDATE)); \
//if (n__->flags & CONN_EVENT_LOCKED) return prev; \
//n__->flags |= CONN_EVENT_LOCKED; int status; if (n__->flags & CONN_EVENT_FORCE) { status = (n__->flags); n__->flags ^= CONN_EVENT_FORCE; } \
//else { status = (n__->flags & ~prev); } int flags_before__ = n__->flags; \
//while (true)
//#define MANAPIHTTP_WORKER_EVENT_LOOP_STREAM(n__) n__->speed_min_delay = (this->m_config->speed_stream_check_delay); \
//auto const prev = std::exchange(n__->flags, ((n__->flags & (~CONN_MASK_UPDATE))) | (flags & CONN_MASK_UPDATE)); \
//if (n__->flags & CONN_EVENT_LOCKED) return prev; \
//n__->flags |= CONN_EVENT_LOCKED; int status; if (n__->flags & CONN_EVENT_FORCE) { status = (n__->flags); n__->flags ^= CONN_EVENT_FORCE; } \
//else { status = (n__->flags & ~prev); } int flags_before__ = n__->flags; \
//while (true)
//#define MANAPIHTTP_WORKER_EVENT_BREAK(n__) if (flags_before__ != n__->flags) { status = n__->flags; continue; }\
//assert (n__->flags & CONN_EVENT_LOCKED); n__->flags ^= CONN_EVENT_LOCKED;  break;
//
//namespace manapi::net::worker {
//    struct connection_base_t {
//        std::size_t transfered;
//        std::size_t transfered_k;
//    };
//
//    struct connection_base2_t : connection_base_t {
//        int flags;
//        uint32_t speed_min_delay;
//    };
//
//    struct connection_prepared_base_t : connection_base2_t {
//        std::vector<std::pair<manapi::net::worker::worker_watcher_cb, int>> ev_cb;
//    };
//
//    struct connection_prepared_t : connection_prepared_base_t {
//        std::unique_ptr<struct connection_io> top;
//    };
//
//    struct tcp_connection_t : connection_prepared_t {
//        manapi::timer t;
//        worker::base *worker;
//        std::shared_ptr<ev::tcp> watcher;
//    };
//
//    struct tls_connection_t : tcp_connection_t {
//        void *ssl;
//        manapi::timer accept_timer;
//        void *rbio;
//        void *wbio;
//    };
//
//    namespace prepared {
//        int event_flags (const shared_conn &conn) MANAPIHTTP_NOEXCEPT;
//
//        int event_flags (const shared_conn &conn, connection_prepared_base_t *data) MANAPIHTTP_NOEXCEPT;
//
//        ssize_t sync_write(interface_worker *w, const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) MANAPIHTTP_NOEXCEPT;
//
//        ssize_t sync_write(interface_worker *w, const shared_conn &conn, connection_prepared_base_t *connection, ev::buff_t *buff, uint32_t nbuff, std::size_t limit_rate, bool finish) MANAPIHTTP_NOEXCEPT;
//
//        void waiting(const shared_conn &conn, connection_base2_t *data, bool state) MANAPIHTTP_NOEXCEPT;
//
//        void waiting(const shared_conn &conn, bool state) MANAPIHTTP_NOEXCEPT;
//
//        int flush_read (worker::interface_worker *w, const shared_conn &conn, connection_prepared_t *data) MANAPIHTTP_NOEXCEPT;
//
//        bool is_writable (http::config *config, connection_prepared_t *data) MANAPIHTTP_NOEXCEPT;
//
//        void top_buffer_clear (connection_prepared_t *s) MANAPIHTTP_NOEXCEPT;
//
//        void event_callback_clear (worker::interface_worker *w, const shared_conn &conn, connection_prepared_base_t *s) MANAPIHTTP_NOEXCEPT;
//
//        void timer_clear (manapi::timer &&t) MANAPIHTTP_NOEXCEPT;
//
//        void feed_event (worker::interface_worker *w, const shared_conn &conn, connection_prepared_t *data, int flags, manapi::slice_complex *sc) MANAPIHTTP_NOEXCEPT;
//
//        void feed_event (worker::interface_worker *w, const shared_conn &conn, int flags, manapi::slice_complex *sc) MANAPIHTTP_NOEXCEPT;
//
//        void update_limit_rate_connection (const shared_conn &sconn, connection_prepared_base_t *data, worker::interface_worker *w, wrk_interface_global_t *global) MANAPIHTTP_NOEXCEPT;
//
//        void event_on (worker::interface_worker *w, const shared_conn &conn, connection_prepared_base_t *data, worker_watcher_cb &&callback, int flags) MANAPIHTTP_NOEXCEPT;
//
//        bool event_unsub (worker::interface_worker *w, const shared_conn &conn, connection_prepared_base_t *data) MANAPIHTTP_NOEXCEPT;
//    }
//}