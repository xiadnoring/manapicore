#pragma once

#include "../ManapiUtils.hpp"
#include "./ManapiTlsOverTcp.hpp"

#if MANAPIHTTP_OPENSSL_DEPENDENCY

namespace manapi::net::worker {
    class OpenSSL_TLS : public worker::TLS {
    public:
        OpenSSL_TLS ();

        ~OpenSSL_TLS ();

        static std::shared_ptr<worker::OpenSSL_TLS> create ();

        void stop(manapi::stoken token) override;

        manapi::future<manapi::status> init(std::size_t deep) override;
    protected:
        bool ssl_is_init_fininshed (void *ssl) MANAPIHTTP_NOEXCEPT override;

        int ssl_get_error (void *ssl, int rhs) MANAPIHTTP_NOEXCEPT override;

        int ssl_accept (void *ssl) MANAPIHTTP_NOEXCEPT override;

        void *ssl_new (void *ctx) MANAPIHTTP_NOEXCEPT override;

        int ssl_write (void *ssl, const void *buff, int size) MANAPIHTTP_NOEXCEPT override;

        int ssl_read (void *ssl, void *buff, int size) MANAPIHTTP_NOEXCEPT override;

        int ssl_shutdown (void *ssl) MANAPIHTTP_NOEXCEPT override;

        void ssl_set_shutdown(void *ssl, int flags) MANAPIHTTP_NOEXCEPT override;

        void ssl_free(void *ssl) MANAPIHTTP_NOEXCEPT override;

        int ssl_bio_read(void *wbio, void *buff, int size) MANAPIHTTP_NOEXCEPT override;

        int ssl_bio_write(void *rbio, const void *buff, int size) MANAPIHTTP_NOEXCEPT override;

        int ssl_bio_should_retry(void *bio) MANAPIHTTP_NOEXCEPT override;

        int ssl_read_early_data(void *ssl, void *buf, std::size_t num, std::size_t *readbytes) MANAPIHTTP_NOEXCEPT override;

        int ssl_write_early_data(void *ssl, const void *buf, std::size_t num, std::size_t *readbytes) MANAPIHTTP_NOEXCEPT override;

        bool ssl_early_data_is_enabled(void *ctx) MANAPIHTTP_NOEXCEPT override;

        int recv_setup_connection(const shared_conn &conn, tls_connection_t *storage) override;

        manapi::status_or<void*> ssl_create_context (size_t version);

        manapi::status ssl_configure_context (void* ctx);
    private:
        std::shared_ptr<net::worker::pool_t> m_pool_data;
    };
}
#endif