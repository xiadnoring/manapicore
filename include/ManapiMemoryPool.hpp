#pragma once

#include <mutex>

#include "./ManapiErrors.hpp"
#include "./ManapiAsync.hpp"
#include "./std/ManapiSlice.hpp"
#include "./std/ManapiBuffer.hpp"
#include "./std/ManapiChain.hpp"

namespace manapi {
    struct object_pool_data_t;

    class object_item_pool_root {
    public:
        object_item_pool_root ();

        object_item_pool_root (void *p);

        virtual ~object_item_pool_root ();

        object_item_pool_root (object_item_pool_root &&n) MANAPICORE_NOEXCEPT;

        object_item_pool_root& operator= (object_item_pool_root &&n) MANAPICORE_NOEXCEPT;

        operator bool() const MANAPICORE_NOEXCEPT {
            return !!this->object;
        }
    protected:
        void *object;
    };

    template<typename T>
    class object_item_pool : public object_item_pool_root {
    public:
        object_item_pool () : object_item_pool_root() {}

        ~object_item_pool() override;

        object_item_pool (object_item_pool &&n) MANAPICORE_NOEXCEPT = default;

        object_item_pool &operator=(object_item_pool &&n) MANAPICORE_NOEXCEPT = default;

        T *operator->() {
            return this->object;
        }

        T &operator*() {
            return *this->object;
        }

        T *release () {
            return std::exchange(this->object, nullptr);
        }
    };

    class object_pool {
        std::shared_ptr<object_pool_data_t> data;
    public:
        typedef std::unique_ptr<manapi::slice> item;

        object_pool();

        ~object_pool();

        //manapi::slice slice (std::size_t min, std::size_t max);

        manapi::status_or<manapi::slice> slice (std::size_t suggested);

        manapi::status_or<manapi::bytebuffer> buffer (std::size_t min, std::size_t max);

        manapi::status_or<manapi::bytebuffer> buffer (std::size_t suggested);

        manapi::bytebuffer buffer (void *pointer, std::size_t suggested);

        void free (void *ptr, std::size_t size) MANAPICORE_NOEXCEPT;

        void clear ();

        static int mem_type (std::size_t size) MANAPICORE_NOEXCEPT;

        static constexpr std::size_t area_size () { return 4096; }

        template<typename T, typename ...Args>
        object_item_pool<T> get (Args&&...args) {
            auto b = this->buffer (sizeof (T)).unwrap();
            auto const p = new(b.data()) T (std::forward<decltype(args)>(args)...);
            b.release(); return object_item_pool<T> (p);
        }
    };

    template<typename T>
    manapi::object_item_pool<T>::~object_item_pool() {
        manapi::async::memory_fabric()->free(this->object, sizeof (T));
    }
}
