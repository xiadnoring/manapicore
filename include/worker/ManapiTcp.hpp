#pragma once

//#include "../ManapiUtils.hpp"
//#include "./ManapiBaseWorker.hpp"
//#include "./ManapiInterfaceWorker.hpp"
//#include "./ManapiAsync.hpp"
//#include "./ManapiTimerObject.hpp"
//
//namespace manapi::net::worker {
//    struct tcp_connection_t;
//
//    class TCP : public worker::interface_worker {
//    public:
//        TCP ();
//
//        ~TCP () override;
//
//        manapi::future<status> init (std::size_t deep) override;
//
//        void waiting(const shared_conn &conn, bool state) MANAPICORE_NOEXCEPT override;
//
//        void onaccept(const std::shared_ptr<ev::tcp> &watcher, int status) MANAPICORE_NOEXCEPT;
//
//        virtual void onrecv (const std::shared_ptr<ev::tcp> &watcher, const worker::shared_conn &conn) MANAPICORE_NOEXCEPT;
//
//        static std::shared_ptr<worker::TCP> create ();
//
//        virtual shared_conn accept (const ev::shared_tcp &w, shared_conn (*init_cb) (void *user_data), void *user_data) MANAPICORE_NOEXCEPT;
//
//        virtual shared_conn accept (const ev::shared_tcp &w) MANAPICORE_NOEXCEPT;
//
//        void close_connection(shared_conn conn, int flags) MANAPICORE_NOEXCEPT override;
//
//        void stop(manapi::stoken token) override;
//
//        void feed_event (const shared_conn &conn, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT override;
//
//        ssize_t sync_write_ex(const worker::shared_conn &conn, manapi::slice &sv, bool flush, std::size_t maxsize) MANAPICORE_NOEXCEPT override;
//
//        ssize_t sync_write_ex(const worker::shared_conn &conn, ev::buff_t *buf, uint32_t buf_cnt, bool flush, std::size_t maxsize) MANAPICORE_NOEXCEPT override;
//
//        ssize_t sync_write(const worker::shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool flush) MANAPICORE_NOEXCEPT override;
//
//        void event_on (const shared_conn & conn, worker_watcher_cb&& callback, int flags) MANAPICORE_NOEXCEPT override;
//
//        void event_unsub(const manapi::net::worker::shared_conn &conn) MANAPICORE_NOEXCEPT override;
//
//        int event_flags(const shared_conn & conn, int flags) MANAPICORE_NOEXCEPT override;
//
//        int event_flags(const shared_conn & conn) MANAPICORE_NOEXCEPT override;
//    protected:
//        virtual void try_read_start (const shared_conn &conn, tcp_connection_t *data) MANAPICORE_NOEXCEPT;
//
//        virtual void read_stop (const shared_conn &conn, tcp_connection_t *data) MANAPICORE_NOEXCEPT;
//
//        virtual int flush_write (const shared_conn &connection, bool flush = false) MANAPICORE_NOEXCEPT;
//
//        void flush_read (const shared_conn &conn, tcp_connection_t *data) MANAPICORE_NOEXCEPT;
//
//        void update_limit_rate () MANAPICORE_NOEXCEPT;
//
//        void on_timeout (shared_conn conn) MANAPICORE_NOEXCEPT;
//
//        virtual void update_limit_rate_connection (const shared_conn &sconn) MANAPICORE_NOEXCEPT;
//
//        bool is_writable(const shared_conn &conn) MANAPICORE_NOEXCEPT override;
//
//        int init_conn (const worker::shared_conn &conn) MANAPICORE_NOEXCEPT;
//
//        virtual int conn_after_write (const worker::shared_conn &conn) MANAPICORE_NOEXCEPT;
//
//        ev::shared_tcp m_watcher_accept;
//
//        conns_by_ip m_ips;
//    private:
//        static shared_conn connection_init_cb (void *user_data) MANAPICORE_NOEXCEPT;
//
//        static void connection_interface_eraser (worker::connection *ptr) MANAPICORE_NOEXCEPT;
//
//        sockaddr_storage m_sockaddrin;
//
//        addrinfo *m_local;
//
//        timer m_limit_rate_timer;
//    };
//}
