#include "ManapiEventStructures.hpp"
#include "ManapiAsync.hpp"
#include "ManapiEventLoop.hpp"
#include "std/ManapiContext.hpp"
#include "./include/ManapiEventStructuresInternal.hpp"
#include "./include/ManapiDebug.hpp"

#ifdef _WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <io.h>
#endif

#define MANAPICORE_EV_CAST_STREAM(x) (reinterpret_cast<uv_stream_t *> (x))
#define MANAPICORE_EV_CAST_HANDLE(x) (reinterpret_cast <uv_handle_t *> (x))
#define MANAPICORE_EV_DEFAULT(name_class, name_struct) \
name_struct *manapi::ev::name_class::custom () MANAPICORE_NOEXCEPT { return &this->m_s; } \
void manapi::ev::name_class::unbind (uv_close_cb cb) MANAPICORE_NOEXCEPT {  uv_close(MANAPICORE_EV_CAST_HANDLE(&this->m_s), cb); }\
void manapi::ev::name_class::unbind () MANAPICORE_NOEXCEPT { uv_close(MANAPICORE_EV_CAST_HANDLE (&this->m_s), callback_close_cb); }\
void manapi::ev::name_class::data (void *data) MANAPICORE_NOEXCEPT { uv_handle_set_data(MANAPICORE_EV_CAST_HANDLE (&this->m_s), data);}\
void *manapi::ev::name_class::data () MANAPICORE_NOEXCEPT {return uv_handle_get_data(MANAPICORE_EV_CAST_HANDLE (&this->m_s)); } \
manapi::ev::loop_ref manapi::ev::name_class::loop () MANAPICORE_NOEXCEPT { return uv_handle_get_loop(MANAPICORE_EV_CAST_HANDLE(&this->m_s)); } \
bool manapi::ev::name_class::is_active() MANAPICORE_NOEXCEPT { return uv_is_active(MANAPICORE_EV_CAST_HANDLE(&this->m_s)); } \
void manapi::ev::name_class::unref () MANAPICORE_NOEXCEPT { return uv_unref(MANAPICORE_EV_CAST_HANDLE(&this->m_s)); } \
void manapi::ev::name_class::ref () MANAPICORE_NOEXCEPT { return uv_ref(MANAPICORE_EV_CAST_HANDLE(&this->m_s)); } \
manapi::ev::name_class::~name_class () = default;
#define MANAPICORE_EV_STREAM(name_class, name_struct) \
int manapi::ev::name_class::listen (int tcp_backlog, uv_connection_cb cb) MANAPICORE_NOEXCEPT {  return uv_listen(MANAPICORE_EV_CAST_STREAM(&this->m_s), tcp_backlog, cb); } \
int manapi::ev::name_class::ip4_addr (const char *ip, int port, sockaddr_in *addr) MANAPICORE_NOEXCEPT { return uv_ip4_addr(ip, port, addr); } \
int manapi::ev::name_class::ip6_addr (const char *ip, int port, sockaddr_in6 *addr) MANAPICORE_NOEXCEPT {  return uv_ip6_addr(ip, port, addr); }
#define MANAPICORE_EV_CHECK(expr) { auto rhs = expr; if (rhs) { std::cout << rhs << "\n"; throw manapi::exception(manapi::ERR_INTERNAL, #expr); } }

MANAPICORE_EV_DEFAULT(async, uv_async_t)
MANAPICORE_EV_DEFAULT(timer, uv_timer_t)
MANAPICORE_EV_DEFAULT(io, uv_poll_t)
MANAPICORE_EV_DEFAULT(check, uv_check_t)
MANAPICORE_EV_DEFAULT(prepare, uv_prepare_t)
MANAPICORE_EV_DEFAULT(idle, uv_idle_t)
MANAPICORE_EV_DEFAULT(connect, uv_connect_t)
MANAPICORE_EV_DEFAULT(work, uv_work_t)
MANAPICORE_EV_DEFAULT(tcp, uv_tcp_t)

MANAPICORE_EV_DEFAULT(udp, uv_udp_t)
MANAPICORE_EV_DEFAULT(write, uv_write_t)
MANAPICORE_EV_DEFAULT(random, uv_random_t)
MANAPICORE_EV_DEFAULT(getaddrinfo, uv_getaddrinfo_t)
MANAPICORE_EV_DEFAULT(getnameinfo, uv_getnameinfo_t)
MANAPICORE_EV_DEFAULT(udp_send, uv_udp_send_t)
MANAPICORE_EV_DEFAULT(fs, uv_fs_t)

MANAPICORE_EV_STREAM(udp, uv_udp_t)
MANAPICORE_EV_STREAM(tcp, uv_tcp_t)

manapi::ev::connect::connect() : m_s() {
}


int manapi::ev::connect::cancel() MANAPICORE_NOEXCEPT {
    return uv_cancel((reinterpret_cast<uv_req_t *>(&this->m_s)));
}

int manapi::ev::connect::bind(uv_tcp_t *p, const sockaddr *addr, uv_connect_cb cb) MANAPICORE_NOEXCEPT {
    return uv_tcp_connect(&this->m_s, p, addr, cb);
}


void manapi::ev::buffer_deleter::operator()(manapi::ev::buff_t *data) {
    delete[] data;
}

void manapi::ev::chars_deleter::operator()(const char *data) {
    delete[] data;
}

manapi::ev::async::async() : m_s() {}

