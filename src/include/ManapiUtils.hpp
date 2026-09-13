#pragma once

#define NOMINMAX

#include <string>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <list>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

#include "ManapiInt.hpp"
#include "ManapiParams.hpp"
#include "ManapiDebug.hpp"

#ifdef _WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   define MANAPIHTTP_NONUNIX true
#else
#   define MANAPIHTTP_NONUNIX false
#endif

#define WORKER_MAX_CNT INT_MAX

namespace manapi {
    void print_stacktrace () MANAPIHTTP_NOEXCEPT;
}

#ifdef MANAPIHTTP_DISABLE_TRACE
#   ifdef manapi_log_trace
#       undef manapi_log_trace
#       define manapi_log_trace(...)
#   endif
#endif

#ifdef MANAPIHTTP_DISABLE_TRACE_HARD
#   ifdef manapi_log_trace_hard
#       undef manapi_log_trace_hard
#       define manapi_log_trace_hard(...)
#   endif
#endif