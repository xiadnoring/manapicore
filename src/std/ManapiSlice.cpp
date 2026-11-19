#include <cstring>

#include "ManapiAsync.hpp"
#include "ManapiMemoryPool.hpp"
#include "std/ManapiSlice.hpp"
#include "std/ManapiContext.hpp"

enum slice__flags {
    SLICE__FLAG_CAN_FREE = 1<<0,
    SLICE__FLAG_CAN_FREE_ONLY_DIV = 1<<1
};

static std::size_t summary_size_buffs (manapi::slice_part_t *first, manapi::slice_part_t *last) {
    std::size_t res = 0;

    if (!last)
        return 0;

    for (; first != last->next; first = first->next) {
        //assert(first->buff.len);
        res += first->buff.len;
    }

    if (res > 100000) {
        manapi_log_warn("slice:size is too big for summary_size_buffs()");
    }

    return res;
}

static std::size_t slice_size (manapi::slice_data_t *data) {
    return data->m_size - data->m_shift - data->m_rshift;
}

static manapi::status slice_shift_add (manapi::slice_data_t *data, std::size_t shift, int flags) {
    if (::slice_size(data) < shift)
        return manapi::status_out_of_range("slice: shift is too large");

    if (data->m_last) {
        data->m_shift += shift;
        assert(data->m_first != data->m_last->next);
        while (data->m_first && data->m_first != data->m_last->next
                && data->m_first->buff.len <= data->m_shift) {
            data->m_shift -= data->m_first->buff.len;
            data->m_size -= data->m_first->buff.len;
            auto next = data->m_first->next;
            if (flags & SLICE__FLAG_CAN_FREE) {
                manapi::async::current()->memory_fabric()->free(data->m_first->buff.base, data->m_first->buff.len);
                delete data->m_first;
            }
            else if (flags & SLICE__FLAG_CAN_FREE_ONLY_DIV) {
                delete data->m_first;
            }
            data->m_first = next;
            data->m_count -= 1;

            if (!data->m_first || data->m_first == data->m_last->next) {
                data->m_last = nullptr;
                data->m_first = nullptr;
                break;
            }
        }
    }

    return manapi::status_ok();
}

static manapi::status slice_rshift_add( manapi::slice_data_t *data, std::size_t s, int flags) MANAPICORE_NOEXCEPT {
    if (s > ::slice_size(data))
        return manapi::status_out_of_range("s is too large");

    assert(!!data->m_last);
    data->m_rshift += s;

    if (data->m_rshift < static_cast<std::size_t>(data->m_last->buff.len)) {
        return manapi::status_ok();
    }

    auto &mem = manapi::async::current()->memory_fabric();

    if (data->m_rshift == static_cast<std::size_t>(data->m_last->buff.len)) {
        if (flags & SLICE__FLAG_CAN_FREE) {
            mem->free(data->m_last->buff.base, data->m_last->buff.len);
            data->m_last->buff.base = nullptr;
            data->m_last->buff.len = 0;
            data->m_size -= data->m_rshift;
            data->m_rshift = 0;
        }
        return manapi::status_ok();
    }

#ifdef _MSC_VER
    manapi::slice_part_t** parts = static_cast<manapi::slice_part_t**>(alloca(sizeof (manapi::slice_part_t*)*data->m_count));
#else
    manapi::slice_part_t *parts[data->m_count];
#endif

    auto current = data->m_first;
    for (uint32_t i = 0; i < data->m_count; i++) {
        parts[i] = current;
        current = current->next;
    }

    while (data->m_count && data->m_rshift >= data->m_last->buff.len) {
        data->m_rshift -= data->m_last->buff.len;
        data->m_size -= data->m_last->buff.len;

        if (flags & SLICE__FLAG_CAN_FREE) {
            mem->free(data->m_last->buff.base, data->m_last->buff.len);
            delete data->m_last;
        }
        else if (flags & SLICE__FLAG_CAN_FREE_ONLY_DIV) {
            delete data->m_last;
        }

        data->m_count --;

        if (data->m_count) {
            data->m_last = parts[data->m_count - 1];
            data->m_last->next = nullptr;
        }
        else {
            assert(data->m_shift == 0);

            data->m_last = nullptr;
            data->m_first = nullptr;

            assert(data->m_size == 0);
        }
    }

    if (data->m_count == 1 && data->m_shift + data->m_rshift == data->m_last->buff.len) {
        if (flags & SLICE__FLAG_CAN_FREE) {
            mem->free(data->m_last->buff.base, data->m_last->buff.len);
            delete data->m_last;
        }
        else if (flags & SLICE__FLAG_CAN_FREE_ONLY_DIV) {
            delete data->m_last;
        }

        data->m_first = nullptr;
        data->m_last = nullptr;
        data->m_size = 0;
        data->m_rshift = 0;
        data->m_shift = 0;
        data->m_count--;
    }

    return manapi::status_ok();
}

static void slice_remove_rshift (manapi::slice_data_t *data) {
    if (!data->m_last) return;
    auto z = manapi::async::memory_fabric();
    std::size_t new_size = static_cast<std::size_t>(data->m_last->buff.len) - data->m_rshift;
    if (z->mem_type(static_cast<std::size_t>(data->m_last->buff.len)) == z->mem_type(new_size)) {
        data->m_size -= data->m_rshift;
        data->m_last->buff.len = static_cast<decltype(data->m_last->buff.len)> (new_size);
    }
    else {
        if (new_size) {
            auto bf = manapi::async::memory_fabric()->buffer(new_size).unwrap();
            ::memcpy(bf.data(), data->m_last->buff.base, static_cast<std::size_t>(data->m_last->buff.len) - data->m_rshift);

            data->m_size -= static_cast<std::size_t>(data->m_last->buff.len);
            manapi::async::memory_fabric()->free(data->m_last->buff.base, data->m_last->buff.len);

            data->m_size += new_size;

            data->m_last->buff.len = static_cast<decltype(data->m_last->buff.len)>(new_size);
            data->m_last->buff.base = static_cast<char *>(bf.release());
        }
        else {
            //if (flags & SLICE__FLAG_CAN_FREE)
            z->free(data->m_last->buff.base, static_cast<std::size_t>(data->m_last->buff.len));

            data->m_last->buff.base = nullptr;
            data->m_last->buff.len = 0;
        }
    }

    data->m_rshift = 0;
}