int manapi::ev::async::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return this->bind(loop, callback_watcher_async);
}

int manapi::ev::async::bind(loop_ref loop, uv_async_cb cb) MANAPICORE_NOEXCEPT {
    return uv_async_init(loop, &this->m_s, cb);
}

int manapi::ev::async::send() MANAPICORE_NOEXCEPT {
    return uv_async_send(&this->m_s);
}

int manapi::ev::async::set() MANAPICORE_NOEXCEPT {
    return this->set(callback_watcher_async);
}

int manapi::ev::async::set(uv_async_cb cb) MANAPICORE_NOEXCEPT {
    auto data = this->data();
    auto loop = this->loop();

    this->unbind();

    if (auto rhs = uv_async_init(loop, &this->m_s, cb)) {
        return rhs;
    }

    this->data(data);
    return 0;
}

int manapi::ev::idle::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return uv_idle_init(loop, &this->m_s);
}

int manapi::ev::idle::start() MANAPICORE_NOEXCEPT {
    return this->start(callback_watcher_idle);
}

int manapi::ev::idle::start(uv_idle_cb cb) MANAPICORE_NOEXCEPT {
    return uv_idle_start(&this->m_s, cb);
}

int manapi::ev::idle::stop() MANAPICORE_NOEXCEPT {
    return uv_idle_stop(&this->m_s);
}

manapi::ev::idle::idle() : m_s() {

}

manapi::ev::check::check() : m_s() {
}

int manapi::ev::check::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return uv_check_init(loop, &this->m_s);
}

int manapi::ev::check::start() MANAPICORE_NOEXCEPT {
    return this->start(callback_watcher_check);
}

int manapi::ev::check::start(uv_check_cb cb) MANAPICORE_NOEXCEPT {
    return uv_check_start(&this->m_s, cb);
}

int manapi::ev::check::stop() MANAPICORE_NOEXCEPT {
    return uv_check_stop(&this->m_s);
}

manapi::ev::io::io () : m_s () {
    
}

int manapi::ev::io::start(int revents, uv_poll_cb cb) MANAPICORE_NOEXCEPT {
    return uv_poll_start(&this->m_s, revents, cb);
}

int manapi::ev::io::start(int revents) MANAPICORE_NOEXCEPT {
    return this->start(revents, ev::callback_watcher_io);
}

int manapi::ev::io::start() MANAPICORE_NOEXCEPT {
    return this->start(this->events());
}

int manapi::ev::io::restart(int revents) MANAPICORE_NOEXCEPT {
    if (this->stop()) return -1;
    return this->start(revents);
}

int manapi::ev::io::stop() MANAPICORE_NOEXCEPT {
    return uv_poll_stop(&this->m_s);
}

int manapi::ev::io::events() MANAPICORE_NOEXCEPT {
#ifdef _WIN32
    return 0;
#else
    return static_cast<int>(this->custom()->io_watcher.pevents & (ev::WRITE|ev::READ));
#endif
}

manapi::ev::write::write() : m_s() {

}

int manapi::ev::write::bind(uv_stream_t *stream, const uv_buf_t *buf, uint32_t nbufs, uv_write_cb cb) MANAPICORE_NOEXCEPT {
    return uv_write(&this->m_s, stream, buf, nbufs, cb);
}

int manapi::ev::write::bind(uv_stream_t *stream, const uv_buf_t *buf, uint32_t nbufs) MANAPICORE_NOEXCEPT {
    return this->bind(stream, buf, nbufs, callback_watcher_write);
}

manapi::ev::tcp::tcp() : m_s() {

}

int manapi::ev::tcp::listen(int tcp_backlog) MANAPICORE_NOEXCEPT {
    return this->listen(tcp_backlog, reinterpret_cast<uv_connection_cb>(ev::callback_watcher_tcp_accept));
}

int manapi::ev::tcp::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return uv_tcp_init(loop, &this->m_s);
}

int manapi::ev::tcp::connect(uv_connect_t *connect, const sockaddr *addr, uv_connect_cb cb) MANAPICORE_NOEXCEPT {
    return uv_tcp_connect(connect, &this->m_s, addr, cb);
}

int manapi::ev::tcp::accept(tcp *parent) MANAPICORE_NOEXCEPT {
    return uv_accept(MANAPICORE_EV_CAST_STREAM(&parent->m_s), MANAPICORE_EV_CAST_STREAM(&this->m_s));
}

int manapi::ev::tcp::read_start() MANAPICORE_NOEXCEPT {
    return this->read_start(ev::callback_watcher_tcp_connection_alloc, ev::callback_watcher_tcp_read);
}

int manapi::ev::tcp::read_start(uv_alloc_cb alloc, uv_read_cb cb) MANAPICORE_NOEXCEPT {
    return uv_read_start(MANAPICORE_EV_CAST_STREAM(&this->m_s), alloc, cb);
}

int manapi::ev::tcp::read_stop() MANAPICORE_NOEXCEPT {
    return uv_read_stop(MANAPICORE_EV_CAST_STREAM(&this->m_s));
}

ssize_t manapi::ev::tcp::try_write(const void *buff, std::size_t len) MANAPICORE_NOEXCEPT {
    ev::buff_t buffs;
    buffs.base = (char*)buff;
    buffs.len = static_cast<decltype(buffs.len)>(len);
    return uv_try_write(MANAPICORE_EV_CAST_STREAM(&this->m_s), &buffs, 1);
}

