#pragma once

#include <filesystem>

#include "../ManapiUtils.hpp"
#include "../json/ManapiJson.hpp"
#include "../std/ManapiContext.hpp"
#include "../ManapiAsync.hpp"
#include "../std/ManapiCancelToken.hpp"

namespace manapi::fs {
    manapi::future<ev::status_or<bool>> exists (std::string path, manapi::ctoken cancellation = nullptr);

    manapi::future<ev::status_or<std::chrono::system_clock::time_point>> last_time_write (std::string path, manapi::ctoken cancellation = nullptr);

    manapi::future<ev::status> mkdir (std::string path, int mode = 0755, bool recursive = true, manapi::ctoken cancellation = nullptr);

    future<ev::status_or<ev::unique_file>> open (std::string path, int flags, int mode, manapi::ctoken cancellation = nullptr);

    future<ev::status> close (ev::file file, ctoken cancellation = nullptr);

    future<ssize_t> write (ev::file file, const void *data, std::size_t size, int64_t offset = -1, manapi::ctoken cancellation = nullptr);

    future<ssize_t> read (ev::file file, void *data, std::size_t size, int64_t offset = -1, manapi::ctoken cancellation = nullptr);

    future<ev::status> write (std::string path, std::string data, int mode, int flags = ev::FS_O_WRONLY|ev::FS_O_CREAT|ev::FS_O_APPEND, int64_t offset = -1, manapi::ctoken cancellation = nullptr);

    future<ev::status_or<std::string>> read (std::string path, int flags = ev::FS_O_RDONLY, int64_t offset = -1, manapi::ctoken cancellation = nullptr);

    /**
     * Async Write
     *
     * @param file
     * @param buff
     * @param nbuff
     * @param offset
     * @param cancellation
     * @return
     */
    future<ssize_t> write (ev::file file, ev::buff_t *buff, uint32_t nbuff, int64_t offset = -1, ctoken cancellation = nullptr);

    /**
     * Async Read
     *
     * @param file
     * @param buff
     * @param nbuff
     * @param offset
     * @param cancellation
     * @return
     */
    future<ssize_t> read (ev::file file, ev::buff_t *buff, uint32_t nbuff, int64_t offset = -1, ctoken cancellation = nullptr);

    future<ssize_t> write (ev::file file, slice_view slice, int64_t offset = -1, ctoken cancellation = nullptr);

    future<ssize_t> read (ev::file file, slice_view slice, int64_t offset = -1, ctoken cancellation = nullptr);

    future<ev::status_or<uint64_t>> file_size (std::string path, manapi::ctoken cancellation = nullptr);

    future<ev::status_or<uint64_t>> file_size (ev::file file, manapi::ctoken cancellation = nullptr);

    future<ev::status> unlink (std::string path, ctoken cancellation = nullptr);

    future<ev::status> rmdir (std::string path, ctoken cancellation = nullptr);

    future<ev::status> rmdir_all (std::string path, ctoken cancellation = nullptr);

    future<ev::status_or<std::unique_ptr<ev::dir_t, ev::dir_deleter_t>>> opendir (std::string path, ctoken ctoken = nullptr);

    future<ev::status> closedir (ev::dir_t *directory, ctoken cancellation = nullptr);

    future<ev::status> statfs (std::string path, std::move_only_function<void(ev::statfs_t *data)> callback, ctoken cancellation = nullptr);

    /**
     * stat
     *
     * @param path Path to the file
     * @param callback Callback which accept a result. If the file doesn't exist, it has a @code nullptr@endcode value
     * @param cancellation Cancellation Token
     * @return
     */
    future<ev::status> stat (std::string path, std::move_only_function<void(ev::stat_t *data)> callback, ctoken cancellation = nullptr);

    /**
     * fstat
     *
     * @param file Path to the file
     * @param callback Callback which accept a result. If the file doesn't exist, it has a @code nullptr@endcode value
     * @param cancellation Cancellation token
     * @return
     */
    future<ev::status> fstat (ev::file file, std::move_only_function<void(ev::stat_t *data)> callback, ctoken cancellation = nullptr);

    future<ev::status> rename (std::string oldpath, std::string newpath, ctoken cancellation = nullptr);

    future<ev::status> copyfile (std::string src, std::string dest, int flags, ctoken cancellation = nullptr);

    future<ev::status> chmod (std::string path, int mode, ctoken cancellation = nullptr);

    future<ev::status> fchmod (ev::file file, int mode, ctoken cancellation = nullptr);

    future<ev::status> utime (std::string path, double atime, double mtime, ctoken cancellation = nullptr);

    future<ev::status> futime (ev::file file, double atime, double mtime, ctoken cancellation = nullptr);

    future<ev::status> link (std::string path, std::string newpath, ctoken cancellation = nullptr);

    future<ev::status> symlink (std::string path, std::string newpath, int flags, ctoken cancellation = nullptr);

    future<ev::status_or<std::string>> readlink (std::string path, ctoken cancellation = nullptr);

    future<ev::status_or<std::string>> realpath (std::string path, ctoken cancellation = nullptr);

    future<ev::status> chown (std::string path, ev::uid_t uid, ev::gid_t gid, ctoken cancellation = nullptr);

    future<ev::status> fchown (ev::file file, ev::uid_t uid, ev::gid_t gid, ctoken cancellation = nullptr);

    future<ev::status> fsync (ev::file file, ctoken cancellation = nullptr);

    future<ev::status_or<std::string>> mkdtemp (std::string tpl, ctoken cancellation = nullptr);

    future<ev::status_or<std::pair<std::string, ev::unique_file>>> mkstemp (std::string tpl, ctoken cancellation = nullptr);

    future<ev::status> fdatasync (ev::file file, ctoken cancellation = nullptr);

    future<ev::status> ftruncate (ev::file file, int64_t offset, ctoken cancellation = nullptr);

    future<ev::status> access (std::string path, int mode, ctoken cancellation = nullptr);

    future<ev::status_or<std::size_t>> scandir (std::string path, int flags, std::move_only_function<void(ev::dir_t *, std::size_t)> callback, ctoken cancellation = nullptr);

    future<ev::status> readdir (ev::dir_t *dir, std::move_only_function<bool (ev::dir_t *, std::size_t)> callback, ctoken cancellation = nullptr);
}


namespace manapi::fs::path {
    static constexpr char delimiter = std::filesystem::path::preferred_separator;

    static constexpr std::string_view string_delimiter (&delimiter, 1);

    std::string_view basename (std::string_view path);

    std::string_view extension (std::string_view path);

    void append_delimiter(std::string &path);

    std::string serialize (std::string_view str);

    std::string absolute (std::string_view path);

    std::string root_directory ();

    std::string_view back (std::string_view str);

    void append (std::string &path, std::string_view next, bool root);

    void append (std::string &path, std::string_view next);

    std::string current_path ();

    template <class... Args>
    std::string join(std::string path, Args&&...args) {
        std::size_t i = 0;
        (..., path::append(path, std::forward<Args>(args), !(i++) ));
        return std::move(path);
    }
}