static void slice_remove_shift (manapi::slice_data_t *data) {
    if (!data->m_first) return;
    auto z = manapi::async::memory_fabric();
    std::size_t new_size = static_cast<std::size_t>(data->m_first->buff.len) - data->m_shift;
    if (z->mem_type(static_cast<std::size_t>(data->m_first->buff.len)) == z->mem_type(new_size)) {
        ::memmove(data->m_first->buff.base, data->m_first->buff.base + data->m_shift, static_cast<std::size_t>(data->m_first->buff.len) - data->m_shift);
        data->m_size -= data->m_shift;
        data->m_first->buff.len = static_cast<decltype(data->m_first->buff.len)> (new_size);
    }
    else {
        if (new_size) {
            auto bf = manapi::async::memory_fabric()->buffer(new_size).unwrap();
            ::memcpy(bf.data(), data->m_first->buff.base + data->m_shift, static_cast<std::size_t>(data->m_first->buff.len) - data->m_shift);
            data->m_size -= static_cast<std::size_t>(data->m_first->buff.len);
            manapi::async::memory_fabric()->free(data->m_first->buff.base, data->m_first->buff.len);

            data->m_size += new_size;

            data->m_first->buff.len = static_cast<decltype(data->m_first->buff.len)>(new_size);
            data->m_first->buff.base = static_cast<char *>(bf.release());
        }
        else {
            //if (flags & SLICE__FLAG_CAN_FREE)
            z->free(data->m_first->buff.base, static_cast<std::size_t>(data->m_first->buff.len));

            data->m_first->buff.base = nullptr;
            data->m_first->buff.len = 0;
        }
    }

    data->m_shift = 0;
}

static manapi::status_or<manapi::slice_base> slice_subslice (const manapi::slice_data_t *data, std::size_t pos, std::size_t size, manapi::slice_part_t **lastfirst) {
    pos += data->m_shift;
    auto const size_ = data->m_size - data->m_rshift;
    manapi::slice_part_t *lastfirst1;

    if (!size)
        return manapi::slice_base(nullptr, nullptr, 0, 0, 0, 0);

    if (!data->m_last)
        return manapi::status_out_of_range("shift and size is too large");

    if (pos == data->m_shift && size == size_) {
        return manapi::slice_base (data->m_first, data->m_last, data->m_count, data->m_shift, data->m_rshift, data->m_size - data->m_shift - data->m_rshift);
    }

    if (pos + size > size_) {
        return manapi::status_out_of_range("subslice size is too large");
    }

    auto tmp_size = size;
    auto tmp_pos = pos;
    uint32_t cnt = 0;
    lastfirst1 = nullptr;
    auto current = data->m_first;
    while (data->m_last->next != current
        && current->buff.len <= pos) {
        pos -= current->buff.len;
        lastfirst1 = current;
        current = current->next;
        cnt++;
    }

    if (current == data->m_last->next)
        return manapi::slice_base{nullptr, nullptr, 0, 0, 0, 0};

    if (tmp_pos + size >= data->m_size - static_cast<std::size_t>(data->m_last->buff.len)) {
        auto const rshift = data->m_size - tmp_pos - size;
        auto z = manapi::slice_base (current, data->m_last, data->m_count - cnt, pos, rshift, data->m_size - tmp_pos + pos - rshift - pos);
        if (lastfirst) *lastfirst = lastfirst1;
        return std::move(z);
    }

    cnt = 0;
    size += pos;
    auto scurrent = current;

    while (data->m_last->next != scurrent
        && scurrent->buff.len < size) {
        cnt++;
        size -= scurrent->buff.len;

        scurrent=scurrent->next;
    }
    if (data->m_last->next == scurrent)
        return manapi::status_out_of_range("pos and size too large");

    std::size_t rshift;

    if (size) {
        assert(scurrent->buff.len >= size);
        rshift = scurrent->buff.len - size;
        cnt++;
    }
    else {
        rshift = 0;
    }

    if (lastfirst) *lastfirst = lastfirst1;

    return manapi::slice_base{current, scurrent, cnt, pos, rshift, tmp_size};
}

manapi::slice_base::slice_base(std::unique_ptr<slice_part_t, slice_part_deleter> buffs, slice_part_t*last, uint32_t nbuff) {
    this->m_last = last;
    this->m_size = summary_size_buffs(buffs.get(), last);
    this->m_first = buffs.release();
    this->m_count = nbuff;
    this->m_rshift = 0;
    this->m_shift = 0;
}

manapi::slice_base::slice_base(std::unique_ptr<slice_part_t, slice_part_deleter> buffs, slice_part_t*last, uint32_t nbuff,
    std::size_t rshift) {
    this->m_last = last;
    this->m_rshift = rshift;
    this->m_size = summary_size_buffs(buffs.get(), last);
    this->m_first = buffs.release();
    this->m_count = nbuff;
    this->m_shift = 0;
}

manapi::slice_base::slice_base(std::unique_ptr<slice_part_t, slice_part_deleter> buffs, slice_part_t*last, uint32_t nbuff, std::size_t shift, std::size_t rshift, std::size_t size) {
    this->m_last = last;
    this->m_rshift = rshift;
    // this->m_size = summary_size_buffs(buffs, &this->m_last);
    this->m_size = size + shift + rshift;
    this->m_first = buffs.release();
    this->m_count = nbuff;
    this->m_shift = 0;

    assert(this->size() == size);
}

manapi::slice_base::slice_base(slice_part_t *first, slice_part_t *last, uint32_t count, std::size_t shift, std::size_t rshift, std::size_t size) {
    this->m_last = last;
    this->m_first = first;
    this->m_count = count;
    // this->m_size = summary_size_buffs (first, last);
    this->m_size = size + shift + rshift;
    this->m_shift = shift;
    this->m_rshift = rshift;

    assert(this->size() == size);
}

