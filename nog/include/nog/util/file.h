
#pragma once
#include "nog/util/time.h"

#include <filesystem>
#include <memory>

namespace nog {
    class FileStatus;

    typedef std::filesystem::path Filepath;
    Filepath get_current_exe_path();

    size_t read_file_data(const char* filename, size_t read_length, std::unique_ptr<char[]>& data);

    size_t read_file_data(const char* filename,
                          size_t read_length,
                          const FileStatus& filestats,
                          std::unique_ptr<char[]>& data);

    static constexpr auto FILE_STAT_OK = 0;

    class FileStatus {
    private:
        // bool exists;
        unsigned int dev;
        unsigned int mode;
        short links;
        short uid;
        short gid;
        size_t fsize;
        Timestamp atime;
        Timestamp mtime;
        Timestamp ctime;
#if OS != WINDOWS_OS
        int blcksz;
        long long blck_cnt;
#endif

    public:
        FileStatus() : FileStatus(0, 0, 0, 0, 0, 0, Timestamp::min(), Timestamp::min(), Timestamp::min()) {}

        FileStatus(unsigned device_number,
                   unsigned access_mode,
                   short hard_link_count,
                   short user_id,
                   short group_id,
                   size_t file_size,
                   Timestamp last_access,
                   Timestamp last_modified,
                   Timestamp last_changed_status)
            : dev{device_number}, mode{access_mode}, links{hard_link_count}, uid{user_id}, gid{group_id},
              fsize{file_size}, atime{last_access}, mtime{last_modified}, ctime{last_changed_status} {}

        unsigned int device_number() const { return dev; }
        unsigned int access_mode() const { return mode; }
        short hard_link_count() const { return links; }
        short user_id() const { return uid; }  // always 0 on windows
        short group_id() const { return gid; } // always 0 on windows
        size_t file_size() const { return fsize; }
        Timestamp last_access_time() const { return atime; }
        Timestamp last_modified_time() const { return mtime; }
        Timestamp last_changed_status_time() const { return ctime; }

        unsigned int& device_number() { return dev; }
        unsigned int& access_mode() { return mode; }
        short& hard_link_count() { return links; }
        short& user_id() { return uid; }  // always 0 on windows
        short& group_id() { return gid; } // always 0 on windows
        size_t& file_size() { return fsize; }
        Timestamp& last_access_time() { return atime; }
        Timestamp& last_modified_time() { return mtime; }
        Timestamp& last_changed_status_time() { return ctime; }
        bool exists() const { return device_number() != 0; }

#if OS != WINDOWS_OS
        int block_size() const { return blcksz; }
        long long block_count() const { return blck_cnt; }

        int& block_size() { return blcksz; }
        long long& block_count() { return blck_cnt; }
#endif
    };

    enum class StatModeType { block_device, char_device, directory, fifo_pipe, symlink, regular, socket, unknown };

    StatModeType file_stat_mode_to_type(unsigned int st_mode);

    int get_stats(const char* filename,
                  unsigned int& device_id,
                  unsigned int& access_mode,
                  short& link_count,
                  short& user_id,  // doesn't mean anything on windows
                  short& group_id, // doesn't mean anything on windows
                  size_t& filesize,
                  Timestamp& last_access, // last_access and last_modified are the same on windows
                  Timestamp& last_modified,
                  Timestamp& last_status_change
#if OS != WINDOWS_OS
                  ,
                  int& block_size,
                  long long& block_count
#endif
    );

    inline int get_stats(const char* filename, FileStatus& s) {
        // file_status s;
        return get_stats(filename,
                         s.device_number(),
                         s.access_mode(),
                         s.hard_link_count(),
                         s.user_id(),
                         s.group_id(),
                         s.file_size(),
                         s.last_access_time(),
                         s.last_modified_time(),
                         s.last_changed_status_time()
#if OS != WINDOWS_OS
                             ,
                         s.block_size(),
                         s.block_count()
#endif
        );
    }

    size_t read_file_data(const char* filename, size_t read_length, std::unique_ptr<char[]>& data);

    size_t read_file_data(const char* filename,
                          size_t read_length,
                          const FileStatus& filestats,
                          std::unique_ptr<char[]>& data);
} // namespace nog