ssize_t manapi::ev::tcp::try_write(const ev::buff_t *buff, uint32_t nbuff) MANAPICORE_NOEXCEPT {
    return uv_try_write(MANAPICORE_EV_CAST_STREAM(&this->m_s), buff, nbuff);
}

int manapi::ev::tcp::s_bind(const sockaddr *addr, uint32_t flags) MANAPICORE_NOEXCEPT {
    return uv_tcp_bind(&this->m_s, addr, flags);
}

int manapi::ev::tcp::getpeername(sockaddr *name, int *namelen) MANAPICORE_NOEXCEPT {
    return uv_tcp_getpeername(&this->m_s, name, namelen);
}

int manapi::ev::tcp::getsockname(sockaddr *name, int *namelen) MANAPICORE_NOEXCEPT {
    return uv_tcp_getsockname(&this->m_s, name, namelen);
}

int manapi::ev::tcp::close_reset(uv_close_cb close_cb) MANAPICORE_NOEXCEPT {
    return uv_tcp_close_reset(&this->m_s, close_cb);
}

int manapi::ev::tcp::close_reset() MANAPICORE_NOEXCEPT {
    return this->close_reset(callback_close_cb);
}

int manapi::ev::tcp::keepalive(int enable, unsigned int delay) MANAPICORE_NOEXCEPT {
    return uv_tcp_keepalive(&this->m_s, enable, delay);
}

int manapi::ev::tcp::nodelay(int enable) MANAPICORE_NOEXCEPT {
    return uv_tcp_nodelay(&this->m_s, enable);
}

int manapi::ev::tcp::simultaneous_accepts(int enable) MANAPICORE_NOEXCEPT {
    return uv_tcp_simultaneous_accepts(&this->m_s, enable);
}

manapi::ev::udp::udp() : m_s() {

}

int manapi::ev::udp::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return uv_udp_init(loop, &this->m_s);
}

int manapi::ev::udp::s_bind(const sockaddr *addr, uint32_t flags) MANAPICORE_NOEXCEPT{
    return uv_udp_bind(&this->m_s, addr, flags);
}

int manapi::ev::udp::recv_start() MANAPICORE_NOEXCEPT {
    return this->recv_start(ev::callback_watcher_udp_alloc, ev::callback_watcher_udp_recv);
}

int manapi::ev::udp::recv_start(uv_alloc_cb alloc, uv_udp_recv_cb cb) MANAPICORE_NOEXCEPT {
    return uv_udp_recv_start(&this->m_s, alloc, cb);
}

int manapi::ev::udp::recv_stop() MANAPICORE_NOEXCEPT{
    return uv_udp_recv_stop(&this->m_s);
}

int manapi::ev::udp::connect(const sockaddr *addr) MANAPICORE_NOEXCEPT {
    return uv_udp_connect(&this->m_s, addr);
}

int manapi::ev::udp::try_send(const uv_buf_t *buf, uint32_t nbuf, sockaddr *addr) MANAPICORE_NOEXCEPT {
    return uv_udp_try_send(&this->m_s, buf, nbuf, addr);
}

manapi::ev::udp_send::udp_send() : m_s() {

}

int manapi::ev::udp_send::bind(uv_udp_t *stream, const uv_buf_t *buf, uint32_t nbufs, uv_udp_send_cb cb,
    const sockaddr *addr) MANAPICORE_NOEXCEPT {
    return uv_udp_send(&this->m_s, stream, buf, nbufs, addr, cb);
}

int manapi::ev::udp_send::bind(uv_udp_t *stream, const uv_buf_t *buf, uint32_t nbufs, const sockaddr *addr) MANAPICORE_NOEXCEPT {
    return this->bind(stream, buf, nbufs, callback_watcher_udp_send, addr);
}

manapi::ev::prepare::prepare() : m_s() {

}

int manapi::ev::prepare::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return uv_prepare_init(loop, &this->m_s);
}

int manapi::ev::prepare::start() MANAPICORE_NOEXCEPT {
    return this->start(ev::callback_watcher_prepare);
}

int manapi::ev::prepare::start(uv_prepare_cb cb) MANAPICORE_NOEXCEPT {
    return uv_prepare_start(&this->m_s, cb);
}

int manapi::ev::prepare::stop() MANAPICORE_NOEXCEPT {
    return uv_prepare_stop(&this->m_s);
}

manapi::ev::timer::timer() : m_s() {

}

int manapi::ev::timer::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return uv_timer_init(loop, &this->m_s);
}

int manapi::ev::timer::start(uint64_t timeout, uint64_t repeat) MANAPICORE_NOEXCEPT {
    return this->start(timeout, repeat, ev::callback_watcher_timer);
}

int manapi::ev::timer::start(uint64_t timeout, uint64_t repeat, uv_timer_cb cb) MANAPICORE_NOEXCEPT {
    return uv_timer_start(&this->m_s, cb, timeout, repeat);
}

int manapi::ev::timer::stop() MANAPICORE_NOEXCEPT {
    return uv_timer_stop(&this->m_s);
}

int manapi::ev::timer::again() MANAPICORE_NOEXCEPT {
    return uv_timer_again(&this->m_s);
}

void manapi::ev::timer::repeat(uint64_t repeat) MANAPICORE_NOEXCEPT {
    uv_timer_set_repeat(&this->m_s, repeat);
}

