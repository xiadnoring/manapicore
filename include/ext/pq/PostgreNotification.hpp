#pragma once

#include <memory>
#include <string_view>
#include "../../ManapiUtils.hpp"

namespace manapi::ext::pq {
    #include <libpq-fe.h>
    class notification {
        struct pgnotify_deter {
            void operator () (PGnotify *p) {
                PQfreemem(p);
            }
        };

        std::unique_ptr<PGnotify, pgnotify_deter> m_pg_notify;

    public:
        notification () = default;

        notification (PGnotify *p) {
            this->m_pg_notify.reset(p);
        }

        notification (notification &&n) MANAPICORE_NOEXCEPT {
            this->m_pg_notify = std::move(n.m_pg_notify);
        }

        operator bool () const {
            return !!this->m_pg_notify;
        }

        MANAPICORE_NODISCARD int pid () const MANAPICORE_NOEXCEPT {
            if (this->m_pg_notify) {
                return this->m_pg_notify->be_pid;
            }

            return -1;
        }

        MANAPICORE_NODISCARD std::string_view channel () const MANAPICORE_NOEXCEPT {
            if (this->m_pg_notify) {
                return this->m_pg_notify->relname;
            }

            return {};
        }

        MANAPICORE_NODISCARD std::string_view payload () const MANAPICORE_NOEXCEPT {
            if (this->m_pg_notify) {
                return this->m_pg_notify->extra;
            }

            return {};
        }
    };
}