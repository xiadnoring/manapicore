#include <iostream>
#include <memory>

#include "ManapiInitTools.hpp"


#include "ManapiEventLoop.hpp"
#include "ManapiDebug.hpp"
#include "ManapiProcess.hpp"
#include "./include/ManapiDebug.hpp"

#if MANAPIHTTP_GRPC_DEPENDENCY
#   include "ManapiGrpc.hpp"
#   include "./include/ManapiInternalGrpc.hpp"
#   include <google/protobuf/any.h>
#endif

#if MANAPIHTTP_OPENSSL_DEPENDENCY
#   include <openssl/ssl.h>
#   include <openssl/rand.h>
#   include <openssl/err.h>
#   include <openssl/bio.h>
#   include <openssl/engine.h>
#endif

#if MANAPIHTTP_CURL_DEPENDENCY
#   include <curl/curl.h>
#endif

void manapi::init_tools::ssl_library_init() {
#if MANAPIHTTP_OPENSSL_DEPENDENCY
        OpenSSL_add_ssl_algorithms();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();
        RAND_poll();
#endif
}

void manapi::init_tools::ev_library_init() {

}

void manapi::init_tools::curl_library_init() {
#if MANAPIHTTP_CURL_DEPENDENCY
    curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
}

void manapi::init_tools::log_trace_init(manapi::debug::trace_level lvl) {
    manapi::debug::set_log_trace_enabled(static_cast<int> (lvl));
}

void manapi::init_tools::log_name_enable(const char *name, bool enable) {
    manapi::debug::set_log_name_enabled(name, enable);
}

void manapi::init_tools::grpc_library_init() {
#if MANAPIHTTP_GRPC_DEPENDENCY
//    manapi::net::wgrpc::event_engine_wrapper::init_instance();
#endif
}

void manapi::clear_tools::clear_all() MANAPIHTTP_NOEXCEPT {
    clear_tools::curl_library_clear();
    clear_tools::ssl_library_clear();
    clear_tools::grpc_clear();
    clear_tools::ev_library_clear();
}

void manapi::clear_tools::clear_thread_all() MANAPIHTTP_NOEXCEPT {
    clear_tools::ssl_library_thread_clear();
    clear_tools::grpc_thread_clear();
}

void manapi::clear_tools::grpc_clear() MANAPIHTTP_NOEXCEPT {
    clear_tools::grpc_thread_clear();
#if MANAPIHTTP_GRPC_DEPENDENCY
//    google::protobuf::ShutdownProtobufLibrary();
//    manapi::net::wgrpc::event_engine_wrapper::get_instance() = nullptr;
#endif
}

void manapi::clear_tools::grpc_thread_clear() MANAPIHTTP_NOEXCEPT {
#if MANAPIHTTP_GRPC_DEPENDENCY
//    net::wgrpc::server_ctx::clean();
#endif
}

void manapi::clear_tools::ssl_library_thread_clear() MANAPIHTTP_NOEXCEPT {
#if MANAPIHTTP_OPENSSL_DEPENDENCY
    OPENSSL_thread_stop();
#endif
}

void manapi::clear_tools::ssl_library_clear() MANAPIHTTP_NOEXCEPT {
#if MANAPIHTTP_OPENSSL_DEPENDENCY
    OPENSSL_thread_stop();
    OPENSSL_cleanup();
    ENGINE_cleanup();
    CONF_modules_unload(1);
    ERR_free_strings();
    EVP_cleanup();
    sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
    //SSL_COMP_free_compression_methods();
    CRYPTO_cleanup_all_ex_data();
#endif
}

void manapi::clear_tools::ev_library_clear() MANAPIHTTP_NOEXCEPT {

}

void manapi::clear_tools::curl_library_clear() MANAPIHTTP_NOEXCEPT {
#if MANAPIHTTP_CURL_DEPENDENCY
    curl_global_cleanup();
#endif
}