uint64_t manapi::ev::timer::repeat() const MANAPICORE_NOEXCEPT {
    return uv_timer_get_repeat(&this->m_s);
}

uint64_t manapi::ev::timer::due_in() const MANAPICORE_NOEXCEPT {
    return uv_timer_get_due_in(&this->m_s);
}

manapi::ev::fs::fs(loop_ref loop) : m_s(), m_loop(loop) {}

int manapi::ev::fs::cancel() MANAPICORE_NOEXCEPT {
    return uv_cancel(reinterpret_cast<uv_req_t *> (&this->m_s));
}

int manapi::ev::fs::open(const char *path, int flags, int mode, uv_fs_cb open_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_open(this->m_loop, &this->m_s, path, flags, mode, open_cb);
}

int manapi::ev::fs::open(const char *path, int flags, int mode) MANAPICORE_NOEXCEPT {
    return this->open(path, flags, mode, callback_watcher_fs);
}

int manapi::ev::fs::read(ev::file fileno, const uv_buf_t *buff, uint32_t nbuff, int64_t offset, uv_fs_cb read_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_read(this->m_loop, &this->m_s, fileno, buff, nbuff, offset, read_cb);
}

int manapi::ev::fs::read(ev::file fileno, const uv_buf_t *buff, uint32_t nbuff, int64_t offset) MANAPICORE_NOEXCEPT {
    return this->read(fileno, buff, nbuff, offset, callback_watcher_fs);
}

int manapi::ev::fs::write(ev::file fileno, const uv_buf_t *buff, uint32_t nbuff, int64_t offset, uv_fs_cb write_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_write(this->m_loop, &this->m_s, fileno, buff, nbuff, offset, write_cb);
}

int manapi::ev::fs::write(ev::file fileno, const uv_buf_t *buff, uint32_t nbuff, int64_t offset) MANAPICORE_NOEXCEPT {
    return this->write(fileno, buff, nbuff, offset, callback_watcher_fs);
}

ssize_t manapi::ev::fs::try_write(ev::file fileno, const void *buff, std::size_t nbuff, int64_t offset) MANAPICORE_NOEXCEPT {
    ssize_t r;

#if defined(_WIN32)
    HANDLE handle;
    OVERLAPPED overlapped, *overlapped_ptr;
    LARGE_INTEGER offset_;
    DWORD bytes;
    DWORD error;
    int result;
    unsigned int index;
    LARGE_INTEGER original_position;
    LARGE_INTEGER zero_offset;
    int restore_position;

    zero_offset.QuadPart = 0;
    restore_position = 0;
    handle = (HANDLE) _get_osfhandle(fileno);
    if (handle == INVALID_HANDLE_VALUE) {
        return ERR_BADF;
    }

    if (offset != -1) {
        memset(&overlapped, 0, sizeof overlapped);
        overlapped_ptr = &overlapped;
        if (SetFilePointerEx(handle, zero_offset, &original_position, FILE_CURRENT)) {
            restore_position = 1;
        }
    }
    else {
        overlapped_ptr = NULL;
    }

    index = 0;
    bytes = 0;

    do {
        DWORD incremental_bytes;

        if (offset != -1) {
            offset_.QuadPart = offset + bytes;
            overlapped.Offset = offset_.LowPart;
            overlapped.OffsetHigh = offset_.HighPart;
        }

        result = WriteFile(handle,
                           buff,
                           static_cast<DWORD>(nbuff),
                           &incremental_bytes,
                           overlapped_ptr);
        bytes += incremental_bytes;
        ++index;
    }
    while (0);

    if (restore_position) {
        SetFilePointerEx(handle, original_position, NULL, FILE_BEGIN);
    }

    if (result || bytes > 0) {
        r = bytes;
    }
    else {
        error = GetLastError();
        /* error */
        if (error == ERROR_ACCESS_DENIED) {
            error = ERROR_INVALID_FLAGS;
        }
        r = uv_translate_sys_error(error);
    }
#else
    if (offset < 0) {
        r = ::write(fileno, buff, nbuff);
    }
    else {
        r = ::pwrite(fileno, buff, nbuff, offset);
    }
#endif
    return r;
}

ssize_t manapi::ev::fs::try_read(ev::file fileno, void *buff, std::size_t nbuff, int64_t offset) MANAPICORE_NOEXCEPT {
    ssize_t r;
#if defined(_WIN32)
    HANDLE handle;
    OVERLAPPED overlapped, *overlapped_ptr;
    LARGE_INTEGER offset_;
    DWORD bytes;
    DWORD error;
    int result;
    unsigned int index;
    LARGE_INTEGER original_position;
    LARGE_INTEGER zero_offset;
    int restore_position;

    zero_offset.QuadPart = 0;
    restore_position = 0;
    handle = (HANDLE) _get_osfhandle(fileno);

    if (handle == INVALID_HANDLE_VALUE) {
        return ERR_BADF;
    }

    if (offset != -1) {
        memset(&overlapped, 0, sizeof overlapped);
        overlapped_ptr = &overlapped;
        if (SetFilePointerEx(handle, zero_offset, &original_position, FILE_CURRENT)) {
            restore_position = 1;
        }
    }
    else {
        overlapped_ptr = NULL;
    }

    index = 0;
    bytes = 0;
    do {
        DWORD incremental_bytes;

        if (offset != -1) {
            offset_.QuadPart = offset + bytes;
            overlapped.Offset = offset_.LowPart;
            overlapped.OffsetHigh = offset_.HighPart;
        }

        result = ReadFile(handle,
                          buff,
                          static_cast<DWORD>(nbuff),
                          &incremental_bytes,
                          overlapped_ptr);
        bytes += incremental_bytes;
        ++index;
    }
    while (0);

    if (restore_position) {
        SetFilePointerEx(handle, original_position, NULL, FILE_BEGIN);
    }

    if (result || bytes > 0) {
        r = bytes;
    }
    else {
        error = GetLastError();
        if (error == ERROR_ACCESS_DENIED) {
            error = ERROR_INVALID_FLAGS;
        }

        if (error == ERROR_HANDLE_EOF || error == ERROR_BROKEN_PIPE) {
            r = bytes;
        }
        else {
            r = uv_translate_sys_error(error);
        }
    }
#else
    if (offset < 0) {
        r = ::read(fileno, buff, nbuff);
    }
    else {
        r = ::pread(fileno, buff, nbuff, offset);
    }
#endif
    return r;
}

