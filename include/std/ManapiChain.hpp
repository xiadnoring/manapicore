#pragma once

#include <memory>
#include <utility>

#include "../ManapiUtils.hpp"

namespace manapi {
    template<typename value_type>
    struct chain_item {
        value_type src;

        std::unique_ptr<chain_item> next;
        chain_item *prev;
    };


    template <typename value_type>
    class chain {
    public:
        using chain_item_un = std::unique_ptr<chain_item<value_type>>;
        using chain_item_ptr = chain_item<value_type> *;
        using pointer = value_type *;
        class chain_iterator {
        public:

            chain_iterator (chain_item_ptr n) {
                this->m_src = n;
            }

            chain_iterator &operator++(int) {
                this->m_src = this->m_src ? this->m_src->next.get() : nullptr;
                return *this;
            }

            chain_iterator &operator--(int) {
                this->m_src = this->m_src ? this->m_src->prev : nullptr;
                return *this;
            }

            value_type &operator*() {
                return this->m_src->src;
            }

            pointer operator->() {
                return &this->m_src->src;
            }

            const value_type &operator*() const {
                return this->m_src->src;
            }

            const pointer operator->() const {
                return &this->m_src->src;
            }

            bool operator==(const chain_iterator &_n) const {
                return this->m_src == _n.m_src;
            }

            bool operator!=(const chain_iterator &_n) const {
                return false == this->operator==(_n);
            }

            friend void swap (chain_iterator &lhs, chain_iterator &rhs) MANAPIHTTP_NOEXCEPT {
                std::swap(lhs.m_src, rhs.m_src);
            }

            operator bool () MANAPIHTTP_NOEXCEPT {
                return this->m_src != nullptr;
            }

            chain_item_ptr m_src;
        };

        class chain_const_iterator {
        public:

            chain_const_iterator (chain_item_ptr n) {
                this->m_src = n;
            }

            chain_const_iterator &operator++(int) {
                this->m_src = this->m_src ? this->m_src->next.get() : nullptr;
                return *this;
            }

            chain_const_iterator &operator--(int) {
                this->m_src = this->m_src ? this->m_src->prev : nullptr;
                return *this;
            }

            value_type const &operator*() {
                return this->m_src->src;
            }

            pointer const operator->() {
                return &this->m_src->src;
            }

            bool operator==(const chain_const_iterator &_n) const {
                return this->m_src == _n.m_src;
            }

            bool operator!=(const chain_const_iterator &_n) const {
                return false == this->operator==(_n);
            }

            friend void swap (chain_const_iterator &lhs, chain_const_iterator &rhs) MANAPIHTTP_NOEXCEPT {
                std::swap(lhs.m_src, rhs.m_src);
            }

            operator bool () MANAPIHTTP_NOEXCEPT {
                return this->m_src != nullptr;
            }

            chain_item_ptr m_src;
        };

        using iterator = chain_iterator;
        using const_iterator = chain_const_iterator;

        chain () {
            this->m_src = nullptr;
            this->m_last = nullptr;
            this->m_s = 0;
        }

        ~chain () {
            this->clear();
        }

        chain (chain &&n) MANAPIHTTP_NOEXCEPT {
            this->m_src = std::move(n.m_src);
            this->m_last = std::exchange(n.m_last, nullptr);
            this->m_s = std::exchange(n.m_s, 0);
        }

        chain &operator=(chain &&n) MANAPIHTTP_NOEXCEPT {
            if (this != &n) {
                this->m_src = std::move(n.m_src);
                this->m_last = std::exchange(n.m_last, nullptr);
                this->m_s = std::exchange(n.m_s, 0);
            }
            return *this;
        }

        void push_back (value_type &&n) {
            auto _n = std::make_unique<chain_item<value_type>>( std::move(n), nullptr, nullptr);
            this->push_back_chain(std::move(_n));
        }

        void push_front (value_type &&n) {
            auto _n = std::make_unique<chain_item<value_type>>( std::move(n), nullptr, nullptr);
            this->push_front_chain(std::move(_n));
        }

        void push_back (const value_type &n) {
            auto _n = std::make_unique<chain_item<value_type>>( n, nullptr, nullptr);
            this->push_back_chain(std::move(_n));
        }

