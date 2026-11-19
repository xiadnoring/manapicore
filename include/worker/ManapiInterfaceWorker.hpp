#pragma once

#include "./ManapiBaseWorker.hpp"
//
//namespace manapi::net::worker {
//    class interface_worker : public worker::base {
//    public:
//        interface_worker ();
//
//        ~interface_worker () override;
//
//        void setup ( std::shared_ptr<net::worker::base_http> site, std::shared_ptr<manapi::net::worker::worker_data_t> worker_data, std::shared_ptr<manapi::net::http::config> config );
//
//        void wrk_global (wrk_interface_global_t *data) MANAPICORE_NOEXCEPT override;
//
//        wrk_interface_global_t *wrk_global () MANAPICORE_NOEXCEPT override;
//
//        MANAPICORE_NODISCARD int worker_flags () MANAPICORE_NOEXCEPT { return this->m_flags; }
//
//        void worker_pool_id (std::size_t worker_pool_id) MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD std::size_t worker_pool_id () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD std::size_t deep_worker_id () const MANAPICORE_NOEXCEPT;
//
//        manapi::future<status> init(std::size_t deep) override;
//
//        void stop(manapi::stoken token) override;
//    protected:
//        int m_flags;
//
//        manapi::slice_complex m_sc;
//
//        wrk_interface_global_t m_global;
//
//        std::size_t m_worker_pool_id;
//
//        std::size_t m_deep_worker_id;
//
//        manapi::stoken m_token;
//    };
//}