int manapi::ev::fs::close(ev::file fileno, uv_fs_cb close_cb) MANAPICORE_NOEXCEPT {
    manapi_log_trace2("manapicore::fs", manapi::debug::LOG_TRACE_LOW, "fs:fd %d close", fileno);
    return uv_fs_close(this->m_loop, &this->m_s, fileno, close_cb);
}

int manapi::ev::fs::close(ev::file fileno) MANAPICORE_NOEXCEPT {
    return this->close(fileno, callback_watcher_fs);
}

int manapi::ev::fs::unlink(const char *path, uv_fs_cb unlink_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_unlink(this->m_loop, &this->m_s, path, unlink_cb);
}

int manapi::ev::fs::unlink(const char *path) MANAPICORE_NOEXCEPT {
    return this->unlink(path, callback_watcher_fs);
}

int manapi::ev::fs::mkdir(const char *path, int mode, uv_fs_cb mkdir_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_mkdir(this->m_loop, &this->m_s, path, mode, mkdir_cb);
}

int manapi::ev::fs::mkdir(const char *path, int mode) MANAPICORE_NOEXCEPT {
    return this->mkdir(path, mode, callback_watcher_fs);
}

int manapi::ev::fs::mkdtemp(const char *path, uv_fs_cb mkdtemp_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_mkdtemp(this->m_loop, &this->m_s, path, mkdtemp_cb);
}

int manapi::ev::fs::mkdtemp(const char *path) MANAPICORE_NOEXCEPT {
    return this->mkdtemp(path, callback_watcher_fs);
}

int manapi::ev::fs::mkstemp(const char *path, uv_fs_cb mkstemp_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_mkstemp(this->m_loop, &this->m_s, path, mkstemp_cb);
}

int manapi::ev::fs::mkstemp(const char *path) MANAPICORE_NOEXCEPT {
    return this->mkstemp(path, callback_watcher_fs);
}

int manapi::ev::fs::rmdir(const char *path, uv_fs_cb rmdir_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_rmdir(this->m_loop, &this->m_s, path, rmdir_cb);
}

int manapi::ev::fs::rmdir(const char *path) MANAPICORE_NOEXCEPT {
    return this->rmdir(path, callback_watcher_fs);
}

int manapi::ev::fs::opendir(const char *path, uv_fs_cb opendir) MANAPICORE_NOEXCEPT {
    return uv_fs_opendir(this->m_loop, &this->m_s, path, opendir);
}

int manapi::ev::fs::opendir(const char *path) MANAPICORE_NOEXCEPT {
    return this->opendir(path, callback_watcher_fs);
}

int manapi::ev::fs::closedir(ev::dir_t * dir, uv_fs_cb closedir_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_closedir(this->m_loop, &this->m_s, dir, closedir_cb);
}

int manapi::ev::fs::closedir(ev::dir_t * dir) MANAPICORE_NOEXCEPT {
    return this->closedir(dir, callback_watcher_fs);
}

int manapi::ev::fs::readdir(ev::dir_t * dir, uv_fs_cb readdir_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_readdir(this->m_loop, &this->m_s, dir, readdir_cb);
}

int manapi::ev::fs::readdir(ev::dir_t * dir) MANAPICORE_NOEXCEPT {
    return this->readdir(dir, callback_watcher_fs);
}

int manapi::ev::fs::scandir(const char *path, int flags, uv_fs_cb scandir_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_scandir(this->m_loop, &this->m_s, path, flags, scandir_cb);
}

int manapi::ev::fs::scandir(const char *path, int flags) MANAPICORE_NOEXCEPT {
    return this->scandir(path, flags, callback_watcher_fs);
}

int manapi::ev::fs::scandir_next(ev::dirent_t *dir) MANAPICORE_NOEXCEPT {
    return uv_fs_scandir_next(&this->m_s, dir);
}

int manapi::ev::fs::stat(const char *path, uv_fs_cb stat_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_stat(this->m_loop, &this->m_s, path, stat_cb);
}

int manapi::ev::fs::stat(const char *path) MANAPICORE_NOEXCEPT {
    return this->stat(path, callback_watcher_fs);
}

int manapi::ev::fs::fstat(ev::file file, uv_fs_cb fstat_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_fstat(this->m_loop, &this->m_s, file, fstat_cb);
}

int manapi::ev::fs::fstat(ev::file file) MANAPICORE_NOEXCEPT {
    return this->fstat(file, callback_watcher_fs);
}

