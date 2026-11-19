#pragma once
//
//#include <memory>
//#include <functional>
//
//#include "../http/ManapiHttpConfig.hpp"
//#include "../http/ManapiHttpCtx.hpp"
//#include "../std/ManapiCancelToken.hpp"
//#include "../std/ManapiRef.hpp"
//#include "../std/ManapiStopToken.hpp"
//#include "../ManapiUtils.hpp"
//
//namespace manapi::net::worker {
//    class base_http;
//}
//
//namespace manapi::net::worker {
//    struct sockaddr_st {
//        char data[std::max(sizeof (struct sockaddr_in), sizeof (struct sockaddr_in6))];
//    };
//
//    template<typename T>
//    void http_wrk_data_deleter (void *pointer) {
//        delete static_cast<T *>(pointer);
//    }
//
//    enum wrk_interface_flags {
//        WRK_INTERFACE_RESERVED = 1,
//        WRK_INTERFACE_CUSTOM_RATE_LIMIT = 2,
//        WRK_INTERFACE_CONN_RETRY = 4,
//        WRK_INTERFACE_TCP_KEEP_ALIVE = 8,
//        WRK_INTERFACE_IS_STREAM = 16,
//        WRK_INTERFACE_CUSTOM_READ = 32,
//        WRK_INTERFACE_IS_DRAINING = 64,
//        WRK_INTERFACE_IS_CTRL = 128,
//        WRK_INTERFACE_RESERVED3 = 256
//    };
//
//    enum worker_base_flags {
//        WORKER_BASE_FLAG_MULTISTREAM = 1,
//        WORKER_BASE_FLAG_RESERVED = 2,
//        WORKER_BASE_FLAG_AUTO_ACK = 4,
//        WORKER_BASE_FLAG_RESERVED2 = 8,
//        WORKER_BASE_FLAG_MAX = WORKER_BASE_FLAG_RESERVED2
//    };
//
//    enum close_flags_t {
//        CLOSE_CONN_EOR = 1,
//        CLOSE_CONN_ERR = 2,
//        CLOSE_CONN_SHUTDOWN = 4,
//        CLOSE_CONN_FINISHED = 8,
//        CLOSE_CONN_EOS = 16
//    };
//
//    enum wrk_global_flags {
//        WRK_GLOBAL_FLAG_MULTISTREAM = 1,
//        WRK_GLOBAL_FLAG_SHUTDOWN_SUPPORTED = 2
//    };
//
//    struct wrk_interface_t {
//        uint32_t flags;
//        void *data;
//    };
//
//    class connection {
//    public:
//        struct ipdata_t {
//            worker::sockaddr_st client;
//            socklen_t len;
//        };
//
//        connection (void *ptr, void (*deleter)(connection *data) = nullptr);
//
//        ~connection();
//
//        template <typename T>
//        T *as () { assert (this->ptr); return static_cast <T *> (this->ptr); }
//
//        manapi::ctoken cancellation;
//
//        std::unique_ptr<ipdata_t> ipdata;
//
//        int version = http::versions::HTTP_v1_1;
//
//        wrk_interface_t wrk;
//
//        int refcnt;
//    private:
//        void *ptr;
//
//        void (*deleter)(connection *data);
//    };
//
//    using shared_conn = reference <worker::connection>;
//    using ibuffpool_t = bytebuffer;
//    using worker_watcher_cb = std::move_only_function<void(const shared_conn &conn, int flags, manapi::slice_complex *sc)>;
//
//    /**
//     * provides API callbacks to workers
//     */
//    struct wrk_interface_global_t {
//        uint64_t flags;
//
//        void *data;
//
//        /**
//         * no needed
//         */
//        uint64_t (*flags_cb)(const shared_conn &conn, wrk_interface_global_t *global) MANAPICORE_NOEXCEPT;
//
//        /**
//         * required, if the worker interface asks for an alpn support
//         */
//        int (*alpn_cb)(wrk_interface_global_t *global, char const *alpn, std::size_t alpn_size) MANAPICORE_NOEXCEPT;
//
//        /**
//         * required
//         */
//        int (*init_cb)(const worker::shared_conn &conn, wrk_interface_global_t *global, worker::base *w) MANAPICORE_NOEXCEPT;
//
//        /**
//         * required if the worker interface asks for a multistream support
//         */
//        int (*init_stream_cb)(const worker::shared_conn &conn, const worker::shared_conn &stream, wrk_interface_global_t *global, worker::base *w) MANAPICORE_NOEXCEPT;
//
//        /**
//         * required
//         */
//        int (*cleanup_cb)(worker::connection *conn, wrk_interface_global_t *global) MANAPICORE_NOEXCEPT;
//
//        /**
//         * required
//         */
//        int (*cleanup_global_cb)(wrk_interface_global_t *data) MANAPICORE_NOEXCEPT;
//
//        /**
//         * not required
//         */
//        void (*custom_read_cb)(const worker::shared_conn &conn, manapi::slice_complex *sc, wrk_interface_global_t *global) MANAPICORE_NOEXCEPT;
//
//        /**
//         * required if the worker interface asks for a custom limit rate callback
//         */
//        void (*update_limit_rate_cb)(const manapi::net::worker::shared_conn &conn, wrk_interface_global_t *global) MANAPICORE_NOEXCEPT;
//
//        /**
//         * required
//         */
//        int (*custom_cb)(const manapi::net::worker::shared_conn &conn, wrk_interface_global_t *global, void* data);
//
//        /**
//         * required, if worker interface asks for a shutdown support
//         * if |shutdown_cb| exists it must call close_connection() after a couple of RTTs
//         */
//        int (*shutdown_cb)(const worker::shared_conn &conn, wrk_interface_global_t *global, bool force) MANAPICORE_NOEXCEPT;
//    };
//
//    struct connection_io {
//        manapi::slice send;
//        manapi::slice recv;
//
//        std::size_t pending_send;
//    };
//
//    enum event_on_flags {
//        EVENT_ON_READ = manapi::ev::READ,
//        EVENT_ON_WRITE = manapi::ev::WRITE,
//        EVENT_ON_AUTO_REMOVE = 4
//    };
//
//    struct connection_base_t;
//
//    class base : public std::enable_shared_from_this<base> {
//    public:
//
//        typedef std::unordered_map<uintptr_t, shared_conn> conn_by_port;
//
//        typedef std::unordered_map<std::string, conn_by_port, manapi::text_hash, std::equal_to<>> conns_by_ip;
//
//        enum connection_status {
//            CONN_READ           = manapi::ev::READ,
//            CONN_WRITE          = manapi::ev::WRITE,
//            CONN_CLOSED         = 4,
//            CONN_REMOVED        = 8,
//            CONN_RECV_END       = 16,
//            CONN_SEND_END       = 32,
//            CONN_IO_WAITING     = 64,
//            CONN_RESERVED       = 128,
//            CONN_TOP_READ       = 128,
//            CONN_EVENT_LOCKED   = 256,
//            CONN_WANT_CLOSE     = 512,
//            CONN_WAS_SHUTDOWN   = 1024,
//            CONN_EVENT_FORCE    = 2048,
//            CONN_MAX_CODE       = 2048,
//
//            CONN_MASK_UPDATE    = CONN_READ | CONN_WRITE | CONN_WANT_CLOSE | CONN_EVENT_FORCE,
//            CONN_MASK_GETTING   = CONN_READ | CONN_WRITE | CONN_CLOSED | CONN_RECV_END | CONN_SEND_END | CONN_WANT_CLOSE,
//        };
//
//        enum connection_io_status {
//            CONN_IO_OK = 0,
//            CONN_IO_ERROR = -1,
//            CONN_IO_WANT_READ = -1000,
//            CONN_IO_WANT_WRITE = -1001,
//            CONN_IO_AGAIN = -1002,
//        };
//
//        enum stream_flags {
//            /**
//             * says to create a new unidirectional stream
//             * instead of a bidirectional stream
//             */
//            CONN_STREAM_FLAG_UNI = 1,
//            CONN_STREAM_FLAG_CTRL = 2
//        };
//
//        base ();
//
//        virtual ~base ();
//
//        virtual void wrk_global (wrk_interface_global_t *data) = 0;
//
//        virtual wrk_interface_global_t *wrk_global () = 0;
//
//        virtual manapi::future<status> init (std::size_t deep) = 0;
//
//        virtual void close_connection (shared_conn conn, int flags) MANAPICORE_NOEXCEPT = 0;
//
//        virtual ssize_t sync_write_ex(const worker::shared_conn &conn, manapi::slice &sv, bool finish, std::size_t maxsize) MANAPICORE_NOEXCEPT = 0;
//
//        virtual ssize_t sync_write_ex(const worker::shared_conn &conn, ev::buff_t *buf, uint32_t buf_cnt, bool finish, std::size_t maxsize) MANAPICORE_NOEXCEPT = 0;
//
//        virtual ssize_t sync_write (const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) MANAPICORE_NOEXCEPT = 0;
//
//        ssize_t sync_write_ex (const shared_conn &conn, const void *buff, std::size_t size, bool finish, std::size_t maxsize) MANAPICORE_NOEXCEPT;
//
//        ssize_t sync_write (const shared_conn &conn, slice_view buffs, bool finish) MANAPICORE_NOEXCEPT;
//
//        ssize_t sync_write (const shared_conn &conn, const void *buff, std::size_t size, bool finish) MANAPICORE_NOEXCEPT;
//
//        virtual bool is_writable (const shared_conn &conn) MANAPICORE_NOEXCEPT = 0;
//
//        virtual void waiting (const shared_conn &conn, bool state) MANAPICORE_NOEXCEPT = 0;
//
//        manapi::future<ssize_t> write (const shared_conn &conn, const void *buff, std::size_t size, bool finish);
//
//        manapi::future<ssize_t> write (const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish);
//
//        manapi::future<ssize_t> write (const shared_conn &conn, manapi::slice_view buffs, bool finish);
//
//        manapi::future<ssize_t> fwrite (const shared_conn &conn, const void *buff, std::size_t size, bool finish);
//
//        manapi::future<ssize_t> fwrite (const shared_conn &conn, manapi::slice_view slice, bool finish);
//
//        manapi::future<ssize_t> fwrite (const shared_conn &conn, manapi::slice &slice, std::size_t size, bool finish);
//
//        virtual void stop (manapi::stoken token) = 0;
//
//        virtual void event_on (const shared_conn & conn, worker_watcher_cb&& callback, int flags) MANAPICORE_NOEXCEPT;
//
//        virtual void event_unsub (const shared_conn & conn) MANAPICORE_NOEXCEPT;
//
//        virtual int event_flags (const shared_conn & conn, int flags) MANAPICORE_NOEXCEPT = 0;
//
//        virtual int event_flags (const shared_conn & conn) MANAPICORE_NOEXCEPT = 0;
//
//        virtual void feed_event (const shared_conn &conn, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT = 0;
//
//        void event_toggle (const shared_conn & conn, bool state, int flag) MANAPICORE_NOEXCEPT;
//
//        /**
//         * Get the ipdata structure from the connection object |conn|
//         * @param conn connection object
//         * @return reference to the ipdata structure
//         */
//        virtual connection::ipdata_t *ipdata (worker::connection *conn) MANAPICORE_NOEXCEPT;
//
//        /**
//         * Obtain the stream identifier from the stream |s|
//         * @param s stream
//         * @return always 0 if streams not supported, otherwise it returns a stream id
//         */
//        virtual int64_t stream_id (const shared_conn & s) MANAPICORE_NOEXCEPT;
//
//        /**
//         * It returns a connection object by the stream identifier
//         * @param id stream identifier
//         * @return always nullptr if streams not supported or not found, otherwise it returns a connection object
//         */
//        virtual shared_conn stream_id (const shared_conn &conn, int64_t id) MANAPICORE_NOEXCEPT;
//
//        /**
//         * get the size of the streams in the connection
//         * @return the size of the streams if a multistream is supported; otherwise, it always returns 0
//         */
//        MANAPICORE_NODISCARD virtual std::size_t streams_size (const shared_conn &conn) const MANAPICORE_NOEXCEPT;
//
//        /**
//         * Create a new stream in |conn|
//         *
//         * @param flags stream flags
//         * @return a stream connection on success, otherwise it returns Unimplemeneted, InternalError, ResourceExhausted
//         */
//        virtual status_or<shared_conn> new_stream (const shared_conn & conn, int flags) MANAPICORE_NOEXCEPT;
//
//        std::shared_ptr<manapi::net::worker::worker_data_t> m_worker_data;
//
//        std::shared_ptr<net::worker::base_http> m_site;
//
//        std::shared_ptr<manapi::net::http::config> m_config;
//
//        int notify_cb (worker_watcher_cb &cb, const shared_conn & conn, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT;
//
//        std::vector< net::worker::worker_watcher_cb > m_removed_cbs;
//
//        uint32_t m_removed_cbs_deps;
//    protected:
//        template<typename Derived>
//        std::shared_ptr<Derived> get_shared () {
//            return std::static_pointer_cast<Derived>(this->shared_from_this());
//        }
//
//        template<typename Derived>
//        std::shared_ptr<Derived> get_weak () {
//            return std::static_pointer_cast<Derived>(this->weak_from_this());
//        }
//    };
//
//    using shared_worker = std::shared_ptr<worker::base>;
//
//}