manapi::slice_iterator::slice_iterator(slice_part_t *part, slice_base *base) {
    this->m_part = part;
    this->m_base = base;
}

manapi::slice_iterator::slice_iterator(const slice_iterator &n) = default;

manapi::slice_iterator & manapi::slice_iterator::operator=(const slice_iterator &n) = default;

manapi::slice_iterator::~slice_iterator() = default;

manapi::slice_iterator & manapi::slice_iterator::operator++() {
    this->m_part = this->m_part->next;
    return *this;
}

manapi::slice_iterator & manapi::slice_iterator::operator++(int) {
    this->m_part = this->m_part->next;
    return *this;
}

std::string_view manapi::slice_iterator::operator*() {
    return std::string_view (static_cast<const char *> (this->buffer()), this->size ());
}

bool manapi::slice_iterator::operator==(const slice_iterator &n) const {
    return this->m_part == n.m_part;
}

void * manapi::slice_iterator::buffer() {
    if (this->m_base->slices_begin() == this->m_part)
        return this->m_part->buff.base + this->m_base->shift();

    return this->m_part->buff.base;
}

std::size_t manapi::slice_iterator::size() const {
    std::size_t s = this->m_part->buff.len;
    if (this->m_base->slices_begin() == this->m_part)
        s -= this->m_base->shift();
    if (this->is_last())
        s -= this->m_base->rshift();
    return s;
}

bool manapi::slice_iterator::is_last() const {
    return this->m_base->slices_end() == this->m_part->next;
}

manapi::slice_const_iterator::slice_const_iterator(slice_part_t *part, slice_base const *base) {
    this->m_part = part;
    this->m_base = base;
}

manapi::slice_const_iterator::slice_const_iterator(const slice_const_iterator &n) = default;

manapi::slice_const_iterator & manapi::slice_const_iterator::operator=(const slice_const_iterator &n) = default;

manapi::slice_const_iterator::~slice_const_iterator() = default;

std::string_view manapi::slice_const_iterator::operator*() {
    return std::string_view (static_cast<const char *> (this->buffer()), this->size ());
}

manapi::slice_const_iterator & manapi::slice_const_iterator::operator++() {
    this->m_part = this->m_part->next;
    return *this;
}

manapi::slice_const_iterator & manapi::slice_const_iterator::operator++(int) {
    this->m_part = this->m_part->next;
    return *this;
}

bool manapi::slice_const_iterator::operator==(const slice_const_iterator &n) const {
    return this->m_part == n.m_part;
}

const void * manapi::slice_const_iterator::buffer() {
    if (this->m_base->slices_begin() == this->m_part)
        return this->m_part->buff.base + this->m_base->shift();

    return this->m_part->buff.base;
}

std::size_t manapi::slice_const_iterator::size() const {
    std::size_t s = this->m_part->buff.len;
    if (this->m_base->slices_begin() == this->m_part)
        s -= this->m_base->shift();
    if (this->is_last())
        s -= this->m_base->rshift();
    return s;
}

bool manapi::slice_const_iterator::is_last() const {
    return this->m_base->slices_end() == this->m_part->next;
}

void manapi::slice_base::slice_part_deleter::operator()(slice_part_t *ptr) {
    if (ptr) {
        manapi::slice_base::slice_part_deleter::operator()(ptr->next);
    }
    delete ptr;
}

manapi::slice_base::~slice_base() = default;

manapi::slice_base::slice_base(slice_base &&n) MANAPICORE_NOEXCEPT = default;

manapi::slice_base & manapi::slice_base::operator=(slice_base &&n) MANAPICORE_NOEXCEPT = default;

manapi::slice_base::slice_base(const slice_base &n) = default;

manapi::slice_base & manapi::slice_base::operator=(const slice_base &n) = default;

manapi::status manapi::slice_base::shift_add(std::size_t shift) MANAPICORE_NOEXCEPT {
    if (this->size() < shift)
        return status_out_of_range("slice: shift is too large");

    if (this->m_last) {
        this->m_shift += shift;
        assert(this->m_first != this->m_last->next);
        while (this->m_first && this->m_first != this->m_last->next
                && this->m_first->buff.len <= this->m_shift) {
            this->m_shift -= this->m_first->buff.len;
            this->m_size -= this->m_first->buff.len;
            //manapi::async::current()->memory_fabric()->free(this->m_first->buff.base, this->m_first->buff.len);
            // auto const ptr = this->m_first;
            this->m_first = this->m_first->next;
            this->m_count -= 1;
            // delete ptr;

            if (!this->m_first || this->m_first == this->m_last->next) {
                this->m_last = nullptr;
                this->m_first = nullptr;
                break;
            }
        }
    }

    return status_ok();
}

manapi::status manapi::slice_base::copy_from(const void *buffer, std::size_t shift, std::size_t size) MANAPICORE_NOEXCEPT {
    if (!size)
        return manapi::status_ok();

    if (shift + size > this->size())
        return status_out_of_range("shift and size is too large");

    assert(!!this->m_last);

    auto current = this->m_first;

    shift += this->m_shift;
    while (current && current != this->m_last->next
        && current->buff.len < shift) {
        shift -= current->buff.len;
        current = current->next;
    }

    return this->copy_from(buffer, current, shift, size);
}

manapi::status manapi::slice_base::copy_from(const void *buffer, const slice_part_t *current, std::size_t shift, std::size_t size) MANAPICORE_NOEXCEPT {
    if (!current || current->buff.len < shift)
        return status_invalid_argument("copy_from:current is invalid");

    auto buffer_casted = static_cast<const char *>(buffer);

    while (size) {
        if (current == this->m_last->next)
            return manapi::status_out_of_range("shift and size is too large");

        std::size_t copy;
        if (current == this->m_last)
            copy = std::min(size, current->buff.len - shift - this->m_rshift);
        else
            copy = std::min(size, current->buff.len - shift);

        memcpy (current->buff.base + shift, buffer_casted, copy);

        buffer_casted += copy;
        size -= copy;
        current = current->next;
        shift = 0;
    }

    return manapi::status_ok();
}

