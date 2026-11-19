#pragma once

#include "ManapiUtils.hpp"
#include "ManapiTimerObject.hpp"
#include "worker/ManapiUdp.hpp"

#if MANAPICORE_OPENSSL_DEPENDENCY

#   include <openssl/ssl.h>

#   if OPENSSL_VERSION_NUMBER >= ((3<<28)|(5<<20)|(0<<4)|0x0L) && false

#       define MANAPICORE_OPENSSL_QUIC_SUPPORT

namespace manapi::net::worker {
    class openssl_quic : public udp {
    protected:
        struct quic_conn_t;

        struct quic_stream_t;
    public:
        openssl_quic ();

        ~openssl_quic() override;

        static std::shared_ptr<worker::openssl_quic> create ();

        manapi::future<status> init(std::size_t deep) override;

        void stop(manapi::stoken token) override;

        void close_connection(shared_conn conn, int flags) MANAPICORE_NOEXCEPT override;

        int event_flags(const shared_conn &conn) MANAPICORE_NOEXCEPT override;

        int event_flags(const shared_conn &conn, int flags) MANAPICORE_NOEXCEPT override;

        worker_watcher_cb event_on(const shared_conn &conn, worker_watcher_cb callback) MANAPICORE_NOEXCEPT override;

        void feed_event(const shared_conn &conn, int flags, const char *buff, std::size_t size, ibuffpool_t *p)  MANAPICORE_NOEXCEPT override;

        bool is_writable(const shared_conn &conn) MANAPICORE_NOEXCEPT override;

        MANAPICORE_NODISCARD std::size_t recv_count(const shared_conn &conn) const MANAPICORE_NOEXCEPT override;

        bytebuffer recv_first_buffer(const shared_conn &conn) MANAPICORE_NOEXCEPT override;

        ssize_t sync_write(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) MANAPICORE_NOEXCEPT override;

        ssize_t sync_write_ex(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, std::size_t size, bool finish, std::size_t maxcnt) MANAPICORE_NOEXCEPT override;

        void waiting(const shared_conn &conn, bool state) MANAPICORE_NOEXCEPT override;

        std::string_view alpn_ossltest () MANAPICORE_NOEXCEPT;

        void alpn_ossltest (std::string test) MANAPICORE_NOEXCEPT;

        void close_stream(shared_conn s, int flags) MANAPICORE_NOEXCEPT;

        void rst_stream (shared_conn s) MANAPICORE_NOEXCEPT;

        connection::ipdata_t *ipdata(worker::connection *conn) MANAPICORE_NOEXCEPT override;

        status_or<shared_conn> new_stream(const shared_conn &conn, int flags) MANAPICORE_NOEXCEPT override;

        int64_t stream_id(const shared_conn &s) MANAPICORE_NOEXCEPT override;

        shared_conn stream_id(const shared_conn &conn, int64_t id) MANAPICORE_NOEXCEPT override;

        MANAPICORE_NODISCARD std::size_t streams_size(const shared_conn &conn) const MANAPICORE_NOEXCEPT override;
    protected:
        void bio_flush_write () MANAPICORE_NOEXCEPT;

        void remove_poll_id (std::size_t poll_id) MANAPICORE_NOEXCEPT;

        void flush_read (const shared_conn &conn, quic_stream_t *data) MANAPICORE_NOEXCEPT;

        void flush_write (const shared_conn &conn, quic_stream_t *data) MANAPICORE_NOEXCEPT;

        void update_limit_rate () MANAPICORE_NOEXCEPT;

        virtual void update_limit_rate_connection (const shared_conn &sconn) MANAPICORE_NOEXCEPT;

        static void stream_interface_eraser (worker::connection *n) MANAPICORE_NOEXCEPT;

        static void connection_interface_eraser (worker::connection *n) MANAPICORE_NOEXCEPT;

        static void timeout_event_cb (uv_timer_t *s) MANAPICORE_NOEXCEPT;

        void onrecv(const std::shared_ptr<ev::udp> &watcher, char *buff, ssize_t size, const sockaddr *addr, unsigned flags) override;

        static void io_unbind_cb (ev::handle *s) MANAPICORE_NOEXCEPT;

        manapi::status_or<shared_conn> stream_accept (const shared_conn &conn, SSL *stream, int flags) MANAPICORE_NOEXCEPT;

        void stream_processing (const shared_conn &s) MANAPICORE_NOEXCEPT;

        manapi::status_or<shared_conn> conn_accept (SSL *client, const sockaddr *addr) MANAPICORE_NOEXCEPT;

        void conn_processing (SSL *client) MANAPICORE_NOEXCEPT;

        int try_init_conn (const shared_conn &conn) MANAPICORE_NOEXCEPT;

        std::unordered_map <std::string, std::map<std::uintptr_t, shared_conn>, manapi::text_hash, std::equal_to<>> conns_;
    private:
        static manapi::status load_params (manapi::net::worker::openssl_quic *w, SSL_CTX *ctx, manapi::json sslconfig);

        static int select_alpn (SSL *ssl, const unsigned char **out, unsigned char *out_len, const unsigned char *in, unsigned int in_len, void *arg);

        std::size_t m_pending_writes = 0;
        BIO *m_rbio;
        BIO *m_wbio;
        std::shared_ptr<net::worker::pool_t> m_pool_data;
        std::vector<SSL_POLL_ITEM> m_polls;
        manapi::timer m_update_limit_timer;
        std::unique_ptr<ev::timer> m_t;
        std::string m_alpn_ossltest;
        void *m_ctx;
        void *m_listener;
        SSL_POLL_ITEM* m_current_poll;
        std::size_t m_current_poll_id;
    };
}
#   endif
#endif