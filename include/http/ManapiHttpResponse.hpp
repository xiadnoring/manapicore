#pragma once
//
//#include <string>
//#include <map>
//
//#include "./ManapiHttpConfig.hpp"
//#include "./ManapiFormData.hpp"
//#include "./ManapiHttpUtils.hpp"
//#include "../ManapiUtils.hpp"
//#include "../ManapiAsync.hpp"
//#include "../ManapiFetch.hpp"
//#include "../json/ManapiJson.hpp"
//
//namespace manapi::net::http {
//    namespace internal {
//        struct handle_data_t;
//
//        struct file_fd_t;
//    }
//
//    struct custom_data_t {
//        void *src = nullptr;
//        std::move_only_function <void(void *)> clean;
//    };
//
//    struct custom_data_deleter_t {
//        void operator()(custom_data_t *n);
//    };
//
//    class response_stream {
//    public:
//        response_stream ( manapi::future<ssize_t> (*cb)( manapi::net::http::response *, manapi::slice_view, bool ), manapi::net::http::response *resp);
//
//        manapi::future<ssize_t> fwrite ( manapi::slice_view buffs, bool fin);
//
//        manapi::net::http::request &req ();
//    private:
//        manapi::net::http::response *m_resp;
//        manapi::future<ssize_t> (*m_cb)( manapi::net::http::response *, manapi::slice_view, bool );
//    };
//
//    class response {
//        friend class uresponse;
//    public:
//        using resp_callback_sync = std::move_only_function<ssize_t(char *buffer, std::size_t size, bool&)>;
//        using resp_callback_async = std::move_only_function<manapi::future<ssize_t>(slice_view buffs, bool&)>;
//        using resp_stream = std::move_only_function<manapi::future<>(response_stream stream)>;
//#ifdef MANAPICORE_FETCH_SUPPORT
//        using resp_proxy_setup_cb = std::move_only_function<void(const std::shared_ptr<manapi::net::fetch> &)>;
//#endif
//
//        response (internal::handle_data_t* cdata, uint16_t status, std::unique_ptr<http::request> req);
//
//        ~response ();
//
//        manapi::status compress (std::string name) MANAPICORE_NOEXCEPT;
//
//        void compress_enabled (bool state) MANAPICORE_NOEXCEPT;
//
//        manapi::status text (std::string plain_text) MANAPICORE_NOEXCEPT;
//
//        manapi::status json (manapi::json data, size_t spaces = 0) MANAPICORE_NOEXCEPT;
//
//        manapi::status form (formdata_send formdata) MANAPICORE_NOEXCEPT;
//
//        void status (uint16_t status_code) MANAPICORE_NOEXCEPT;
//
//        void status_code (uint16_t status_code) MANAPICORE_NOEXCEPT;
//
//        manapi::status replacers (std::vector<std::pair<std::string, std::string>> replacers) MANAPICORE_NOEXCEPT;
//
//        void partial_enabled (bool state) MANAPICORE_NOEXCEPT;
//
//        manapi::status file (std::string path) MANAPICORE_NOEXCEPT;
//
//        manapi::status fd (ev::unique_file fd, std::string path = {}) MANAPICORE_NOEXCEPT;
//
//        manapi::status slice (manapi::slice sv) MANAPICORE_NOEXCEPT;
//
//#ifdef MANAPICORE_FETCH_SUPPORT
//        manapi::status proxy (std::string url) MANAPICORE_NOEXCEPT;
//
//        manapi::status proxy (std::string url, resp_proxy_setup_cb cb) MANAPICORE_NOEXCEPT;
//#endif
//
//        manapi::status callback_sync (resp_callback_sync cb) MANAPICORE_NOEXCEPT;
//
//        manapi::status callback_async (resp_callback_async cb) MANAPICORE_NOEXCEPT;
//
//        manapi::status callback_stream (resp_stream cb) MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD uint16_t status_code () const MANAPICORE_NOEXCEPT;
//
//        std::string_view status_message () MANAPICORE_NOEXCEPT;
//
//        std::map<std::string, std::string, std::less<>> &headers () MANAPICORE_NOEXCEPT;
//
//        manapi::status header (const std::string &key, std::string value) MANAPICORE_NOEXCEPT;
//
//        void remove_header (std::string_view key) MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool contains_header (std::string_view key) const MANAPICORE_NOEXCEPT;
//
//        status_or<std::string_view> header (std::string_view key) MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_file () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_text () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_proxy () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_no_data () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_formdata () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_async_cb() const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_sync_cb() const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool is_slice () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool partial_enabled () const MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD int data_type() const MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<internal::file_fd_t *> file () MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<std::string *> text () MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<std::string *> url () MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<manapi::slice *> slice () MANAPICORE_NOEXCEPT;
//
//        MANAPICORE_NODISCARD bool contains_ranges () MANAPICORE_NOEXCEPT;
//
//        std::unique_ptr<std::vector<std::pair<std::size_t, std::size_t>>> ranges () MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<formdata_send *> formdata () MANAPICORE_NOEXCEPT;
//#ifdef MANAPICORE_FETCH_SUPPORT
//        std::unique_ptr<resp_proxy_setup_cb> &proxy_setup_cb () MANAPICORE_NOEXCEPT;
//#endif
//        std::string compress () MANAPICORE_NOEXCEPT;
//
//        std::unique_ptr<std::vector <std::pair<std::string, std::string>>> &replacers () MANAPICORE_NOEXCEPT;
//
//        manapi::status custom_data (custom_data_t data) MANAPICORE_NOEXCEPT;
//
//        const std::shared_ptr<http::config> &config () MANAPICORE_NOEXCEPT;
//
//        struct custom_data_t *custom_data () MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<resp_callback_async*> callback_async() MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<resp_callback_sync*> callback_sync() MANAPICORE_NOEXCEPT;
//
//        manapi::status_or<resp_stream *> callback_stream() MANAPICORE_NOEXCEPT;
//
//        request_data_t *request_data () MANAPICORE_NOEXCEPT;
//
//        http::request *req () MANAPICORE_NOEXCEPT;
//
//        internal::handle_data_t *connection_data () MANAPICORE_NOEXCEPT;
//
//        internal::handle_data_t *connection_data_release () MANAPICORE_NOEXCEPT;
//
//        template<typename T>
//        T *custom_data_as () MANAPICORE_NOEXCEPT {
//            return static_cast<T*>(this->custom_data()->src);
//        }
//    private:
//        void finish () MANAPICORE_NOEXCEPT;
//
//        manapi::status finish (std::unique_ptr<std::move_only_function<void(std::exception_ptr)>> cb) MANAPICORE_NOEXCEPT;
//
//        manapi::status check_type_ (int type) MANAPICORE_NOEXCEPT;
//
//        std::string &body () MANAPICORE_NOEXCEPT;
//
//        // detect the range header
//        void detect_ranges () MANAPICORE_NOEXCEPT;
//
//        uint8_t m_type;
//
//        uint16_t m_status_code;
//
//        uint8_t m_flags;
//
//        std::unique_ptr<std::string> m_compress;
//
//        std::map<std::string, std::string, std::less<>> m_headers;
//
//        std::unique_ptr<std::vector <std::pair <std::size_t , std::size_t> > > m_ranges;
//
//        // custom data for layers
//        std::unique_ptr<custom_data_t, custom_data_deleter_t> m_custom_data;
//
//        std::unique_ptr<std::vector<std::pair<std::string, std::string>>> m_replacers;
//
//        std::unique_ptr<http::request> m_req;
//
//        internal::handle_data_t *m_cdata;
//
//        std::unique_ptr<std::move_only_function<void(std::exception_ptr err)>> m_finish_cb;
//
//#ifdef MANAPICORE_FETCH_SUPPORT
//        std::unique_ptr<std::move_only_function<void(const std::shared_ptr<manapi::net::fetch> &)>> m_proxy_setup;
//#endif
//
//        void *m_data;
//    };
//
//    class uresponse {
//    public:
//        uresponse (response *resp, std::unique_ptr<std::move_only_function<void(std::exception_ptr)>> cb);
//
//        ~uresponse();
//
//        uresponse (uresponse &&n) MANAPICORE_NOEXCEPT;
//
//        uresponse &operator=(uresponse &&n) MANAPICORE_NOEXCEPT;
//
//        response *operator->() MANAPICORE_NOEXCEPT;
//
//        const response *operator->() const MANAPICORE_NOEXCEPT;
//
//        response &operator*() MANAPICORE_NOEXCEPT;
//
//        const response &operator *() const MANAPICORE_NOEXCEPT;
//
//        void finish () MANAPICORE_NOEXCEPT;
//    private:
//        response *m_resp;
//    };
//
//}