manapi::status manapi::slice_base::copy_to(void *buffer, std::size_t shift, std::size_t size) MANAPICORE_NOEXCEPT {
    auto buffer_casted = static_cast<char *>(buffer);

    if (size) {
        if (!this->m_last)
            return status_out_of_range("shift and size is too large");

        auto current = this->m_first;
        shift += this->m_shift;
        while (current != this->m_last
                && current->buff.len <= shift) {
            shift -= current->buff.len;
            current=current->next;
        }
        while (size) {
            if (current == this->m_last->next)
                return manapi::status_out_of_range("shift and size is too large");

            std::size_t copy;
            if (current == this->m_last)
                copy = std::min(size, current->buff.len - shift - this->m_rshift);
            else
                copy = std::min(size, current->buff.len - shift);

            memcpy(buffer_casted, current->buff.base + shift, copy);

            size -= copy;
            shift = 0;
            current=current->next;
            buffer_casted+=copy;
        }
    }
    return status_ok();
}

manapi::status manapi::slice_base::copy_from(const slice_base &n, std::size_t shift, std::size_t shift_n, std::size_t size) MANAPICORE_NOEXCEPT {
    if (!size)
        return status_ok();

    if (!this->m_last)
        return status_out_of_range("shift and size is too large");

    auto current = this->m_first,
         current_n = n.m_first;

    shift += this->m_shift;
    while (current != this->m_last
        && current->buff.len <= shift) {
        shift -= current->buff.len;
        current=current->next;
    }

    shift_n += n.shift();

    while (current_n != n.m_last
        && current_n->buff.len <= shift_n) {
        shift_n -= current_n->buff.len;
        current_n=current_n->next;
    }

    while (size) {
        if (current == this->m_last->next
            || current_n == n.m_last->next)
            return manapi::status_out_of_range("shift and size is too large");

        std::size_t fcopy;
        if (current->next == this->m_last->next)
            fcopy = current->buff.len - shift - this->m_rshift;
        else
            fcopy = current->buff.len - shift;

        std::size_t scopy;
        if (current_n->next == n.m_last->next)
            scopy = current_n->buff.len - shift_n - n.m_rshift;
        else
            scopy = current_n->buff.len - shift_n;

        auto const copy = std::min(size, std::min(fcopy, scopy));

        memcpy (current->buff.base + shift, current_n->buff.base + shift_n, copy);

        shift += copy;
        shift_n += copy;

        size -= copy;

        if (shift == current->buff.len) {
            shift = 0;
            current=current->next;
        }

        if (shift_n == current_n->buff.len) {
            shift_n = 0;
            current_n=current_n->next;
        }
    }

    return manapi::status_ok();
}

manapi::status_or<manapi::slice_base> manapi::slice_base::subslice(std::size_t pos, std::size_t size) const MANAPICORE_NOEXCEPT {
    return ::slice_subslice (this, pos, size, nullptr);
}

manapi::status_or<manapi::slice_base> manapi::slice_base::subslice(std::size_t pos) const MANAPICORE_NOEXCEPT {
    if (this->size() == pos)
        return manapi::slice_base(nullptr, nullptr, 0, 0, 0, 0);

    auto z = *this;
    z.shift_add(pos).unwrap();

    return std::move(z);
}

const manapi::slice_part_t * manapi::slice_base::slices_begin() const {
    return this->m_first;
}

const manapi::slice_part_t * manapi::slice_base::slices_end() const {
    return (!!this->m_last) ? (this->m_last->next) : nullptr;
}

const manapi::slice_part_t * manapi::slice_base::slices_rbegin() const {
    return this->m_last;
}

manapi::slice_iterator manapi::slice_base::begin() {
    return slice_iterator{this->m_first, this};
}

manapi::slice_iterator manapi::slice_base::end() {
    return slice_iterator{(this->m_last) ? (this->m_last->next) : nullptr, this};
}

manapi::slice_const_iterator manapi::slice_base::begin() const {
    return slice_const_iterator{this->m_first, this};
}

manapi::slice_const_iterator manapi::slice_base::end() const {
    return slice_const_iterator{(this->m_last) ? (this->m_last->next) : nullptr, this};
}

int manapi::slice_base::cmp(const manapi::slice_base &n) const MANAPICORE_NOEXCEPT {
    auto size = n.size();

    auto buffptr1 = this->m_first;
    auto buffptr2 = n.m_first;

    std::size_t cursor1 = this->shift();
    std::size_t cursor2 = n.shift();

    if (buffptr1 && buffptr2) {
        std::size_t size1 = buffptr1->buff.len;
        std::size_t size2 = buffptr2->buff.len;

        if (!buffptr1->next)
            size1 -= this->m_rshift;

        if (!buffptr2->next)
            size2 -= n.m_rshift;

        while (size && buffptr1 && buffptr2) {
            auto cmp_size = std::min<std::size_t>(size1 - cursor1,
                size2 - cursor2);

            if (cmp_size > size)
                cmp_size = size;

            if (auto const rhs = memcmp(buffptr1->buff.base + cursor1, buffptr2->buff.base + cursor2, cmp_size))
                return rhs > 0 ? 1 : -1;

            cursor1 += cmp_size;
            cursor2 += cmp_size;

            if (cursor1 == buffptr1->buff.len) {
                buffptr1 = buffptr1->next;
                cursor1 = 0;

                if (buffptr1) {
                    size1 = buffptr1->buff.len;
                    if (!buffptr1->next)
                        size1 -= this->m_rshift;
                }
            }

            if (cursor2 == buffptr2->buff.len) {
                buffptr2 = buffptr2->next;
                cursor2 = 0;

                if (buffptr2) {
                    size2 = buffptr2->buff.len;
                    if (!buffptr2->next)
                        size2 -= n.m_rshift;
                }
            }

            size -= cmp_size;
        }
    }

    size = n.size();

    if (this->size() == size)
        return 0;

    return this->size() > size ? 1 : -1;
}

