
#include "nog/source.h"

#include "nog/util/file.h"

namespace nog {
    const Span& Span::EMPTY = Span(0, 0);
    const Span& Span::INVALID = Span(0, 0);

    SpanExt Span::to_extended() const {
        NOG_ASSERT((this->flags() & Span::CACHED_MARKER) == Span::CACHED_MARKER,
                   "cannot create SpanExt from uncached Span");
        return SpanExt(this->start_pos(), this->end_pos());
    }
    auto Span::operator<=>(const Span& other) const {
        return this->to_extended().operator<=>(other.to_extended());
    }

    bool Span::operator==(const Span& other) const { return this->to_extended() == other.to_extended(); }

    bool Span::operator!=(const Span& other) const { return this->to_extended() != other.to_extended(); }

    std::ostream& operator<<(std::ostream& output, const Span& s) {
        return output << '(' << s.start_or_idx << ',' << s.len_or_flags << ')';
    }
    // bool Span::is_empty() const { return *this == Span::EMPTY; }
    //

    PathResolver::PathResolver() {}
    PathResolver::PathResolver(std::shared_ptr<BuildSettings> settings) : search_dirs{}, lib_dirs{}, sys_dirs{} {}

    std::expected<Filepath, SrcErr> PathResolver::application_dir() const {
        auto app_path = get_current_exe_path();
        if (std::filesystem::exists(app_path)) {
            return app_path.parent_path();
        }

        return std::unexpected(SrcErr::ApplicationDirNotFound);
    }

    std::expected<Filepath, SrcErr> PathResolver::working_dir() const {
        auto found_path = std::filesystem::current_path();
        if (std::filesystem::exists(found_path)) {
            return found_path;
        }

        return std::unexpected(SrcErr::WorkingDirNotFound);
    }

    std::vector<std::string> PathResolver::include_dirs() const {
        auto result = this->search_dirs;
        result.push_back("/");
        return result;
    }

    std::vector<std::string> PathResolver::library_dirs() const {
        auto result = this->lib_dirs;
        result.push_back("/");
        return result;
    }

    std::vector<std::string> PathResolver::system_dirs() const {
        auto result = this->sys_dirs;
        result.push_back("/");
        return result;
    }

    std::expected<std::string, SrcErr> PathResolver::resolve_path(const std::string& path,
                                                                  PathResolutionKind resolution_kind) const {
        auto plain_path = Filepath(path);

        if (std::filesystem::exists(plain_path)) {
            auto canon = std::filesystem::canonical(plain_path);
            return canon.string();
        }

        Filepath dir;
        if (auto app_dir = this->application_dir(); app_dir.has_value()) {
            dir = app_dir.value();
            dir /= plain_path;
            if (std::filesystem::exists(dir)) {
                auto canon = std::filesystem::canonical(dir);
                return canon.string();
            }
        }

        if (auto working_dir = this->working_dir(); working_dir.has_value()) {
            dir = working_dir.value();
            dir /= plain_path;
            if (std::filesystem::exists(dir)) {
                auto canon = std::filesystem::canonical(dir);
                return canon.string();
            }
        }

        static constexpr PathResolutionKind FILE_TYPES[3] = {PathResolutionKind::Inc,
                                                             PathResolutionKind::Lib,
                                                             PathResolutionKind::Sys};

        std::vector<Filepath> search_paths;
        for (const auto& kind_itr : FILE_TYPES) {
            auto res_type = kind_itr & resolution_kind;
            std::vector<std::string>::const_iterator begin;

            std::vector<std::string>::const_iterator end;

            switch (res_type) {
                case PathResolutionKind::Sys: {
                    begin = this->sys_dirs.begin();
                    end = this->sys_dirs.end();
                    break;
                }
                case PathResolutionKind::Lib: {
                    begin = this->lib_dirs.begin();
                    end = this->lib_dirs.end();

                    break;
                }
                case PathResolutionKind::Inc: {
                    begin = this->search_dirs.begin();
                    end = this->search_dirs.end();

                    break;
                }
                default: {
                    // if there is nothing then we should get out of here because there is an error in the caller
                    // return Error(SrcCodeErrKind::FilenameNotResolved);
                    return std::unexpected(SrcErr::FilenameNotResolved);
                }
            }

            // morph the string lists into a list of Filepaths
            std::transform(begin, end, search_paths.begin(), [](const auto& s) { return Filepath(s); });

            for (const auto& sp : search_paths) {
                auto start_path = sp;
                start_path /= plain_path;
                if (std::filesystem::exists(start_path)) {
                    auto canon = std::filesystem::canonical(start_path);
                    return canon.string();
                }
            }
        }

        return std::unexpected(SrcErr::FilenameNotResolved);
    }

    SrcId src_id_from_name_and_pkg(const std::string& name, const PkgId& pkg) {
        size_t seed{0};
        combine_hashes(seed, name);
        combine_hashes(seed, pkg.value());
        return SrcId(seed);
    }

    RetentionPolicy retention_policy_from_src_kind(SrcKind kind) {
        switch (kind) {
            case SrcKind::UserFile:
            case SrcKind::AnonFile:
            case SrcKind::MacroExpansion: {
                return RetentionPolicy::Static;
            }
            default:
                return RetentionPolicy::Volatile;
        }
    }

    std::string error_msg(const SrcErr& err) {
        switch (err) {
            case nog::SrcErr::SrcFileTooLarge: {
                return "source file is too large. 4 GB is the maximum single file size.";
            }
            case SrcErr::ApplicationDirNotFound: {
                return "current exe directory not found";
            }
            case SrcErr::WorkingDirNotFound: {
                return "failed to find working directory. There is something very very very wrong.";
            }
            case SrcErr::FilenameNotResolved: {
                return "filename not resolved";
            }
            case SrcErr::TryingToResolveVirtualFile: {
                return "trying to resolve virtual file in physical filesystem";
            }
            default:
                return "unknown SrcCodeErrKind";
        }
    }
    std::expected<std::string, SrcErr> SrcManager::get_text_from_file(const std::string& filename) {
        std::unique_ptr<char[]> data;
        if (read_file_data(filename.c_str(), 0, data) > 0) {
            return std::string(data.get());
        }

        return std::unexpected(SrcErr::FailedToReadSrcFile);
    }
} // namespace nog
