#pragma once

#include "./../include/ManapiErrors.hpp"
#include "./../include/ManapiTimerObject.hpp"

namespace manapi::internal {
    manapi::status timer__update_interval_state (const std::shared_ptr<timer::timer_data_t> &data) MANAPICORE_NOEXCEPT;

    void timer__unref_important () MANAPICORE_NOEXCEPT;

    void timer__call (const std::shared_ptr<manapi::timer::timer_data_t> &data) MANAPICORE_NOEXCEPT;
}