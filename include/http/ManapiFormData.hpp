#pragma once

#include <map>
#include <string>
#include <memory>

#include "../ManapiUtils.hpp"
#include "../ManapiAsync.hpp"
#include "../std/ManapiCancelToken.hpp"
#include "../std/ManapiContext.hpp"

namespace manapi::net {
    struct file_data_t {
        std::string file_name;
        std::string mime_type;
        std::string param_name;
    };

    class formdata_recv {
        struct formdata_recv_headers_t;

        struct formdata_recv_ctx_t {
            int current;
            int next;

            uint32_t n1;
            uint32_t n2;

            std::string boundary;

            std::unique_ptr<formdata_recv_headers_t> hctx;
            std::unique_ptr<std::map<std::string, std::string, std::less<>>> headers;
        };
    public:
        typedef std::move_only_function<manapi::future<ssize_t> (slice_view buffs, bool fin)> ondata_cb_t;

        typedef std::move_only_function<ondata_cb_t(std::string name)> onparam_cb_t;

        typedef std::move_only_function<manapi::future<ssize_t>(slice_view buffs, bool fin)> req_data_cb_t;

        typedef std::move_only_function<manapi::future<manapi::status>(req_data_cb_t handler)> onrecv_cb_t;

        formdata_recv (onrecv_cb_t onrecv_cb);

        ~formdata_recv ();

        formdata_recv (formdata_recv &&n) MANAPICORE_NOEXCEPT;

        formdata_recv &operator=(formdata_recv &&n) MANAPICORE_NOEXCEPT;

        manapi::future<manapi::status> get (std::string_view content_type, onparam_cb_t cb);

        static ondata_cb_t save_file (std::string file, int mode = ev::IRUSR|ev::IWUSR|ev::IRGRP|ev::IROTH, ssize_t maxlen = -1, manapi::ctoken cancellation = nullptr);

        static ondata_cb_t save_string (std::string *str, ssize_t maxlen = -1);

        static ondata_cb_t skip (ssize_t maxlen = -1);
    private:
        manapi::future<ssize_t> onrecv_multipart_ (slice_view buffs);
        manapi::future<ssize_t> onrecv_urlencoded_ (slice_view buffs);

        onparam_cb_t m_onparam_cb;
        ondata_cb_t m_ondata_cb;
        onrecv_cb_t m_onrecv_cb;
        formdata_recv_ctx_t m_ctx;
    };

    class formdata_send {
    public:
        formdata_send ();

        ~formdata_send ();

        formdata_send (formdata_send &&n) MANAPICORE_NOEXCEPT;

        formdata_send& operator= (formdata_send &&n) MANAPICORE_NOEXCEPT;

        manapi::status set_file (const std::string &name, std::string filepath) MANAPICORE_NOEXCEPT;

        manapi::status set_file (const std::string &name, std::string filepath, std::string filename, std::string filemime) MANAPICORE_NOEXCEPT;

        manapi::status set_text (const std::string &name, std::string data) MANAPICORE_NOEXCEPT;

        void erase (std::string_view name) MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD bool contains (std::string_view name) const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD manapi::future<manapi::status_or<uint64_t>> payload_size () const;

        MANAPICORE_NODISCARD manapi::status_or<uint64_t> multipart_size (std::size_t boundary_size) const MANAPICORE_NOEXCEPT;

        MANAPICORE_NODISCARD std::string generate_boundary () const;

        manapi::future<manapi::status> data2multipart (std::string boundary, std::move_only_function<manapi::future<manapi::status>(manapi::slice_view, bool fin)> write);
    private:
        struct data_file_storage {
            std::string filename;
            std::string filemime;
        };

        struct data_storage {
            int type;
            std::string data;
            data_file_storage file;
        };

        std::unordered_map<std::string, std::vector<data_storage>, manapi::text_hash, std::equal_to<>> data{};
    };
}
