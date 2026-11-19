/**
 * @file std/ManapiSlice.hpp
 * @brief provides slice structures and utils
 *
 * @author Timur Zajnullin
 */

#pragma once

#include "./ManapiBuffer.hpp"
#include "../std/ManapiChain.hpp"
#include "../ManapiEventStructures.hpp"
#include "../ManapiUtils.hpp"
#include "../ManapiErrors.hpp"

namespace manapi {
    class slice_base;

    struct slice_part_t {
        manapi::ev::buff_t buff;
        slice_part_t *next;
    };

    class slice_iterator {
    public:
        slice_iterator (slice_part_t *part, slice_base *base);

        slice_iterator (const slice_iterator &n);

        slice_iterator &operator=(const slice_iterator &n);

        ~slice_iterator();

        slice_iterator &operator++();

        slice_iterator &operator++(int);

        bool operator==(const slice_iterator &n) const;

        /**
         * access the storage buffer via a pointer
         * @return the storage buffer pointer
         */
        void *buffer ();

        std::string_view operator *();

        /**
         * get the storage buffer size
         * @return the storage buffer size
         */
        MANAPICORE_NODISCARD std::size_t size () const;

        /**
         * is it the last in chain
         * @return true if it's the last in the chain; otherwise, returns false
         */
        MANAPICORE_NODISCARD bool is_last () const;
    private:
        slice_part_t *m_part;

        slice_base *m_base;
    };


    class slice_const_iterator {
    public:
        slice_const_iterator (slice_part_t *part, slice_base const *base);

        slice_const_iterator (const slice_const_iterator &n);

        slice_const_iterator &operator=(const slice_const_iterator &n);

        ~slice_const_iterator();

        slice_const_iterator &operator++();

        slice_const_iterator &operator++(int);

        std::string_view operator *();

        bool operator==(const slice_const_iterator &n) const;

        /**
         * access the storage buffer via a pointer
         * @return the storage buffer pointer
         */
        const void *buffer ();

        /**
         * get the storage buffer size
         * @return the storage buffer size
         */
        MANAPICORE_NODISCARD std::size_t size () const;

        /**
         * is it the last in chain
         * @return true if it's the last in the chain; otherwise, returns false
         */
        MANAPICORE_NODISCARD bool is_last () const;
    private:
        slice_part_t *m_part;
        slice_base const *m_base;
    };

    struct slice_data_t {
        std::size_t m_size;
        std::size_t m_shift;
        std::size_t m_rshift;
        slice_part_t *m_first;
        slice_part_t *m_last;
        uint32_t m_count;
    };

    class slice_base : protected slice_data_t {
        friend class slice;
    public:
        struct slice_part_deleter { void operator () (slice_part_t *ptr); };

        slice_base (std::unique_ptr<slice_part_t, slice_part_deleter> buffs, slice_part_t*last, uint32_t nbuff);

        slice_base (std::unique_ptr<slice_part_t, slice_part_deleter> buffs, slice_part_t*last, uint32_t nbuff, std::size_t rshift);

        slice_base (std::unique_ptr<slice_part_t, slice_part_deleter> buffs, slice_part_t*last, uint32_t nbuff, std::size_t shift, std::size_t rshift, std::size_t size);

        slice_base (slice_part_t *first, slice_part_t *last, uint32_t count, std::size_t shift, std::size_t rshift, std::size_t size);

        virtual ~slice_base();

        slice_base (slice_base &&n) MANAPICORE_NOEXCEPT;

        slice_base &operator=(slice_base &&n) MANAPICORE_NOEXCEPT;

        slice_base (const slice_base &n);

        slice_base &operator=(const slice_base &n);

        /**
         * Adjusts the left shift by |shift|
         *
         * WARNING: This operation is irreversible
         *
         * @param shift left shift to add
         * @return Ok if success; otherwise it returns OutOfRange
         */
        virtual manapi::status shift_add (std::size_t shift) MANAPICORE_NOEXCEPT;

        manapi::status copy_from (const void *buffer, std::size_t shift, std::size_t size) MANAPICORE_NOEXCEPT;

        manapi::status copy_from (const void *buffer, const slice_part_t *current, std::size_t shift, std::size_t size) MANAPICORE_NOEXCEPT;

        manapi::status copy_to (void *buffer, std::size_t shift, std::size_t size) MANAPICORE_NOEXCEPT;

        manapi::status copy_from (const slice_base &n, std::size_t shift, std::size_t shift_n, std::size_t size) MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD manapi::status_or<manapi::slice_base> subslice (std::size_t pos, std::size_t size) const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD manapi::status_or<manapi::slice_base> subslice (std::size_t pos) const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::size_t shift () const { return this->m_shift; }

        MANAPICORE_NODISCARD std::size_t rshift () const { return this->m_rshift; }

        MANAPICORE_NODISCARD bool empty () const { return this->size() == 0; }

        MANAPICORE_NODISCARD const slice_part_t *slices_begin () const;

        MANAPICORE_NODISCARD const slice_part_t *slices_end () const;

        MANAPICORE_NODISCARD const slice_part_t *slices_rbegin () const;

        slice_iterator begin ();

        slice_iterator end ();

        MANAPICORE_NODISCARD slice_const_iterator begin () const;

        MANAPICORE_NODISCARD slice_const_iterator end () const;

        MANAPICORE_NODISCARD int cmp (const manapi::slice_base &n) const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD int cmp (const void *data, std::size_t size) const MANAPICORE_NOEXCEPT;

        void slices_buffs (ev::buff_t *buffs, std::size_t cnt, std::size_t *sz = nullptr) const;

