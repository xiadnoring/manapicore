#pragma once
//
//#include "../ManapiUtils.hpp"
//#include "./ManapiTlsOverTcp.hpp"
//
//#if MANAPIHTTP_WOLFSSL_DEPENDENCY
//
//namespace manapi::net::worker {
//    class WolfSSL_TLS : public worker::TLS {
//    public:
//        WolfSSL_TLS (std::shared_ptr<net::worker::base_http> site, manapi::net::worker::worker_data_t* wdata, manapi::net::http::config *config);
//
//        ~WolfSSL_TLS () override;
//
//        manapi::future<manapi::status> init(std::size_t deep) override;
//
//        static std::shared_ptr<worker::WolfSSL_TLS> create (std::shared_ptr<net::worker::base_http> site, manapi::net::worker::worker_data_t* wdata, manapi::net::http::config* config);
//
//        void stop(manapi::stoken token) override;
//    protected:
//
//        bool ssl_is_init_fininshed (void *ssl) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_get_error (void *ssl, int rhs) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_accept (void *ssl) MANAPIHTTP_NOEXCEPT override;
//
//        void *ssl_new (void *ctx) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_write (void *ssl, const void *buff, int size) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_read (void *ssl, void *buff, int size) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_shutdown (void *ssl) MANAPIHTTP_NOEXCEPT override;
//
//        void ssl_set_shutdown(void *ssl, int flags) MANAPIHTTP_NOEXCEPT override;
//
//        void ssl_free(void *ssl) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_bio_read(void *wbio, void *buff, int size) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_bio_should_retry(void *bio) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_bio_write(void *rbio, const void *buff, int size) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_read_early_data(void *ssl, void *buf, std::size_t num, std::size_t *readbytes) MANAPIHTTP_NOEXCEPT override;
//
//        int ssl_write_early_data(void *ssl, const void *buf, std::size_t num, std::size_t *readbytes) MANAPIHTTP_NOEXCEPT override ;
//
//        bool ssl_early_data_is_enabled(void *ctx) MANAPIHTTP_NOEXCEPT override;
//
//        int recv_setup_connection(const shared_conn &conn, tls_connection_t *data) MANAPIHTTP_NOEXCEPT override;
//
//        manapi::status_or<void *> ssl_create_context (size_t version) MANAPIHTTP_NOEXCEPT;
//
//        manapi::status ssl_configure_context (void *ctx, worker::pool_t *pool_data, std::size_t deeplvl) MANAPIHTTP_NOEXCEPT;
//    private:
//        std::shared_ptr<net::worker::pool_t> pool_data_;
//    };
//}
//#endif