int manapi::slice_base::cmp(const void *data, std::size_t size) const MANAPICORE_NOEXCEPT {
    int rhs;
    size_t ss;

    for (auto it = this->begin(); it != this->end(); it++) {
        ss = std::min<std::size_t>(it.size(), size);
        rhs = memcmp (static_cast<const char *>(it.buffer()), static_cast<const char *>(data), ss);
        if (rhs)
            return rhs;
        size -= ss;
        data = static_cast<const char*>(data) + ss;
        if (it.size() != ss)
            return 1;
    }

    if (!size)
        return 0;

    return -1;
}

void manapi::slice_base::slices_buffs(ev::buff_t *buffs, std::size_t cnt, std::size_t *sz) const {
    auto buffsptr = buffs;
    auto cur = this->m_first;

    if (sz) *sz = 0;

    while (this->m_last && cur && cur != this->m_last->next && cnt) {
        *buffsptr = cur->buff;
        if (sz) *sz += static_cast<std::size_t>(cur->buff.len);
        buffsptr++;
        cur = cur->next;
        cnt--;
    }

    if (buffs != buffsptr) {
        assert(!!this->m_last);
        buffsptr--;

        buffs->base += this->m_shift;
        buffs->len -= static_cast<decltype(buffs->len)>(this->m_shift);
        if (sz) *sz -= this->m_shift;

        if (cur == this->m_last->next) {
            if (sz) *sz -= this->m_rshift;
            buffsptr->len -= static_cast<decltype(buffsptr->len)>(this->m_rshift);
        }
    }
}

std::unique_ptr<manapi::ev::buff_t, manapi::ev::buffer_deleter> manapi::slice_base::slices_buffs(std::size_t max_cnt, std::size_t *cnt, std::size_t *sz) const {
    std::unique_ptr<manapi::ev::buff_t, manapi::ev::buffer_deleter> buffs;
    auto buffs_cnt = std::min<std::size_t>(this->m_count, max_cnt);
    buffs.reset(new ev::buff_t[buffs_cnt]);
    if (cnt) *cnt = buffs_cnt;
    auto buffsptr = buffs.get();
    auto cur = this->m_first;

    if (sz) *sz = 0;

    while (this->m_last && cur && cur != this->m_last->next && max_cnt) {
        *buffsptr = cur->buff;
        if (sz) *sz += static_cast<std::size_t>(cur->buff.len);
        buffsptr++;
        cur = cur->next;
        max_cnt--;
    }

    if (buffs.get() != buffsptr) {
        assert(!!this->m_last);
        buffsptr--;
        buffs->base += this->m_shift;
        if (sz) *sz -= this->m_shift;
        buffs->len -= static_cast<decltype(buffs->len)>(this->m_shift);
        if (cur == this->m_last->next) {
            if (sz) *sz -= this->m_rshift;
            buffsptr->len -= static_cast<decltype(buffsptr->len)>(this->m_rshift);
        }
    }

    return std::move(buffs);
}

std::size_t manapi::slice_base::size() const {
    return this->m_size - this->m_shift - this->m_rshift;
}

std::string manapi::slice_base::to_string() const {
    std::string z;
    z.reserve(this->size());
    std::size_t i = 0;
    for (const auto b : *this) {
        ::memcpy(z.data() + i, b.data(), b.size());
        i += b.size();
    }
    return std::move(z);
}

manapi::slice_view::slice_view(const slice_base &n) : slice_base(n) {

}

manapi::slice_view::slice_view(const slice &n) : slice_base(n) {
}

manapi::slice_view::slice_view(const slice_view &n) = default;

manapi::slice_view & manapi::slice_view::operator=(const slice_view &n) = default;

manapi::slice_view::~slice_view() = default;

manapi::slice_ref::slice_ref() : slice_base(nullptr, nullptr, 0, 0, 0, 0) {

}

manapi::slice_ref::slice_ref(const slice_ref &n) : slice_ref() {
    assert(!n.m_rshift);
    assert(!n.m_shift);

    for (auto it = n.slices_begin(); it != n.slices_end(); ++it) {
        auto t = std::make_unique<slice_part_t>();
        t->buff = it->buff;
        if (this->m_last) {
            this->m_last->next = t.release();
            this->m_last = this->m_last->next;
        }
        else {
            this->m_first = t.release();
            this->m_last = this->m_first;
        }
    }

    this->m_count = n.m_count;
    this->m_size = n.m_size;
}

manapi::slice_ref & manapi::slice_ref::operator=(const slice_ref &n) {
    this->clear();

    assert(!n.m_rshift);
    assert(!n.m_shift);

    for (auto it = n.slices_begin(); it != n.slices_end(); ++it) {
        auto t = std::make_unique<slice_part_t>();
        t->buff = it->buff;
        if (this->m_last) {
            this->m_last->next = t.release();
            this->m_last = this->m_last->next;
        }
        else {
            this->m_first = t.release();
            this->m_last = this->m_first;
        }
    }

    this->m_count = n.m_count;
    this->m_size = n.m_size;
    return *this;
}

manapi::slice_ref::slice_ref(slice_ref &&n) MANAPICORE_NOEXCEPT = default;

manapi::slice_ref & manapi::slice_ref::operator=(slice_ref &&n) MANAPICORE_NOEXCEPT = default;

manapi::slice_ref::~slice_ref() {
    this->clear();
}

manapi::status manapi::slice_ref::push_back(const void *buffer, std::size_t size) {
    assert(!this->m_rshift);
    auto t = std::make_unique<slice_part_t>();
    t->buff.base = (char*)(buffer);
    t->buff.len = static_cast<decltype(t->buff.len)>(size);
    this->m_size += size;
    if (this->m_last) {
        this->m_last->next = t.release();
        this->m_last = this->m_last->next;
    }
    else {
        assert(!this->m_first);
        this->m_first = t.release();
        this->m_last = this->m_first;
    }
    this->m_count++;
    return status_ok();
}

manapi::status manapi::slice_ref::shift_add(std::size_t shift) MANAPICORE_NOEXCEPT {
    return ::slice_shift_add(this, shift, SLICE__FLAG_CAN_FREE_ONLY_DIV);
}

