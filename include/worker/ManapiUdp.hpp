#pragma once

#include "../ManapiUtils.hpp"
#include "./ManapiBaseWorker.hpp"
#include "./ManapiInterfaceWorker.hpp"

namespace manapi::net::worker {
    class udp : public worker::interface_worker {
    public:
        udp();
        ~udp() override;
        manapi::future<status> init(std::size_t deep) override;
        void stop(manapi::stoken token) override;
        virtual void onrecv (const std::shared_ptr<ev::udp> &watcher, char *buff, ssize_t size, const sockaddr *addr, unsigned flags) = 0;
    protected:
        virtual void recv_buffer_dealloc (const ev::buff_t *buf);
        virtual void recv_buffer_alloc (std::size_t nread, ev::buff_t *buff);

        sockaddr_storage m_sockaddrin;
        std::shared_ptr<ev::udp> m_udp_accept;
        addrinfo *m_local;
    private:
    };
}