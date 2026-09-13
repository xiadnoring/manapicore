#pragma once

#include "./ManapiBaseWorker.hpp"

namespace manapi::net::worker {
    class interface_worker : public worker::base {
    public:
        interface_worker ();

        ~interface_worker () override;

        void setup ( std::shared_ptr<net::worker::base_http> site, std::shared_ptr<manapi::net::worker::worker_data_t> worker_data, std::shared_ptr<manapi::net::http::config> config );

        void wrk_global (wrk_interface_global_t *data) MANAPIHTTP_NOEXCEPT override;

        wrk_interface_global_t *wrk_global () MANAPIHTTP_NOEXCEPT override;

        MANAPIHTTP_NODISCARD int worker_flags () MANAPIHTTP_NOEXCEPT;

        void worker_pool_id (std::size_t worker_pool_id) MANAPIHTTP_NOEXCEPT;

        MANAPIHTTP_NODISCARD std::size_t worker_pool_id () const MANAPIHTTP_NOEXCEPT;

        MANAPIHTTP_NODISCARD std::size_t deep_worker_id () const MANAPIHTTP_NOEXCEPT;

        manapi::future<status> init(std::size_t deep) override;

        void stop(manapi::stoken token) override;
    protected:
        int m_flags;

        wrk_interface_global_t m_global;

        std::size_t m_worker_pool_id;

        std::size_t m_deep_worker_id;

        manapi::stoken m_token;
    };
}