void manapi::slice_ref::clear() MANAPICORE_NOEXCEPT {
    auto cur = this->m_first;
    if (this->m_last) {
        while (cur && cur != this->m_last->next) {
            delete std::exchange(cur, cur->next);
        }
    }

    this->m_first = nullptr;
    this->m_last = nullptr;
    this->m_size = 0;
    this->m_shift = 0;
    this->m_count = 0;
}


manapi::slice::slice() : slice_base(nullptr, nullptr, 0, 0, 0, 0) {
}

manapi::status_or<manapi::slice> manapi::slice::create(std::size_t n) MANAPICORE_NOEXCEPT {
    return manapi::async::current()->memory_fabric()->slice(n);
}

manapi::slice::slice(std::unique_ptr<slice_part_t, slice_part_deleter> buffs, slice_part_t*last, uint32_t nbuff)
    : slice_base(std::move(buffs), last, nbuff){
}

manapi::slice::slice(std::unique_ptr<slice_part_t, slice_part_deleter> buffs,slice_part_t*last, uint32_t nbuff, std::size_t rshift)
    : slice_base(std::move(buffs), last, nbuff, rshift){
}

manapi::slice::slice(std::unique_ptr<slice_part_t, slice_part_deleter> buffs,slice_part_t*last, uint32_t nbuff, std::size_t shift,
    std::size_t rshift, std::size_t size) : slice_base(std::move(buffs), last, nbuff, shift, rshift, size) {

}

manapi::slice::slice(slice_part_t *first, slice_part_t *last, uint32_t count, std::size_t shift, std::size_t rshift,
                     std::size_t size) : slice_base(first, last, count, shift, rshift, size) {
}

manapi::slice::slice(std::string_view n) : slice(n.data(), n.size()) {
}

manapi::slice::slice(const char *n) : slice(std::string_view(n)) {
}

manapi::slice::slice(const char *buffer, std::size_t sz) : slice() {
    this->resize(sz);
    this->copy_from(buffer, 0, sz);
}

//
// manapi::slice::slice(slice_base n) : slice_base(std::move(n)) {
//
// }

manapi::slice::slice(slice &&n) MANAPICORE_NOEXCEPT : slice() {
    this->m_first = n.m_first;
    this->m_last = n.m_last;
    this->m_count = n.m_count;
    this->m_rshift = n.m_rshift;
    this->m_size = n.m_size;
    this->m_shift = n.m_shift;

    n.m_size = 0;
    n.m_shift = 0;
    n.m_rshift = 0;
    n.m_count = 0;
    n.m_first = nullptr;
    n.m_last = nullptr;
}

manapi::slice & manapi::slice::operator=(slice &&n) MANAPICORE_NOEXCEPT {
    this->clear();

    this->m_first = n.m_first;
    this->m_last = n.m_last;
    this->m_count = n.m_count;
    this->m_rshift = n.m_rshift;
    this->m_shift = n.m_shift;
    this->m_size = n.m_size;

    n.m_size = 0;
    n.m_shift = 0;
    n.m_rshift = 0;
    n.m_count = 0;
    n.m_first = nullptr;
    n.m_last = nullptr;
    return *this;
}

manapi::slice::~slice() {
    this->clear();
}

manapi::status manapi::slice::resize(std::size_t size) MANAPICORE_NOEXCEPT {
    auto const cur = this->size();

    if (size == cur)
        return status_ok();

    if (size < cur) {
        auto res = ::slice_rshift_add(this, cur - size, SLICE__FLAG_CAN_FREE).ok();
        assert(res);
    }
    else {
        size -= cur;

        auto rhs = std::min<size_t>(this->m_rshift, size);
        const auto prev_rshift = this->m_rshift;
        this->m_rshift -= rhs;
        size -= rhs;

        if (size) {
            if (this->m_last && this->m_last->buff.len < manapi::object_pool::area_size()) {
                auto sz = static_cast<std::size_t>(this->m_last->buff.len) + size;

                auto bf = manapi::async::memory_fabric()->buffer(std::max(sz, manapi::object_pool::area_size())).unwrap();

                if (bf.size() >= sz) {
                    this->m_rshift = bf.size() - sz;
                    sz = 0;
                }
                else {
                    sz -= bf.size();
                }

                ::memcpy(bf.data(), this->m_last->buff.base, static_cast<std::size_t>(this->m_last->buff.len) - prev_rshift);
                this->m_size -= static_cast<std::size_t>(this->m_last->buff.len);
                manapi::async::memory_fabric()->free(this->m_last->buff.base, this->m_last->buff.len);

                this->m_size += bf.size();

                this->m_last->buff.len = static_cast<decltype(this->m_last->buff.len)>(bf.size());
                this->m_last->buff.base = static_cast<char *>(bf.release());

                if (sz) {
                    auto slice = manapi::async::memory_fabric()->slice(sz);
                    if (!slice.ok())
                        return slice.err();

                    this->push_back(slice.unwrap());
                }
            }
            else {
                auto slice = manapi::async::memory_fabric()->slice(size);
                if (!slice.ok())
                    return slice.err();
                this->push_back(slice.unwrap());
            }
        }
    }

    return status_ok();
}

manapi::status manapi::slice::push_back(bytebuffer buffer) MANAPICORE_NOEXCEPT {
    if (buffer.empty()) return status_ok();

    std::unique_ptr<slice_part_t> t (new (std::nothrow) slice_part_t{});
    if (!t) return status_resource_exhausted();

    ::slice_remove_rshift(this);

    auto const shift = buffer.shift();

    t->buff.len = static_cast<decltype(t->buff.len)>(buffer.realsize());
    this->m_size += buffer.realsize();
    assert(!this->m_rshift);
    this->m_rshift = buffer.realsize() - buffer.size() - buffer.shift();
    t->buff.base = static_cast<char*>(buffer.release());

    if (shift) ::memmove(t->buff.base, t->buff.base + shift, t->buff.len - shift);

    if (this->m_last) {
        this->m_last->next = t.release();
        this->m_last = this->m_last->next;
    }
    else {
        assert(!this->m_first);
        this->m_first = t.release();
        this->m_last = this->m_first;
    }

    this->m_count++;

    return status_ok();
}

