#include <any>
#include <cstring>
#include <format>
#include <stdexcept>
#include <utility>

#include "std/ManapiBuffer.hpp"
#include "std/ManapiContext.hpp"
#include "../include/ManapiUtils.hpp"

manapi::bytebuffer::bytebuffer() {
    this->m_src = nullptr;
    this->m_s = 0;
    this->m_shift = 0;
    this->m_reserved = 0;
}

bool manapi::bytebuffer::operator==(const std::nullptr_t &) const {
    return this->m_src == nullptr;
}

manapi::bytebuffer::bytebuffer(void *m_src, std::size_t size) {
    this->m_src = static_cast <uint8_t *> (m_src);
    this->m_s = static_cast<uint32_t>(size);
    this->m_reserved = static_cast<uint32_t>(size);
    this->m_shift = 0;
}

manapi::bytebuffer::bytebuffer(void *m_src, std::size_t size, std::size_t m_reserved) {
    this->m_src = static_cast <uint8_t *> (m_src);
    this->m_s = static_cast<uint32_t>(size);
    this->m_reserved = static_cast<uint32_t>(m_reserved);
    this->m_shift = 0;
}

manapi::bytebuffer::~bytebuffer() {
    this->clear();
}

manapi::bytebuffer::bytebuffer(bytebuffer &&n) MANAPIHTTP_NOEXCEPT {
    this->m_s = std::exchange(n.m_s, 0);
    this->m_reserved = std::exchange(n.m_reserved, 0);
    this->m_src = std::exchange(n.m_src, nullptr);
    this->m_shift = std::exchange(n.m_shift, 0);
}

manapi::bytebuffer & manapi::bytebuffer::operator=(bytebuffer &&n) MANAPIHTTP_NOEXCEPT {
    this->clear();

    this->m_s = std::exchange(n.m_s, 0);
    this->m_reserved = std::exchange(n.m_reserved, 0);
    this->m_src = std::exchange(n.m_src, nullptr);
    this->m_shift = std::exchange(n.m_shift, 0);
    return *this;
}

char * manapi::bytebuffer::data() {
    return reinterpret_cast<char *> (this->m_src) + this->m_shift;
}

char &manapi::bytebuffer::operator[](std::size_t i_) {
    return this->at(i_);
}

char & manapi::bytebuffer::at(std::size_t i_) {
    return *(this->data() + i_);
}

const char * manapi::bytebuffer::data() const {
    return reinterpret_cast<char *> (this->m_src) + this->m_shift;
}

manapi::bytebuffer::operator bool() const {
    return !!this->m_src;
}

std::size_t manapi::bytebuffer::size() const {
    return this->m_s - this->m_shift;
}

std::size_t manapi::bytebuffer::realsize() const {
    return this->m_reserved;
}

manapi::status manapi::bytebuffer::realresize(std::size_t s) MANAPIHTTP_NOEXCEPT {

    if (this->m_s == s)
        return status_ok();

    if (this->m_reserved >= s) {
        this->m_s = static_cast<uint32_t>(s);
        return status_ok();
    }

    auto z = manapi::async::memory_fabric();
    s -= this->m_shift;

    /* is slice */
    if (z->mem_type(static_cast<std::size_t>(this->m_reserved)) == z->mem_type(s)) {
        ::memmove(this->m_src, this->m_src + this->m_shift, static_cast<std::size_t>(this->m_reserved) - this->m_shift);
        this->m_s = static_cast<uint32_t>(s);
    }
    else {
        if (s) {
            auto bf = manapi::async::memory_fabric()->buffer(s).unwrap();

            ::memcpy(bf.data(), this->m_src + this->m_shift, static_cast<std::size_t>(this->m_s) - this->m_shift);
            manapi::async::memory_fabric()->free(this->m_src, static_cast<std::size_t>(this->m_reserved));

            this->m_s = static_cast<uint32_t>(s);
            this->m_reserved = static_cast<uint32_t>(bf.realsize());
            this->m_src = static_cast<uint8_t *>(bf.release());

            assert(this->m_reserved >= this->m_s);
        }
        else {
            z->free(this->m_src, static_cast<std::size_t>(this->m_reserved));
            this->m_src = nullptr;
            this->m_reserved = 0;
            this->m_s = 0;
        }
    }

    this->m_shift = 0;

    return status_ok();
}

manapi::status manapi::bytebuffer::resize(std::size_t s) MANAPIHTTP_NOEXCEPT {
    return this->realresize(s + this->m_shift);
}

manapi::status manapi::bytebuffer::resize_max(std::size_t s) MANAPIHTTP_NOEXCEPT {
    return this->resize(std::max<std::size_t>(s, this->realsize()));
}

void manapi::bytebuffer::reset_shift() MANAPIHTTP_NOEXCEPT {
    this->m_shift = 0;
}

void manapi::bytebuffer::clear() MANAPIHTTP_NOEXCEPT {
    if (this->m_src && this->m_reserved)
        manapi::async::current()->memory_fabric()->free(reinterpret_cast<void*>(this->m_src), this->m_reserved);

    this->m_src = nullptr;
    this->m_s = 0;
    this->m_reserved = 0;
    this->m_shift = 0;
}

void * manapi::bytebuffer::release() {
    this->m_s = 0;
    this->m_reserved = 0;
    this->m_shift = 0;
    return std::exchange(this->m_src, nullptr);
}

std::size_t manapi::bytebuffer::shift() const {
    return this->m_shift;
}

void manapi::bytebuffer::shift(std::size_t n) {
    this->m_shift = static_cast<uint32_t>(n);
}

void manapi::bytebuffer::shift_add(std::size_t n) {
    this->m_shift += static_cast<uint32_t>(n);
}

bool manapi::bytebuffer::empty() const {
    return !this->size();
}
