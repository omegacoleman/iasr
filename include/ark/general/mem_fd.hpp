#pragma once

#include <ark/bindings.hpp>

#include <ark/error/ec_or.hpp>
#include <ark/io/fd.hpp>

namespace ark {
class mem_fd : public seekable_fd {
protected:
  mem_fd(int fd_int) : seekable_fd(fd_int) {}

private:
  static ec_or<mem_fd> __create(async_context *ctx, const string &name,
                                int flags) noexcept {
    int ret = clinux::memfd_create(name.c_str(), flags);
    if (ret == -1) {
      return clinux::errno_ec();
    }
    mem_fd f{ret};
    f.set_async_context(ctx);
    return move(f);
  }

public:
  static ec_or<mem_fd> create(const string &name, int flags) noexcept {
    return __create(nullptr, name, flags);
  }

  static ec_or<mem_fd> create(async_context &ctx, const string &name,
                              int flags) noexcept {
    return __create(&ctx, name, flags);
  }
};
} // namespace ark