manapi::status manapi::slice::push_back(slice &&s) MANAPICORE_NOEXCEPT {
    // WARNING: split(...) uses this function
    if (!s.empty()) {
        ::slice_remove_shift(&s);
        ::slice_remove_rshift(this);

        assert (!this->m_rshift && !s.m_shift);

        this->m_rshift = s.m_rshift;
        if (this->m_last) {
            this->m_last->next = s.m_first;
            this->m_last = s.m_last;
        }
        else {
            this->m_last = s.m_last;
        }
        if (!this->m_first) {
            this->m_first = s.m_first;
            this->m_shift = 0;
        }
        this->m_count += s.m_count;
        this->m_size += s.m_size;

        s.m_first = nullptr;
        s.m_last = nullptr;
        s.m_count = 0;
        s.m_size = 0;
        s.m_rshift = 0;
        s.m_shift = 0;
    }

    return status_ok();
}

manapi::status manapi::slice::push_back(const ev::buff_t *buff) MANAPICORE_NOEXCEPT {
    try {
        if (!buff->len) return status_ok();

        std::unique_ptr<slice_part_t> t (new (std::nothrow) slice_part_t{});
        if (!t) return status_resource_exhausted();

        ::slice_remove_rshift (this);
        assert(!this->m_rshift);

        t->buff.len = buff->len;
        this->m_size += static_cast<std::size_t> (buff->len);
        t->buff.base = buff->base;

        if (this->m_last) {
            this->m_last->next = t.release();
            this->m_last = this->m_last->next;
        }
        else {
            assert(!this->m_first);
            this->m_first = t.release();
            this->m_last = this->m_first;
        }

        this->m_count++;

        return status_ok();
    }
    catch (std::exception const &e) {
        manapi_log_error(e.what());
    }

    manapi::async::memory_fabric()->free ( buff->base, static_cast<std::size_t>(buff->len) );

    return status_resource_exhausted();
}

manapi::status manapi::slice::push_back(const void *buffer, std::size_t size) MANAPICORE_NOEXCEPT {
    try {
        if (!size) return status_ok();
        auto last = this->m_last;
        std::size_t shift;
        if (last) shift = static_cast<std::size_t>(last->buff.len) - this->m_rshift;
        this->resize(size + this->size()).unwrap();
        if (!last) {
            shift = 0;
            last = this->m_first;
        }
        this->copy_from(buffer, last, shift, size).unwrap();

        return status_ok();
    }
    catch (std::exception const &e) {
        manapi_log_error(e.what());
    }
    return status_resource_exhausted();
}

manapi::status manapi::slice::push_back(std::string_view buffer) MANAPICORE_NOEXCEPT {
    return this->push_back(buffer.data(), buffer.size());
}

manapi::status manapi::slice::shift_add(std::size_t shift) MANAPICORE_NOEXCEPT {
    return ::slice_shift_add (this, shift, SLICE__FLAG_CAN_FREE);
}

manapi::status_or<manapi::slice> manapi::slice::split(std::size_t start, std::size_t size) MANAPICORE_NOEXCEPT {
    try {
        slice_part_t *lastfirst = nullptr;
        auto z = ::slice_subslice(this, start, size, &lastfirst);
        if (!z) return z.err();
        manapi::slice_base sv = z.unwrap();

        manapi::slice res;
        res.m_first = sv.m_first;
        res.m_last = sv.m_last;
        res.m_count = sv.m_count;
        res.m_shift = sv.m_shift;
        res.m_rshift = sv.m_rshift;
        res.m_size = sv.m_size;

        if (res.m_first) {

            if (res.m_shift || res.m_rshift) {
                std::size_t cc_shift_sub = 0, cc_rshift_sub = 0;
                if (res.m_first == this->m_first) cc_shift_sub = this->m_shift;
                if (res.m_last == this->m_last) cc_rshift_sub = this->m_rshift;
                auto cc = manapi::async::memory_fabric()->slice(res.m_shift + res.m_rshift - cc_shift_sub - cc_rshift_sub).unwrap();

                assert(!cc.shift());

                cc.copy_from(res.m_first->buff.base + cc_shift_sub, 0, res.m_shift - cc_shift_sub).unwrap();
                cc.copy_from(res.m_last->buff.base + static_cast<std::size_t> (res.m_last->buff.len) - res.m_rshift,
                             res.m_shift, res.m_rshift - cc_rshift_sub).unwrap();

                this->m_count -= res.m_count;
                this->m_size -= res.m_size;

                auto last = this->m_last;
                auto next = res.m_last->next;
                res.m_last->next = nullptr;

                if (last != res.m_last) {
                    ::slice_remove_rshift(&cc);
                }

                if (this->m_first == res.m_first) {
                    this->m_first = nullptr;
                    this->m_shift = 0;
                }
                else {
                    lastfirst->next = nullptr;
                }

                auto rshift = this->m_rshift;
                this->m_rshift = 0;

                if (this->m_first) {
                    assert(!!lastfirst);
                    this->m_last = lastfirst;
                    this->push_back(std::move(cc)).unwrap();
                }
                else {
                    this->m_last = nullptr;
                    this->push_back(std::move(cc)).unwrap();
                }



                if (last != res.m_last) {
                    assert(next);
                    this->m_last->next = next;
                    this->m_last = last;

                    assert(!this->m_rshift);
                    this->m_rshift = rshift;
                }
                else {
                    assert(!next);
                }

                assert(!this->m_last || static_cast<std::size_t>(this->m_last->buff.len) >= this->m_rshift);
            }
            else {

                this->m_count -= res.m_count;
                this->m_size -= res.m_size;

                if (this->m_first == res.m_first) {
                    this->m_first = res.m_last->next;
                    this->m_shift = 0;
                }
                else {
                    lastfirst->next = res.m_last->next;
                }

                res.m_last->next = nullptr;

                if (this->m_last == res.m_last) {
                    this->m_rshift = 0;
                    this->m_last = lastfirst;
                }
            }
        }

        return std::move(res);
    }
    catch (std::bad_alloc const &) {
        return manapi::status_resource_exhausted();
    }
    catch (std::exception const &e) {
        manapi_log_error("%s due to %s", "split failed", e.what());
        return manapi::status_internal("split failed");
    }
}

