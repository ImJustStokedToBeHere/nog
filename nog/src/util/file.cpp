#include "nog/util/file.h"

#include "nog/platform_def.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sys/stat.h>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif defined(__linux__)
    #include <limits.h>
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#endif

namespace nog {

    Filepath get_current_exe_path() {
#if defined(_WIN32)
        wchar_t buffer[MAX_PATH];
        DWORD size = GetModuleFileNameW(NULL, buffer, MAX_PATH);
        if (size == 0 || size == MAX_PATH) {
            return {}; // Handle error or path truncation
        }
        return Filepath(buffer);

#elif defined(__linux__)
        char buffer[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len == -1) {
            return {}; // Handle error
        }
        buffer[len] = '\0';
        return Filepath(buffer);

#elif defined(__APPLE__)
        char buffer[PATH_MAX];
        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) == 0) {
            return Filepath(buffer).lexically_normal();
        }
        return {}; // Handle error or handle dynamic sizing if path is longer
#else
        return {}; // Unsupported OS
#endif
    }

    namespace jinn::fs {
#if OS == WINDOWS_OS
        StatModeType file_stat_mode_to_type(unsigned int st_mode) {
            switch (st_mode & S_IFMT) {
                case _S_IFCHR:
                    return StatModeType::char_device;
                case _S_IFDIR:
                    return StatModeType::directory;
                case _S_IFIFO:
                    return StatModeType::fifo_pipe;
                case _S_IFREG:
                    return StatModeType::regular;
                default:
                    return StatModeType::unknown;
            }
        }
#else
        StatModeType file_stat_mode_to_type(unsigned int st_mode) {
            switch (st_mode & S_IFMT) {
                case S_IFBLK:
                    return StatModeType::block_device;
                case S_IFCHR:
                    return StatModeType::char_device;
                case S_IFDIR:
                    return StatModeType::directory;
                case S_IFIFO:
                    return StatModeType::fifo_pipe;
                case S_IFLNK:
                    return StatModeType::symlink;
                case S_IFREG:
                    return StatModeType::regular;
                case S_IFSOCK:
                    return StatModeType::socket;
                default:
                    return StatModeType::unknown;
            }
        }
#endif

        int get_stats(const char* filename,
                      unsigned int& drive_number,
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
        ) {

#if OS == WINDOWS_OS
            struct _stat64 status;
            auto result = _stat64(filename, &status);

            if (result == 0) {
                drive_number = status.st_dev;
                access_mode = status.st_mode;
                link_count = status.st_nlink;
                user_id = status.st_uid;
                group_id = status.st_gid;
                filesize = status.st_size;
                last_access = sys_clock::from_time_t(status.st_atime);
                last_modified = sys_clock::from_time_t(status.st_mtime);
                last_status_change = sys_clock::from_time_t(status.st_ctime);

                return fs::stats_ok;
            }
#else
            struct stat64 status;
            auto result = stat64(filename, &status);

            if (result == 0) {
                drive_number = status.st_dev;
                access_mode = status.st_mode;
                link_count = status.st_nlink;
                user_id = status.st_uid;
                group_id = status.st_gid;
                filesize = status.st_size;
                last_access = SystemClock::from_time_t(status.st_atime);
                last_modified = SystemClock::from_time_t(status.st_mtime);
                last_status_change = SystemClock::from_time_t(status.st_ctime);
                block_size = status.st_blksize;
                block_count = status.st_blocks;
                return FILE_STAT_OK;
            }
#endif

            return result;
        }

        size_t read_file_data(const char* filename,
                              size_t read_length,
                              const FileStatus& status,
                              std::unique_ptr<char[]>& data) {

            if (status.exists()) {
                // the file exists, if the read_length argument has a value of 0 then use the retrieved filesize as
                // the read length
                read_length = read_length == 0 ? status.file_size() : read_length;
            } else {
                // the file doesn't exist or there was some other problem calling stat on in
                data = nullptr;
                return 0;
            }

            // lets actually open the file and get our data
            std::ifstream is(filename, std::ios::binary);

            if (is.is_open()) {
                // open success, lets do it!
                // acquire only enough for the read length
                data = std::make_unique<char[]>(read_length);
                is.seekg(std::ios::beg);
                is.read(reinterpret_cast<char*>(data.get()), read_length);
            }

            return read_length;
        }

        size_t read_file_data(const char* filename, size_t read_length, std::unique_ptr<char[]>& data) {
            size_t filesize = 0;
            auto last_modified = Timestamp::min();
            // lets do a stat call on the file and make sure it exists
            FileStatus status;
            auto stat_result = get_stats(filename, status);

            return nog::read_file_data(filename, read_length, status, data);
        }
    } // namespace jinn::fs
} // namespace nog
