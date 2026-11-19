//#include "worker/ManapiInterfaceWorker.hpp"
//#include "../include/ManapiUtils.hpp"
//
//manapi::net::worker::interface_worker::interface_worker() {
//    this->m_deep_worker_id = 0;
//    this->m_flags = 0;
//    this->m_worker_pool_id = 0;
//}
//
//manapi::net::worker::interface_worker::~interface_worker() {
//    if (this->m_global.cleanup_global_cb) {
//        this->m_global.cleanup_global_cb(&this->m_global, this);
//    }
//}
//
//void manapi::net::worker::interface_worker::wrk_global(wrk_interface_global_t *data) MANAPICORE_NOEXCEPT {
//    this->m_global = *data;
//}
//
//manapi::net::worker::wrk_interface_global_t * manapi::net::worker::interface_worker::wrk_global() MANAPICORE_NOEXCEPT {
//    return &this->m_global;
//}
//
//void manapi::net::worker::interface_worker::worker_pool_id(std::size_t worker_pool_id) MANAPICORE_NOEXCEPT {
//    this->m_worker_pool_id = worker_pool_id;
//}
//
//std::size_t manapi::net::worker::interface_worker::worker_pool_id() const MANAPICORE_NOEXCEPT {
//    return this->m_worker_pool_id;
//}
//
//std::size_t manapi::net::worker::interface_worker::deep_worker_id() const MANAPICORE_NOEXCEPT {
//    return this->m_deep_worker_id;
//}
//
//manapi::future<manapi::status> manapi::net::worker::interface_worker::init(std::size_t deep) {
//    this->m_token = manapi::stoken (nullptr);
//    co_return manapi::status_ok();
//}
//
//void manapi::net::worker::interface_worker::stop(manapi::stoken token) {
//    this->m_token.next( std::move( token ) );
//    this->m_token.unref();
//}
//
//void manapi::net::worker::interface_worker::setup(std::shared_ptr<net::worker::base_http> site,
//                                                  std::shared_ptr<manapi::net::worker::worker_data_t> worker_data,
//                                                  std::shared_ptr<manapi::net::http::config> config) {
//    this->m_config = std::move(config);
//    this->m_worker_data = std::move(worker_data);
//    this->m_site = std::move(site);
//}
//