int manapi::ev::fs::lstat(const char *path, uv_fs_cb lstat_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_lstat(this->m_loop, &this->m_s, path, lstat_cb);
}

int manapi::ev::fs::lstat(const char *path) MANAPICORE_NOEXCEPT {
    return this->lstat(path, callback_watcher_fs);
}

int manapi::ev::fs::statfs(const char *path, uv_fs_cb statfs_cb) MANAPICORE_NOEXCEPT {
    return uv_fs_statfs(this->m_loop, &this->m_s, path, statfs_cb);
}

int manapi::ev::fs::statfs(const char *path) MANAPICORE_NOEXCEPT {
    return this->statfs(path, callback_watcher_fs);
}

int manapi::ev::fs::rename(const char *path, const char *new_path, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_rename(this->m_loop, &this->m_s, path, new_path, cb);
}

int manapi::ev::fs::rename(const char *path, const char *new_path) MANAPICORE_NOEXCEPT {
    return this->rename(path, new_path, callback_watcher_fs);
}

int manapi::ev::fs::fsync(ev::file file, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_fsync(this->m_loop, &this->m_s, file, cb);
}

int manapi::ev::fs::fsync(ev::file file) MANAPICORE_NOEXCEPT {
    return this->fsync(file, callback_watcher_fs);
}

int manapi::ev::fs::fdatasync(ev::file file, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_fdatasync(this->m_loop, &this->m_s, file, cb);
}

int manapi::ev::fs::fdatasync(ev::file file) MANAPICORE_NOEXCEPT {
    return this->fdatasync(file, callback_watcher_fs);
}

int manapi::ev::fs::ftruncate(ev::file file, int64_t off, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_ftruncate(this->m_loop, &this->m_s, file, off, cb);
}

int manapi::ev::fs::ftruncate(ev::file file, int64_t off) MANAPICORE_NOEXCEPT {
    return this->ftruncate(file, off, callback_watcher_fs);
}

int manapi::ev::fs::copyfile(const char *path1, const char *path2, int flags, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_copyfile(this->m_loop, &this->m_s, path1, path2, flags, cb);
}

int manapi::ev::fs::copyfile(const char *path1, const char *path2, int flags) MANAPICORE_NOEXCEPT {
    return this->copyfile(path1, path2, flags, callback_watcher_fs);
}

int manapi::ev::fs::sendfile(ev::file outfd, ev::file infd, int64_t off, size_t length, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_sendfile(this->m_loop, &this->m_s, outfd, infd, off, length, cb);
}

int manapi::ev::fs::sendfile(ev::file outfd, ev::file infd, int64_t off, size_t length) MANAPICORE_NOEXCEPT {
    return this->sendfile(outfd, infd, off, length, callback_watcher_fs);
}

int manapi::ev::fs::access(const char *path, int mode, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_access(this->m_loop, &this->m_s, path, mode, cb);
}

int manapi::ev::fs::access(const char *path, int mode) MANAPICORE_NOEXCEPT {
    return this->access(path, mode, callback_watcher_fs);
}

int manapi::ev::fs::chmod(const char *path, int mode, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_chmod(this->m_loop, &this->m_s, path, mode, cb);
}

int manapi::ev::fs::chmod(const char *path, int mode) MANAPICORE_NOEXCEPT {
    return this->chmod(path, mode, callback_watcher_fs);
}

int manapi::ev::fs::fchmod(ev::file file, int mode, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_fchmod(this->m_loop, &this->m_s, file, mode, cb);
}

int manapi::ev::fs::fchmod(ev::file file, int mode) MANAPICORE_NOEXCEPT {
    return this->fchmod(file, mode, callback_watcher_fs);
}

int manapi::ev::fs::utime(const char *path, double atime, double mtime, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_utime(this->m_loop, &this->m_s, path, atime, mtime, cb);
}

int manapi::ev::fs::utime(const char *path, double atime, double mtime) MANAPICORE_NOEXCEPT {
    return this->utime(path, atime, mtime, callback_watcher_fs);
}

int manapi::ev::fs::futime(ev::file file, double atime, double mtime, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_futime(this->m_loop, &this->m_s, file, atime, mtime, cb);
}

int manapi::ev::fs::futime(ev::file file, double atime, double mtime) MANAPICORE_NOEXCEPT {
    return this->futime(file, atime, mtime, callback_watcher_fs);
}

int manapi::ev::fs::lutime(const char *path, double atime, double mtime, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_lutime(this->m_loop, &this->m_s, path, atime, mtime, cb);
}

int manapi::ev::fs::lutime(const char *path, double atime, double mtime) MANAPICORE_NOEXCEPT {
    return this->lutime(path, atime, mtime, callback_watcher_fs);
}

int manapi::ev::fs::link(const char *path, const char *new_path, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_link(this->m_loop, &this->m_s, path, new_path, cb);
}

int manapi::ev::fs::link(const char *path, const char *new_path) MANAPICORE_NOEXCEPT {
    return this->link(path, new_path, callback_watcher_fs);
}

int manapi::ev::fs::symlink(const char *path, const char *new_path, int flags, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_symlink(this->m_loop, &this->m_s, path, new_path, flags, cb);
}

int manapi::ev::fs::symlink(const char *path, const char *new_path, int flags) MANAPICORE_NOEXCEPT {
    return this->symlink(path, new_path, flags, callback_watcher_fs);
}