void manapi::slice::clear() MANAPICORE_NOEXCEPT {
    auto cur = this->m_first;

    while (cur&&cur != this->m_last->next) {
        manapi::async::current()->memory_fabric()->free(cur->buff.base, cur->buff.len);
        delete std::exchange(cur, cur->next);
    }

    this->m_first = nullptr;
    this->m_last = nullptr;
    this->m_size = 0;
    this->m_shift = 0;
}

manapi::slice manapi::slice::copy() const {
    auto nsv = manapi::async::memory_fabric()->slice(this->size()).unwrap();
    nsv.copy_from(*this, 0, 0, this->size()).unwrap();
    return std::move(nsv);
}

manapi::slice::slice(const manapi::ev::buff_t *buff) : slice() {
    this->push_back( buff ).unwrap();
}

manapi::slice_view::slice_view() : slice_base(nullptr, nullptr, 0, 0, 0, 0) {

}

manapi::slice_view::slice_view(slice_part_t *part) : slice_base(part, part, 1, 0, 0, part->buff.len) {

}

manapi::slice_view::slice_view(slice_part_t *first, slice_part_t *last, uint32_t count, std::size_t shift, std::size_t rshift, std::size_t size) : slice_base(first, last, count, shift, rshift, size) {

}

enum slice_complex_flags {
    SLICE_COMPLEX_IS_SLICE = 1
};

static void manapi__slice_complex_reset (uint8_t flags, manapi::slice_complex::st_t *s) {
    if (flags & SLICE_COMPLEX_IS_SLICE) {
        s->s.~slice();
    } else {
        s->sv.~slice_view();
    }
}

manapi::slice_complex::slice_complex() : slice_complex( manapi::slice()) {
}

manapi::slice_complex::~slice_complex() {
    manapi__slice_complex_reset (this->m_flags, &this->m_st);
}

manapi::slice_complex::slice_complex(manapi::slice &&n) {
    new (&this->m_st) manapi::slice ( std::forward<decltype(n)>(n) );
    this->m_flags = SLICE_COMPLEX_IS_SLICE;
}

manapi::slice_complex::slice_complex(manapi::slice_view &&n) {
    new (&this->m_st) manapi::slice_view ( std::forward<decltype(n)>(n) );
    this->m_flags = 0;
}

manapi::slice_complex::slice_complex(manapi::slice_complex &&n) MANAPICORE_NOEXCEPT {
    this->m_flags = n.m_flags;
    if (n.m_flags & SLICE_COMPLEX_IS_SLICE) {
        new (&this->m_st) manapi::slice ( std::move (n.m_st.s) );
    } else {
        new (&this->m_st) manapi::slice_view (  (n.m_st.sv) );
    }
}

manapi::slice_complex &manapi::slice_complex::operator=(manapi::slice &&n) MANAPICORE_NOEXCEPT {
    if (&this->m_st.s != &n) {
        manapi__slice_complex_reset(this->m_flags, &this->m_st);
        this->m_flags = SLICE_COMPLEX_IS_SLICE;
        new(&this->m_st) manapi::slice(std::forward<decltype(n)>(n));
    }
    return *this;
}

manapi::slice_complex &manapi::slice_complex::operator=(manapi::slice_view &&n) MANAPICORE_NOEXCEPT {
    if (&this->m_st.sv != &n) {
        manapi__slice_complex_reset(this->m_flags, &this->m_st);
        this->m_flags = 0;
        new(&this->m_st) manapi::slice_view (std::forward<decltype(n)>(n));
    }
    return *this;
}

manapi::slice_complex &manapi::slice_complex::operator=(manapi::slice_complex &&n) MANAPICORE_NOEXCEPT {
    if (this != &n) {
        manapi__slice_complex_reset (this->m_flags, &this->m_st);
        this->m_flags = n.m_flags;
        if (n.m_flags & SLICE_COMPLEX_IS_SLICE) {
            new (&this->m_st) manapi::slice ( std::move (n.m_st.s) );
        } else {
            new (&this->m_st) manapi::slice_view (  (n.m_st.sv) );
        }
    }
    return *this;
}

manapi::slice_base *manapi::slice_complex::base() MANAPICORE_NOEXCEPT {
    if (this->m_flags & SLICE_COMPLEX_IS_SLICE) return &this->m_st.s;
    return &this->m_st.sv;
}

const manapi::slice_base *manapi::slice_complex::base() const MANAPICORE_NOEXCEPT {
    if (this->m_flags & SLICE_COMPLEX_IS_SLICE) return &this->m_st.s;
    return &this->m_st.sv;
}

bool manapi::slice_complex::is_slice() const MANAPICORE_NOEXCEPT {
    return this->m_flags & SLICE_COMPLEX_IS_SLICE;
}

bool manapi::slice_complex::is_slice_view() const MANAPICORE_NOEXCEPT {
    return !(this->m_flags & SLICE_COMPLEX_IS_SLICE);
}

manapi::slice *manapi::slice_complex::get_slice() MANAPICORE_NOEXCEPT {
    return &this->m_st.s;
}

const manapi::slice *manapi::slice_complex::get_slice() const MANAPICORE_NOEXCEPT {
    return &this->m_st.s;
}

manapi::slice_view *manapi::slice_complex::get_slice_view() MANAPICORE_NOEXCEPT {
    return &this->m_st.sv;
}

const manapi::slice_view *manapi::slice_complex::get_slice_view() const MANAPICORE_NOEXCEPT {
    return &this->m_st.sv;
}

manapi::slice_complex &manapi::slice_complex::to_slice() {
    if (this->m_flags & SLICE_COMPLEX_IS_SLICE) return *this;
    auto s = manapi::slice ();
    s.resize( this->m_st.sv.size() ).unwrap();
    s.copy_from( this->m_st.sv, 0, 0, this->m_st.sv.size() ).unwrap();
    return *this;
}


manapi::slice_complex::st_t::~st_t() {
}

manapi::slice_complex::st_t::st_t() {
}
