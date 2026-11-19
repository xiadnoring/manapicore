#pragma once
//
//#include "../ManapiUtils.hpp"
//#include "./ManapiBaseWorker.hpp"
//#include "./ManapiTcp.hpp"
//
//namespace manapi::net::worker {
//    struct tls_connection_t;
//
//    class TLS : public worker::TCP {
//    public:
//
//        TLS ();
//
//        ~TLS () override;
//
//        manapi::future<status> init (std::size_t deep) override;
//
//        shared_conn accept (const ev::shared_tcp &w) MANAPICORE_NOEXCEPT override;
//
//        void close_connection(shared_conn conn, int flags) MANAPICORE_NOEXCEPT override;
//
//        ssize_t sync_write_ex(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool flush, std::size_t maxsize) MANAPICORE_NOEXCEPT override;
//
//        ssize_t sync_write_ex(const shared_conn &conn, manapi::slice &sv, bool flush, std::size_t maxsize) MANAPICORE_NOEXCEPT override;
//
//        ssize_t sync_write(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool flush) MANAPICORE_NOEXCEPT override;
//
//        int event_flags (const shared_conn & conn, int flags) MANAPICORE_NOEXCEPT override;
//    protected:
//        char    m_ssl_error_none,
//                m_ssl_error_want_read,
//                m_ssl_error_want_write,
//                m_ssl_error_zero_return,
//                m_ssl_error_ssl,
//                m_ssl_error_syscall,
//                m_ssl_send_shutdown,
//                m_ssl_recv_shutdown;
//        char
//                m_early_data_read_finish,
//                m_early_data_read_error,
//                m_early_data_read_success;
//        char
//                m_ssl_shutdown_success,
//                m_ssl_shutdown_fatal_error,
//                m_ssl_shutdown_not_done;
//
//        void *m_ctx;
//
//        int m_ssl_session_ctx_id;
//
//        virtual bool ssl_is_init_fininshed (void *ssl) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_get_error (void *ssl, int rhs) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_accept (void *ssl) MANAPICORE_NOEXCEPT = 0;
//
//        virtual void *ssl_new (void *ctx) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_write (void *ssl, const void *buff, int size) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_read (void *ssl, void *buff, int size) MANAPICORE_NOEXCEPT = 0;
//
//        virtual bool ssl_early_data_is_enabled (void *ctx) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_write_early_data (void *ssl, const void *buf, std::size_t num, std::size_t *readbytes) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_read_early_data (void *ssl, void *buf, std::size_t num, std::size_t *readbytes) MANAPICORE_NOEXCEPT = 0;
//        /**
//         *
//         * @param ssl SSL connection
//         * @return OK: 1; ERR: not 1
//         */
//
//        virtual int ssl_shutdown (void *ssl) MANAPICORE_NOEXCEPT = 0;
//
//        /**
//         *
//         * @param ssl SSL connection
//         * @param flags sent/recv
//         */
//
//        virtual void ssl_set_shutdown(void *ssl, int flags) MANAPICORE_NOEXCEPT = 0;
//
//        virtual void ssl_free(void *ssl) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_bio_write (void *rbio, const void *buff, int size) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_bio_read (void *wbio, void *buff, int size) MANAPICORE_NOEXCEPT = 0;
//
//        virtual std::size_t ssl_bio_ctrl_pending (void *wbio) MANAPICORE_NOEXCEPT = 0;
//
//        virtual std::size_t ssl_pending (void *ssl) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int ssl_bio_should_retry (void *bio) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int recv_setup_connection (const shared_conn &conn, tls_connection_t *storage) = 0;
//
//        static void connection_interface_eraser (worker::connection *data) MANAPICORE_NOEXCEPT;
//
//        void shutdown_async (shared_conn conn);
//
//        void onrecv(const std::shared_ptr<ev::tcp> &watcher, const shared_conn &conn, ibuffpool_t &&buffer) MANAPICORE_NOEXCEPT override;
//
//        int conn_after_write(const worker::shared_conn &conn) MANAPICORE_NOEXCEPT override;
//    private:
//        static shared_conn connection_init_cb (void *user_data) MANAPICORE_NOEXCEPT;
//
//        int manapi_do_process (const shared_conn &conn, tls_connection_t *data);
//
//        int manapi_do_handshake (const shared_conn &conn, tls_connection_t *data);
//
//        int ssl_bio_flush_write (const shared_conn &conn, tls_connection_t *m, std::size_t maxsize, bool fin) MANAPICORE_NOEXCEPT;
//
//        int ssl_bio_flush_read (const shared_conn &conn, tls_connection_t *m, std::size_t maxsize);
//    };
//}