int manapi::ev::fs::readlink(const char *path, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_readlink(this->m_loop, &this->m_s, path, cb);
}

int manapi::ev::fs::readlink(const char *path) MANAPICORE_NOEXCEPT {
    return this->readlink(path, callback_watcher_fs);
}

int manapi::ev::fs::realpath(const char *path, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_realpath(this->m_loop, &this->m_s, path, cb);
}

int manapi::ev::fs::realpath(const char *path) MANAPICORE_NOEXCEPT {
    return this->realpath(path, callback_watcher_fs);
}

int manapi::ev::fs::chown(const char *path, uid_t uid, gid_t gid, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_chown(this->m_loop, &this->m_s, path, uid, gid, cb);
}

int manapi::ev::fs::chown(const char *path, uid_t uid, gid_t gid) MANAPICORE_NOEXCEPT {
    return this->chown(path, uid, gid, callback_watcher_fs);
}

int manapi::ev::fs::fchown(ev::file file, uid_t uid, gid_t gid, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_fchown(this->m_loop, &this->m_s, file, uid, gid, cb);
}

int manapi::ev::fs::fchown(ev::file file, uid_t uid, gid_t gid) MANAPICORE_NOEXCEPT {
    return this->fchown(file, uid, gid, callback_watcher_fs);
}

int manapi::ev::fs::lchown(const char *path, uid_t uid, gid_t gid, uv_fs_cb cb) MANAPICORE_NOEXCEPT {
    return uv_fs_lchown(this->m_loop, &this->m_s, path, uid, gid, cb);
}

int manapi::ev::fs::lchown(const char *path, uid_t uid, gid_t gid) MANAPICORE_NOEXCEPT {
    return this->lchown(path, uid, gid, callback_watcher_fs);
}

ssize_t manapi::ev::fs::result() const MANAPICORE_NOEXCEPT {
    return this->m_s.result;
}

void manapi::ev::fs::cleanup() MANAPICORE_NOEXCEPT {
    uv_fs_req_cleanup (&this->m_s);
}

manapi::ev::random::random() : m_s() {}

int manapi::ev::random::cancel() MANAPICORE_NOEXCEPT {
    return uv_cancel(reinterpret_cast<uv_req_t *> (&this->m_s));
}

int manapi::ev::random::bind(loop_ref loop, char *buff, std::size_t size, uv_random_cb cb) MANAPICORE_NOEXCEPT {
    return uv_random(loop, &this->m_s, buff, size, /* flags */ 0, cb);
}

int manapi::ev::random::bind (loop_ref loop, char *buff, std::size_t size) MANAPICORE_NOEXCEPT {
    return this->bind(loop, buff, size, callback_watcher_random);
}

manapi::ev::getaddrinfo::getaddrinfo() : m_s() {

}

int manapi::ev::getaddrinfo::cancel() MANAPICORE_NOEXCEPT {
    return uv_cancel(reinterpret_cast<uv_req_t *> (&this->m_s));
}

int manapi::ev::getaddrinfo::bind(loop_ref loop, const char *node, const char *service, const addrinfo *hints,
    uv_getaddrinfo_cb getaddrinfo_cb) MANAPICORE_NOEXCEPT {
        return uv_getaddrinfo(loop, &this->m_s, getaddrinfo_cb, node, service, hints);
}

int manapi::ev::getaddrinfo::bind(loop_ref loop, const char *node, const char *service,
    const addrinfo *hints) MANAPICORE_NOEXCEPT {
        return this->bind(loop, node, service, hints, callback_watcher_getaddrinfo);
}

void manapi::ev::getaddrinfo::free(addrinfo *n) MANAPICORE_NOEXCEPT {
    uv_freeaddrinfo(n);
}

manapi::ev::getnameinfo::getnameinfo() : m_s() {
}

int manapi::ev::getnameinfo::cancel() MANAPICORE_NOEXCEPT {
    return uv_cancel(reinterpret_cast<uv_req_t *> (&this->m_s));
}

int manapi::ev::getnameinfo::bind(loop_ref loop, const sockaddr *addr, int flags, uv_getnameinfo_cb cb) MANAPICORE_NOEXCEPT {
    return uv_getnameinfo(loop, &this->m_s, cb, addr, flags);
}

int manapi::ev::getnameinfo::bind(loop_ref loop, const sockaddr *addr, int flags) MANAPICORE_NOEXCEPT {
    return this->bind(loop, addr, flags, callback_watcher_getnameinfo);
}

manapi::ev::work::work() : m_s() {}

int manapi::ev::work::cancel() MANAPICORE_NOEXCEPT {
    return uv_cancel(reinterpret_cast<uv_req_t *>(&this->m_s));
}

int manapi::ev::work::bind(loop_ref loop, uv_work_cb cb, uv_after_work_cb after_cb) MANAPICORE_NOEXCEPT {
    return uv_queue_work(loop, &this->m_s, cb, after_cb);
}

int manapi::ev::work::bind(loop_ref loop) MANAPICORE_NOEXCEPT {
    return this->bind(loop, callback_watcher_work, callback_watcher_after_work);
}

const char * manapi::ev::strerror(int errnum) MANAPICORE_NOEXCEPT {
    return uv_strerror(errnum);
}

const char * manapi::ev::namerror(int errnum) MANAPICORE_NOEXCEPT {
    return uv_err_name(errnum);
}