        MANAPICORE_NODISCARD std::unique_ptr<ev::buff_t, ev::buffer_deleter> slices_buffs (std::size_t max_cnt, std::size_t *cnt = nullptr, std::size_t *sz = nullptr) const;

        MANAPICORE_NODISCARD uint32_t slices_size () const { return this->m_count; }

        MANAPICORE_NODISCARD std::size_t size () const;

        MANAPICORE_NODISCARD std::string to_string () const;
    };

    class slice final : public slice_base {
    public:
        slice ();

        static manapi::status_or<slice> create (std::size_t n) MANAPICORE_NOEXCEPT;

        slice (std::unique_ptr<slice_part_t, slice_part_deleter> buffs,slice_part_t*last, uint32_t nbuff);

        slice (std::unique_ptr<slice_part_t, slice_part_deleter> buffs,slice_part_t*last,  uint32_t nbuff, std::size_t rshift);

        slice (std::unique_ptr<slice_part_t, slice_part_deleter> buffs,slice_part_t*last, uint32_t nbuff, std::size_t shift, std::size_t rshift, std::size_t size);

        slice (slice_part_t *first, slice_part_t *last, uint32_t count, std::size_t shift, std::size_t rshift, std::size_t size);

        slice (std::string_view n);

        slice (const char* n);

        slice (const char *buffer, std::size_t sz);

        slice (const ev::buff_t *buff);

        slice (slice &&n) MANAPICORE_NOEXCEPT;

        slice &operator=(slice &&n) MANAPICORE_NOEXCEPT;

        ~slice() override;

        /**
         * Resize the slices chain capacity
         * @param size capacity
         * @return Ok if success; otherwise, ResourceExhausted
         */
        manapi::status resize (std::size_t size) MANAPICORE_NOEXCEPT;

        /**
         * Adds |buffer| to the slices chain
         * @param buffer bytebuffer
         * @return Ok if success; otherwise, ResourceExhausted
         */
        manapi::status push_back (bytebuffer buffer) MANAPICORE_NOEXCEPT;

        /**
         * Adds |s| to the slices chain
         * @param s slices
         * @return Ok if success; otherwise, InternalError
         */
        manapi::status push_back (slice &&s) MANAPICORE_NOEXCEPT;

        /**
         * Moves |buff| to the slices chain
         * @param buff buffer
         * @return Ok if success, otherwise, ResourceExhausted
         */
        manapi::status push_back (const ev::buff_t *buff) MANAPICORE_NOEXCEPT;

        /**
         * Copy |buffer| to the back of the slices chain
         * @param buffer buffer pointer
         * @param size buffer size
         * @return Ok if success, otherwise, ResourceExhausted
         */
        manapi::status push_back (const void *buffer, std::size_t size) MANAPICORE_NOEXCEPT;

        /**
         * Adds |buffer| to the slices chain
         * @param buffer buffer
         * @return Ok if success, otherwise, ResourceExhausted
         */
        manapi::status push_back (std::string_view buffer) MANAPICORE_NOEXCEPT;

        /**
         * Adjusts the left shift by |shift| and clears free buffers
         *
         * WARNING: This operation is irreversible
         *
         * @param shift left shift to add
         * @return Ok if success; otherwise it returns OutOfRange
         */
        manapi::status shift_add (std::size_t shift) MANAPICORE_NOEXCEPT override;

        manapi::status_or<manapi::slice> split (std::size_t start, std::size_t size) MANAPICORE_NOEXCEPT;

        void clear () MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD manapi::slice copy () const;
    };

    class slice_view final : public slice_base {
    public:
        slice_view ();

        slice_view (slice_part_t *part);

        slice_view (slice_part_t *first, slice_part_t *last, uint32_t count, std::size_t shift, std::size_t rshift, std::size_t size);

        slice_view (const slice_base &n);

        slice_view (const slice &n);

        slice_view (const slice_view &n);

        slice_view &operator=(const slice_view &n);

        ~slice_view() override;
    };

    class slice_ref final : public slice_base {
    public:
        slice_ref ();

        slice_ref (const slice_ref &n);

        slice_ref &operator=(const slice_ref &n);

        slice_ref (slice_ref &&n) MANAPICORE_NOEXCEPT;

        slice_ref &operator=(slice_ref &&n) MANAPICORE_NOEXCEPT;

        ~slice_ref() override;

        manapi::status push_back (const void *buffer, std::size_t size);

        manapi::status shift_add(std::size_t shift) MANAPICORE_NOEXCEPT override;

        void clear () MANAPICORE_NOEXCEPT;
    };

    class slice_complex {
    public:
        union st_t {
            manapi::slice s;
            manapi::slice_view sv;

            st_t();

            ~st_t();
        };

        slice_complex ();

        ~slice_complex ();

        slice_complex ( manapi::slice &&n );

        slice_complex ( manapi::slice_view &&n );

        slice_complex (manapi::slice_complex &&n) MANAPICORE_NOEXCEPT;

        slice_complex &operator= ( manapi::slice &&n ) MANAPICORE_NOEXCEPT;

        slice_complex &operator= ( manapi::slice_view &&n ) MANAPICORE_NOEXCEPT;

        slice_complex &operator= (manapi::slice_complex &&n ) MANAPICORE_NOEXCEPT;

        slice_base *base ( ) MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD const slice_base *base ( ) const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD bool is_slice () const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD bool is_slice_view () const MANAPICORE_NOEXCEPT;

        slice *get_slice () MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD const slice *get_slice () const MANAPICORE_NOEXCEPT;

        slice_view *get_slice_view () MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD const slice_view *get_slice_view () const MANAPICORE_NOEXCEPT;

        slice_complex &to_slice ();
    private:
        uint8_t m_flags;
        st_t m_st;
    };
}
