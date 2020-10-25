#pragma once

#include <iasr/bindings.hpp>

#include <iasr/async/context.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/fd.hpp>

namespace iasr {
class normal_file : public seekable_fd {
protected:
  normal_file(int fd_int) : seekable_fd(fd_int) {}

private:
  static ec_or<normal_file> __open(async_context *ctx, string path, int flags,
                                   mode_t mode) noexcept {
    int ret = clinux::open(path.c_str(), flags, mode);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    normal_file ret_fd(ret);
    ret_fd.set_async_context(ctx);
    return move(ret_fd);
  }

  static ec_or<normal_file> __mkostemp(async_context *ctx, const string &templ,
                                       int flags) noexcept {
    int ret = clinux::mkostemp(const_cast<char *>(templ.c_str()), flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    normal_file ret_fd(ret);
    ret_fd.set_async_context(ctx);
    return move(ret_fd);
  }

public:
  static ec_or<normal_file> open(string path, int flags) noexcept {
    return __open(nullptr, path, flags, 0755);
  }

  static ec_or<normal_file> open(string path, int flags, mode_t mode) noexcept {
    return __open(nullptr, path, flags, mode);
  }

  static ec_or<normal_file> open(async_context &ctx, string path,
                                 int flags) noexcept {
    return __open(&ctx, path, flags, 0755);
  }

  static ec_or<normal_file> open(async_context &ctx, string path, int flags,
                                 mode_t mode) noexcept {
    return __open(&ctx, path, flags, mode);
  }

  static ec_or<normal_file> mkostemp(const string &templ, int flags) noexcept {
    return __mkostemp(nullptr, templ, flags);
  }

  static ec_or<normal_file> mkostemp(async_context &ctx, const string &templ,
                                     int flags) noexcept {
    return __mkostemp(&ctx, templ, flags);
  }
};
} // namespace iasr