std::size_t manapi::ev::hrtime() MANAPICORE_NOEXCEPT {
    return ::uv_hrtime();
}

manapi::ev::status::status() {
    this->m_syserr = 0;
}

manapi::ev::status::~status() = default;

manapi::ev::status::status(manapi::err_num code, std::string_view msg, int syserr) : manapi::status(code, msg) {
    this->m_syserr = syserr;
}

manapi::ev::status::status(manapi::err_num code, std::string msg, int syserr) : manapi::status(code, std::move(msg)) {
    this->m_syserr = syserr;
}

manapi::ev::status::status(manapi::err_num code, const char *msg, int syserr) : manapi::status(code, msg) {
    this->m_syserr = syserr;
}

manapi::ev::status::status(status &&n) MANAPICORE_NOEXCEPT {
    this->m_syserr = std::exchange(n.m_syserr, 0);
    manapi::status::operator=(std::forward<decltype(n)>(n));
}

manapi::ev::status & manapi::ev::status::operator=(status &&n) MANAPICORE_NOEXCEPT {
    this->m_syserr = std::exchange(n.m_syserr, 0);
    manapi::status::operator=(std::forward<decltype(n)>(n));
    return *this;
}

manapi::ev::status::status(manapi::status &&n) MANAPICORE_NOEXCEPT {
    this->m_syserr = 0;
    manapi::status::operator=(std::forward<decltype(n)>(n));
}

manapi::ev::status & manapi::ev::status::operator=(manapi::status &&n) MANAPICORE_NOEXCEPT {
    this->m_syserr = 0;
    manapi::status::operator=(std::forward<decltype(n)>(n));
    return *this;
}

manapi::ev::status::status(const status &n) : manapi::status(n) {
    this->m_syserr = n.m_syserr;
}

manapi::ev::status & manapi::ev::status::operator=(const status &n) {
    this->m_syserr = n.m_syserr;
    manapi::status::operator=(std::forward<decltype(n)>(n));
    return *this;
}

std::string manapi::ev::status::fullmsg() const {
    return std::format("{} syserr={} sysname={} sysmsg={}", manapi::status::fullmsg(), this->m_syserr, this->sysname(), this->sysmsg());
}

int manapi::ev::status::syserr() const {
    return this->m_syserr;
}

std::string_view manapi::ev::status::sysname() const {
    if (!this->m_syserr) return "OK";
    return ev::namerror(this->m_syserr);
}

std::string_view manapi::ev::status::sysmsg() const {
    if (!this->m_syserr) return "OK";
    return ev::strerror(this->m_syserr);
}

manapi::ev::unique_file::unique_file() {
}

manapi::ev::unique_file::unique_file(ev::file fd) : m_fd(fd) {

}

manapi::ev::unique_file::~unique_file() {
    this->reset();
}

manapi::ev::unique_file::unique_file(unique_file &&n) MANAPICORE_NOEXCEPT {
    this->m_fd = n.m_fd;
    n.m_fd.reset();
}

manapi::ev::unique_file & manapi::ev::unique_file::operator=(unique_file &&n) MANAPICORE_NOEXCEPT {
    if (this != &n) {
        this->reset();
        this->m_fd = n.m_fd;
        n.m_fd.reset();
    }
    return *this;
}

manapi::status_or<manapi::ev::file> manapi::ev::unique_file::release() MANAPICORE_NOEXCEPT {
    if (this->m_fd.has_value()) {
        auto val = this->m_fd.value();
        this->m_fd.reset();
        return manapi::ev::file( val );
    }
    return manapi::status_not_found("unique_file:empty");
}

manapi::ev::file manapi::ev::unique_file::get() const {
    if (this->m_fd.has_value())
        return this->m_fd.value();
    throw manapi::exception (manapi::ERR_NOT_FOUND, "unique_file:empty");
}
//
// manapi::ev::unique_file::operator bool() const MANAPICORE_NOEXCEPT {
//     return this->m_fd.has_value();
// }

void manapi::ev::unique_file::reset() MANAPICORE_NOEXCEPT {
    if (this->m_fd.has_value()) {
        auto fd = this->release().unwrap();
        ::uv_fs_t req;
        manapi_log_trace2("manapicore::fs", manapi::debug::LOG_TRACE_LOW, "fs:fd %d close", fd);
        if (auto rhs = ::uv_fs_close(manapi::async::eventloop()->loop(), &req, fd, nullptr))
            manapi_log_error("%s failed due to %s", "uv_fs_close", ev::strerror(rhs));
        ::uv_fs_req_cleanup(&req);
    }
}

void manapi::ev::unique_file::reset(ev::file fd) MANAPICORE_NOEXCEPT {
    this->reset();
    this->m_fd = fd;
}

bool manapi::ev::unique_file::has_value() const MANAPICORE_NOEXCEPT {
    return this->m_fd.has_value();
}

void manapi::ev::dir_deleter_t::operator()(ev::dir_t *ptr) MANAPICORE_NOEXCEPT {
    ::uv_fs_t req;
    if (auto rhs = ::uv_fs_closedir(manapi::async::eventloop()->loop(), &req, ptr, nullptr))
        manapi_log_error("%s failed due to %s", "uv_fs_closedir", ev::strerror(rhs));
    ::uv_fs_req_cleanup(&req);
}

void manapi::ev::dirent_deleter_t::operator()(manapi::ev::dirent_t *ptr) MANAPICORE_NOEXCEPT {
    delete[] ptr;
}