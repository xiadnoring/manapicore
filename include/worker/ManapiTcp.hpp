#pragma once

#include "../ManapiUtils.hpp"
#include "./ManapiBaseWorker.hpp"
#include "./ManapiInterfaceWorker.hpp"
#include "./ManapiAsync.hpp"
#include "./ManapiTimerObject.hpp"

namespace manapi::net::worker {
    struct tcp_connection_t;

    class TCP : public worker::interface_worker {
    public:
        TCP ();

        ~TCP () override;

        manapi::future<status> init (std::size_t deep) override;

        void waiting(const shared_conn &conn, bool state) MANAPIHTTP_NOEXCEPT override;

        void onaccept(const std::shared_ptr<ev::tcp> &watcher, int status) MANAPIHTTP_NOEXCEPT;

        virtual void onrecv (const std::shared_ptr<ev::tcp> &watcher, const worker::shared_conn &conn, ibuffpool_t &&buffer) MANAPIHTTP_NOEXCEPT;

        static std::shared_ptr<worker::TCP> create ();

        virtual shared_conn accept (const ev::shared_tcp &w, shared_conn (*init_cb) (void *user_data), void *user_data) MANAPIHTTP_NOEXCEPT;

        virtual shared_conn accept (const ev::shared_tcp &w) MANAPIHTTP_NOEXCEPT;

        void close_connection(shared_conn conn, int flags) MANAPIHTTP_NOEXCEPT override;

        void stop(manapi::stoken token) override;

        void feed_event (const shared_conn &conn, int flags, const char *buff, std::size_t size, ibuffpool_t *p) MANAPIHTTP_NOEXCEPT override;

        ssize_t sync_write_ex(const worker::shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, std::size_t size, bool finish, std::size_t maxcnt) MANAPIHTTP_NOEXCEPT override;

        ssize_t sync_write(const worker::shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) MANAPIHTTP_NOEXCEPT override;

        worker_watcher_cb event_on(const shared_conn & conn, worker_watcher_cb callback) MANAPIHTTP_NOEXCEPT override;

        int event_flags(const shared_conn & conn, int flags) MANAPIHTTP_NOEXCEPT override;

        int event_flags(const shared_conn & conn) MANAPIHTTP_NOEXCEPT override;

        MANAPIHTTP_NODISCARD std::size_t recv_count(const shared_conn &conn) const MANAPIHTTP_NOEXCEPT override;

        bytebuffer recv_first_buffer(const shared_conn &conn) MANAPIHTTP_NOEXCEPT override ;
    protected:
        virtual void read_start (const shared_conn &conn, tcp_connection_t *data) MANAPIHTTP_NOEXCEPT;

        virtual void read_stop (const shared_conn &conn, tcp_connection_t *data) MANAPIHTTP_NOEXCEPT;

        virtual int flush_write (const shared_conn &connection, bool flush = false) MANAPIHTTP_NOEXCEPT;

        void flush_read (const shared_conn &conn, tcp_connection_t *data) MANAPIHTTP_NOEXCEPT;

        void update_limit_rate () MANAPIHTTP_NOEXCEPT;

        void on_timeout (shared_conn conn) MANAPIHTTP_NOEXCEPT;

        virtual void update_limit_rate_connection (const shared_conn &sconn) MANAPIHTTP_NOEXCEPT;

        bool is_writable(const shared_conn &conn) MANAPIHTTP_NOEXCEPT override;

        virtual int on_accept (const worker::shared_conn &conn) MANAPIHTTP_NOEXCEPT;

        int on_reaccept (const worker::shared_conn &conn) MANAPIHTTP_NOEXCEPT;

        int on_accept_bind (const worker::shared_conn &conn) MANAPIHTTP_NOEXCEPT;

        virtual int conn_after_write (const worker::shared_conn &conn) MANAPIHTTP_NOEXCEPT;

        ev::shared_tcp m_watcher_accept;
    
        conns_by_ip m_ips;
    private:
        static shared_conn connection_init_cb (void *user_data) MANAPIHTTP_NOEXCEPT;

        static void connection_interface_eraser (worker::connection *ptr) MANAPIHTTP_NOEXCEPT;

        sockaddr_storage m_sockaddrin;

        addrinfo *m_local;

        timer m_limit_rate_timer;
    };
}
