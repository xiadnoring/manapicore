//#include <memory.h>
//#include <fcntl.h>
//#include <memory>
//
//#include "std/ManapiPromise.hpp"
//#include "worker/ManapiBaseWorker.hpp"
//#include "worker/ManapiBaseUtils.hpp"
//#include "../include/ManapiUtils.hpp"
//
//manapi::net::worker::connection::connection(void *ptr, void (*deleter)(connection *data)): refcnt(0), ptr (ptr), wrk(), version(), deleter(deleter), ipdata(), cancellation() {}
//
//manapi::net::worker::connection::~connection() {
//    if (this->deleter) this->deleter(this);
//}
//
//manapi::net::worker::base::base() = default;
//
//manapi::net::worker::base::~base() = default;
//
//ssize_t manapi::net::worker::base::sync_write_ex(const shared_conn &conn, const void *buff, std::size_t size, bool finish, std::size_t maxsize) MANAPICORE_NOEXCEPT {
//    ev::buff_t buffs;
//    buffs.base = (char*)(buff);
//    buffs.len = static_cast<decltype(buffs.len)>(size);
//    return this->sync_write_ex(conn, &buffs, 1, finish, maxsize);
//}
//
//ssize_t manapi::net::worker::base::sync_write(const shared_conn &conn, slice_view buffs, bool finish) MANAPICORE_NOEXCEPT {
//    uint32_t const count = std::min<uint32_t>(buffs.slices_size(), 128);
//#ifdef _MSC_VER
//    ev::buff_t *slices = static_cast<ev::buff_t*>(alloca(sizeof (ev::buff_t) * count));
//#else
//    ev::buff_t slices[count];
//#endif
//    buffs.slices_buffs(slices, count);
//    return this->sync_write(conn, slices, count, finish && buffs.slices_size() == count);
//}
//
//ssize_t manapi::net::worker::base::sync_write(const shared_conn &conn, const void *buff, std::size_t size, bool finish) MANAPICORE_NOEXCEPT {
//    ev::buff_t buffs;
//    buffs.base = (char*)(buff);
//    buffs.len = static_cast<decltype(buffs.len)>(size);
//    return this->sync_write(conn, &buffs, 1, finish);
//}
//
//manapi::future<ssize_t> manapi::net::worker::base::write(const shared_conn &conn, const void *buff, std::size_t size, bool finish) {
//    ev::buff_t d;
//    d.base = (char*)buff;
//    d.len = static_cast<decltype(d.len)>(size);
//    co_return co_await this->write(conn, &d, 1, finish);
//}
//
//static manapi::future<ssize_t> write_internal (manapi::net::worker::base *w, ssize_t rhs, const manapi::net::worker::shared_conn &conn, manapi::slice_view buffs, bool finish) {
//    using namespace manapi;
//    using namespace manapi::net;
//    using namespace manapi::net::worker;
//
//    typedef manapi::async::promise_sync<ssize_t> promise;
//
//    if (rhs) co_return rhs;
//
//    w->waiting(conn, true);
//
//    struct write_data_t {
//        worker::base *w;
//        const shared_conn *conn;
//        slice_view *buffs;
//        bool finish;
//        worker_watcher_cb prev_cb;
//        int prev_flags;
//        promise::resolve_t resolve;
//    } write_data{};
//
//    write_data.w = w;
//    write_data.conn = &conn;
//    write_data.buffs = &buffs;
//    write_data.finish = finish;
//
//    try {
//        rhs = co_await promise ([&write_data]
//            (promise::resolve_t resolve, promise::reject_t reject)
//            -> void {
//            write_data.resolve=std::move(resolve);
//            write_data.prev_cb = write_data.w->event_on(*write_data.conn, [&write_data]
//                (const shared_conn &conn, int flags, const char *buffer, ssize_t nsize, ibuffpool_t *p) -> void {
//                    try {
//                        assert(!buffer && !nsize);
//
//                        if (flags & ev::DISCONNECT) {
//                            write_data.resolve(-1);
//                            goto finish;
//                        }
//
//                        if (flags & ev::WRITE) {
//                            auto const rhs = write_data.w->sync_write(conn, *write_data.buffs, write_data.finish);
//                            if (rhs) {
//                                write_data.resolve(rhs);
//                                goto finish;
//                            }
//                            return;
//                        }
//                    }
//                    catch (...) {
//                        //reject(std::current_exception());
//                        write_data.resolve(-1);
//                        goto finish;
//                    }
//
//                    return;
//                    finish: write_data.w->event_flags(conn, 0);
//            });
//            write_data.prev_flags = write_data.w->event_flags(*write_data.conn, ev::WRITE);
//        });
//        w->waiting(conn, false);
//
//        w->event_on(conn, std::move(write_data.prev_cb));
//        w->event_flags(conn, write_data.prev_flags);
//    }
//    catch (std::exception const &e) {
//        manapi_log_trace("%s failed due to %s", "fetch:write", e.what());
//        w->waiting(conn, false);
//        rhs = -1;
//    }
//
//    co_return rhs;
//}
//
//manapi::future<ssize_t> manapi::net::worker::base::write(const shared_conn &conn, ev::buff_t *buff, uint32_t nbuff, bool finish) {
//    using promise = manapi::async::promise_sync<ssize_t>;
//    assert(nbuff > 0);
//    auto rhs = this->sync_write(conn, buff, nbuff, finish);
//
//    if (rhs) co_return rhs;
//
//    struct write_data_t {
//        worker::base *w;
//        const shared_conn *conn;
//        manapi::ev::buff_t *buff;
//        uint32_t nbuff;
//        bool finish;
//        promise::resolve_t resolve;
//    } write_data{};
//
//    write_data.w = this;
//    write_data.conn = &conn;
//    write_data.buff = buff;
//    write_data.nbuff = nbuff;
//    write_data.finish = finish;
//
//    try {
//        rhs = co_await promise ([&write_data] (promise::resolve_t resolve, promise::reject_t reject)
//            -> void {
//            write_data.resolve = std::move(resolve);
//            write_data.w->event_on(*write_data.conn, [&write_data]
//                (const shared_conn &conn, int flags, manapi::slice_complex *sc) -> void {
//
//                    assert(sc->base()->empty());
//
//                    if (flags & ev::DISCONNECT) {
//                        write_data.resolve(-1);
//                        write_data.w->event_unsub(conn);
//                    } else if (flags & ev::WRITE) {
//                        auto const rhs = write_data.w->sync_write(conn, write_data.buff, write_data.nbuff, write_data.finish);
//                        if (rhs) {
//                            write_data.resolve(rhs);
//                            write_data.w->event_unsub(conn);
//                        }
//                    }
//
//            }, ev::WRITE);
//
//            write_data.w->waiting( *write_data.conn, true );
//        });
//
//        this->waiting(conn, false);
//    }
//    catch (std::exception const &e) {
//        manapi_log_trace("%s failed due to %s", "fetch:write", e.what());
//        rhs = -1;
//    }
//
//    co_return rhs;
//}
//
//manapi::future<ssize_t> manapi::net::worker::base::write(const shared_conn &conn, manapi::slice_view buffs, bool finish) {
//    using promise = manapi::async::promise_sync<ssize_t>;
//
//    auto rhs = this->sync_write(conn, buffs, finish);
//
//    return write_internal(this, rhs, conn, buffs, finish);
//}
//
//manapi::future<ssize_t> manapi::net::worker::base::fwrite(const shared_conn &conn,const void *buff, std::size_t size, bool finish) {
//    std::size_t total = 0;
//    ssize_t rhs = 0;
//    while (total < size) {
//        rhs = co_await this->write(conn, static_cast<const char *>(buff) + total, size - total, finish);
//        if (rhs <= 0)
//            co_return rhs;
//
//        total += static_cast<std::size_t> (rhs);
//    }
//    co_return static_cast<ssize_t>(total);
//}
//
//manapi::future<ssize_t> manapi::net::worker::base::fwrite(const shared_conn &conn, manapi::slice_view slice, bool finish) {
//    size_t total = 0;
//    ssize_t rhs = 0;
//    auto const size = slice.size();
//
//    while (total < size) {
//        rhs = co_await this->write(conn, slice, finish);
//        if (rhs <= 0)
//            co_return rhs;
//
//        total += static_cast<std::size_t>(rhs);
//
//        if (total == size)
//            break;
//
//        slice = slice.subslice(static_cast<std::size_t>(rhs)).unwrap();
//    }
//    co_return static_cast<ssize_t>(total);
//}
//
//manapi::future<ssize_t> manapi::net::worker::base::fwrite(const shared_conn &conn, manapi::slice &slice, std::size_t size, bool finish) {
//    std::size_t total = 0;
//    ssize_t rhs = 0;
//    std::size_t shifted = 0;
//    auto const slice_size = size;
//    auto sv = slice.subslice(0, slice_size).unwrap();
//
//    if (!size) {
//        if (finish)
//            co_return this->sync_write(conn, sv, finish);
//        co_return 0;
//    }
//
//    while (total < slice_size) {
//        rhs = this->sync_write(conn, sv, finish);
//
//        if (rhs < 0)
//            co_return rhs;
//
//        if (!rhs) {
//            if (size) {
//                assert(!shifted);
//                slice.resize(size).unwrap();
//                assert(slice.size() == size);
//                size = 0;
//            }
//
//            assert(slice.size() == (slice_size - shifted));
//
//            if (shifted != total) {
//                slice.shift_add(total - shifted).unwrap();
//                shifted = total;
//                assert(slice.size() == (slice_size - shifted));
//            }
//
//            sv = slice_view{slice};
//
//            assert(sv.size() == (slice_size - shifted));
//            rhs = co_await write_internal(this, rhs, conn, sv, finish);
//            if (rhs <= 0)
//                co_return rhs;
//        }
//
//        total += static_cast<std::size_t> (rhs);
//
//        if (total == slice_size)
//            break;
//
//        sv = sv.subslice(static_cast<std::size_t>(rhs)).unwrap();
//    }
//
//    co_return static_cast<ssize_t>(total);
//}
//
//void manapi::net::worker::base::event_toggle(const shared_conn & conn, bool state, int flag) MANAPICORE_NOEXCEPT {
//    auto flags = this->event_flags(conn);
//    if (state) {
//        if (!(flags & flag)) {
//            this->event_flags(conn, flags | flag);
//        }
//    }
//    else if (flags & flag) {
//        this->event_flags(conn, flags ^ flag);
//    }
//}
//
//manapi::net::worker::connection::ipdata_t * manapi::net::worker::base::ipdata(worker::connection *conn) MANAPICORE_NOEXCEPT {
//    return conn->ipdata.get();
//}
//
//int64_t manapi::net::worker::base::stream_id(const shared_conn & s) MANAPICORE_NOEXCEPT {
//    return 0;
//}
//
//manapi::net::worker::shared_conn manapi::net::worker::base::stream_id(const shared_conn &conn, int64_t id) MANAPICORE_NOEXCEPT {
//    return nullptr;
//}
//
//std::size_t manapi::net::worker::base::streams_size(const shared_conn &conn) const MANAPICORE_NOEXCEPT {
//    return 0;
//}
//
//manapi::status_or<manapi::reference<manapi::net::worker::connection>> manapi::net::worker::base::new_stream(const shared_conn & conn, int flags) MANAPICORE_NOEXCEPT {
//    return status_unimplemented("worker:Streams are not supported");
//}
//
//int manapi::net::worker::base::notify_cb(manapi::net::worker::worker_watcher_cb &cb, const manapi::net::worker::shared_conn &conn, int flags, manapi::slice_complex *sc) MANAPICORE_NOEXCEPT {
//    this->m_removed_cbs_deps++;
//    int rhs;
//
//    try {
//        cb ( conn, flags, sc );
//        rhs = manapi::ERR_OK;
//    }
//    catch (std::exception const &e) {
//        manapi_log_error("%s failed due to %s", "notify_cb", e.what());
//        rhs = manapi::ERR_ABORTED;
//    }
//
//    if (!--this->m_removed_cbs_deps) {
//        this->m_removed_cbs.clear();
//    }
//
//    return rhs;
//}
