#pragma once

#include <utility>

#include "./../ManapiUtils.hpp"

namespace manapi {
    namespace reference_impl {
        template <typename T, typename = void> struct HasRefCnt : std::false_type {};

        template <typename T> struct HasRefCnt<T, decltype(void(std::declval<T>().refcnt))> : std::true_type {};

        template <typename T>
        struct is_atomic : std::false_type {};

        template <typename U>
        struct is_atomic<std::atomic<U>> : std::true_type {};
    }

    template<typename T, std::enable_if_t<reference_impl::HasRefCnt<T>::value, std::nullptr_t> = nullptr>
    class reference {
    public:
        using is_atomic = reference_impl::is_atomic<decltype(std::declval<T>().refcnt)>;

        reference () {
            this->src = nullptr;
        }

        reference (T *n) {
            this->src = n;
            if (n) {
                if constexpr (is_atomic::value) {
                    n->refcnt.fetch_add(1);
                } else {
                    n->refcnt++;
                }
            }
        }

        ~reference() {
            this->reset();
        }

        reference (reference &&n) MANAPIHTTP_NOEXCEPT {
            this->src = n.src;
            n.src = nullptr;
        }

        reference& operator= (reference &&n) MANAPIHTTP_NOEXCEPT {
            if (this != &n) {
                if (n.src) {
                    if constexpr (is_atomic::value) {
                        n.src->refcnt.fetch_add(1);
                    } else {
                        n.src->refcnt++;
                    }
                    this->reset();
                    this->src = n.src;
                    n.reset();
                }
                else {
                    this->reset();
                }
            }
            return *this;
        }

        reference (const reference &n) : src(nullptr) {
            this->reset(n.src);
        }

        reference& operator= (const reference &n) {
            if (this != &n) {
                this->reset(n.src);
            }
            return *this;
        }

        void reset () MANAPIHTTP_NOEXCEPT {
            if (this->src) {
                if constexpr (is_atomic::value) {
                    if (this->src->refcnt.fetch_sub(1) == 1) {
                        delete this->src;
                    }
                } else {
                    if (!--this->src->refcnt) {
                        delete this->src;
                    }
                }
                this->src = nullptr;
            }
        }

        void reset (T *n) MANAPIHTTP_NOEXCEPT {
            if (n) {
                if constexpr (is_atomic::value) {
                    n->refcnt.fetch_add(1);
                } else {
                    n->refcnt++;
                }
                this->reset();
                this->src = n;
            }
            else {
                this->reset();
            }
        }

        T *operator-> () MANAPIHTTP_NOEXCEPT {
            return this->src;
        }

        T *get () MANAPIHTTP_NOEXCEPT {
            return this->src;
        }

        MANAPIHTTP_NODISCARD T *get () const MANAPIHTTP_NOEXCEPT {
            return this->src;
        }

        MANAPIHTTP_NODISCARD T *operator->() const MANAPIHTTP_NOEXCEPT {
            return this->src;
        }

        T &operator *() MANAPIHTTP_NOEXCEPT {
            return *this->src;
        }

        MANAPIHTTP_NODISCARD const T &operator*() const MANAPIHTTP_NOEXCEPT {
            return *this->src;
        }

        MANAPIHTTP_NODISCARD operator bool () const MANAPIHTTP_NOEXCEPT {
            return !!this->src;
        }

        manapi::status ref () const MANAPIHTTP_NOEXCEPT {
            if (this->src) {
                if constexpr (is_atomic::value) {
                    this->src->refcnt.fetch_add(1);
                } else {
                    this->src->refcnt++;
                }
                return manapi::status_ok();
            }
            return manapi::status_internal("ref:Object doesn't exist");
        }

        void unref () const MANAPIHTTP_NOEXCEPT {
            if (this->src) {
                if constexpr (is_atomic::value) {
                    if (this->src->refcnt.fetch_sub(1) == 1) {
                        delete this->src;
                        this->src = nullptr;
                    }
                } else {
                    if (!--this->src->refcnt) {
                        delete this->src;
                        this->src = nullptr;
                    }
                }
            }
        }
    private:
        mutable T *src;
    };
}