        void push_front (const value_type &n) {
            auto _n = std::make_unique<chain_item<value_type>>( n, nullptr, nullptr);
            this->push_front_chain(std::move(_n));
        }

        void push_back_chain (chain_item_un n) {
            if (this->m_last == nullptr) {
                this->m_src = std::move(n);
                this->m_last = this->m_src.get();
                ++this->m_s;
                return;
            }

            this->m_last->next = std::move(n);
            this->m_last->next->prev = this->m_last;
            this->m_last = this->m_last->next.get();
            ++this->m_s;
        }

        void push_front_chain (chain_item_un n) {
            if (this->m_src == nullptr) {
                this->m_last = n.get();
                this->m_src = std::move(n);
                ++this->m_s;
                return;
            }

            this->m_src->prev = n.get();
            n->next = std::move(this->m_src);
            this->m_src = std::move(n);
            ++this->m_s;
        }

        chain_item_un erase (chain_item<value_type> *n) {
            auto ss = this->size();

            if (!ss) {
                return nullptr;
            }

            chain_item_un m;

            if (ss == 1) {
                if (this->m_src.get() == n) {
                    m = std::move(this->m_src);

                    this->m_src = nullptr;
                    this->m_last = nullptr;

                    --this->m_s;
                }
                return std::move(m);
            }

            auto next = std::move(n->next);
            auto prev = n->prev;

            if (!next && !prev) {
                // doesn't exist
                return nullptr;
            }

            if (!prev) {
                m = std::move(this->m_src);
                this->m_src = std::move(next);
                this->m_src->prev = nullptr;
            }
            else {
                m = std::move(prev->next);
                prev->next = std::move(next);
                if (prev->next)
                    prev->next->prev = prev;
                else
                    this->m_last = prev;
            }

            --this->m_s;
            m->prev = nullptr;
            m->next = nullptr;
            return std::move(m);
        }

        iterator erase (iterator n, chain_item_un &u) {
            if (!n.m_src) { return n; }
            auto next = n.m_src->next.get();
            u = this->erase(n.m_src);
            return iterator{next};
        }

        iterator erase (iterator n) {
            if (!n.m_src) { return n; }
            auto next = n.m_src->next.get();
            this->erase(n.m_src);
            return iterator{next};
        }

        void pop_back () {
            if (!this->m_last) {
                return;
            }

            this->m_last = this->m_last->prev;

            if (this->m_last) {
                this->m_last->next = nullptr;
            }
            else {
                this->m_src = nullptr;
            }

            --this->m_s;
        }

        void pop_front () {
            if (!this->m_src) {
                return;
            }

            this->m_src = std::move(this->m_src->next);

            if (this->m_src) {
                this->m_src->prev = nullptr;
            }
            else {
                this->m_last = nullptr;
            }

            --this->m_s;
        }

        MANAPIHTTP_NODISCARD bool empty () const {
            return this->size() == 0;
        }

        void clear () {
            chain_item_un c = std::move(this->m_src);
            while (c) {
                chain_item_un next = std::move(c->next);
                c->prev = nullptr;
                c = std::move(next);
            }
            this->m_src = nullptr;
            this->m_last = nullptr;
            this->m_s = 0;
        }

        iterator begin () {
            return iterator{this->m_src.get()};
        }

        iterator end () {
            return iterator{nullptr};
        }

        const_iterator begin () const {
            return const_iterator{this->m_src.get()};
        }

        const_iterator end () const {
            return const_iterator{nullptr};
        }

        const_iterator rbegin () const {
            return const_iterator{this->m_last};
        }

        const_iterator rend () const {
            return const_iterator{nullptr};
        }

        iterator rbegin () {
            return iterator{this->m_last};
        }

        iterator rend () {
            return iterator{nullptr};
        }

        MANAPIHTTP_NODISCARD size_t size () const {
            return this->m_s;
        }

        value_type &back () {
            return *this->rbegin();
        }

        value_type &front () {
            return *this->begin();
        }
    private:
        size_t m_s;
        chain_item_un m_src;
        chain_item<value_type> *m_last;
    };
}
