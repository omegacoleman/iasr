#pragma once

#include <iasr/bindings.hpp>
#include <iasr/clinux.hpp>

#include <iasr/buffer/buffer.hpp>
#include <iasr/error/ec_or.hpp>

namespace iasr {
namespace net {

class in_address_impl {
public:
  static const clinux::sa_family_t address_family = AF_INET;
  static const clinux::socklen_t size = sizeof(clinux::sockaddr_in);
  using sockaddr_ptr_t = clinux::sockaddr_in *;
  using const_sockaddr_ptr_t = const clinux::sockaddr_in *;

  static ec_or<string> host(const_sockaddr_ptr_t sa) noexcept {
    char buff[16];
    const char *s =
        clinux::inet_ntop(AF_INET, &sa->sin_addr, buff, sizeof(buff));
    if (s == nullptr) {
      return clinux::errno_ec();
    }
    return {s};
  }

  static error_code host(sockaddr_ptr_t sa, string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(), &sa->sin_addr);
    if (ret <= 0)
      return error_code(EINVAL, system_category());
    return {};
  }

  static unsigned short port(const_sockaddr_ptr_t sa) noexcept {
    return clinux::ntohs(sa->sin_port);
  }

  static void port(sockaddr_ptr_t sa, unsigned short p) noexcept {
    sa->sin_port = clinux::htons(p);
  }

  static ec_or<string> to_string(const_sockaddr_ptr_t sa) noexcept {
    ostringstream oss;
    auto host_ret = host(sa);
    IASR_PASS_EC_ON(host_ret);
    string host_s = host_ret.get();
    oss << host_s << ":" << port(sa);
    return oss.str();
  }
};

class in6_address_impl {
public:
  static const clinux::sa_family_t address_family = AF_INET6;
  static const clinux::socklen_t size = sizeof(clinux::sockaddr_in6);
  using sockaddr_ptr_t = clinux::sockaddr_in6 *;
  using const_sockaddr_ptr_t = const clinux::sockaddr_in6 *;

  static ec_or<string> host(const_sockaddr_ptr_t sa) noexcept {
    char buff[64];
    const char *s =
        clinux::inet_ntop(AF_INET6, &sa->sin6_addr, buff, sizeof(buff));
    if (s == nullptr) {
      return clinux::errno_ec();
    }
    return {s};
  }

  static error_code host(sockaddr_ptr_t sa, string host_s) noexcept {
    int ret = clinux::inet_pton(address_family, host_s.c_str(), &sa->sin6_addr);
    if (ret <= 0)
      return error_code(EINVAL, system_category());
    return {};
  }

  static unsigned short port(const_sockaddr_ptr_t sa) noexcept {
    return clinux::ntohs(sa->sin6_port);
  }

  static void port(sockaddr_ptr_t sa, unsigned short p) noexcept {
    sa->sin6_port = clinux::htons(p);
  }

  static ec_or<string> to_string(const_sockaddr_ptr_t sa) noexcept {
    ostringstream oss;
    auto host_ret = host(sa);
    IASR_PASS_EC_ON(host_ret);
    string host_s = host_ret.get();
    oss << "[" << host_s << "]:" << port(sa);
    return oss.str();
  }
};

template <class Impl> class address_with_family;

class address {
protected:
  buffer b_;

public:
  template <class Impl> friend class address_with_family;

  address() : b_() {}
  address(buffer b) : b_(move(b)) {}

  address(const address &o) : b_(o.b_.cbegin(), o.b_.cend()) {}
  address &operator=(const address &o) {
    b_ = buffer{o.b_.cbegin(), o.b_.cend()};
    return *this;
  }

  operator bool() const noexcept { return b_.data() != nullptr; }

  clinux::sa_family_t sa_family() const noexcept { return sa_ptr()->sa_family; }
  clinux::sockaddr *sa_ptr() noexcept {
    return reinterpret_cast<clinux::sockaddr *>(b_.data());
  }
  const clinux::sockaddr *sa_ptr() const noexcept {
    return reinterpret_cast<const clinux::sockaddr *>(b_.data());
  }
  clinux::socklen_t sa_len() const noexcept {
    return static_cast<clinux::socklen_t>(b_.size());
  }
};

template <class Impl> class address_with_family : public address {
public:
  using size_type = clinux::socklen_t;

protected:
  typename Impl::sockaddr_ptr_t sa_ptr() noexcept {
    return reinterpret_cast<typename Impl::sockaddr_ptr_t>(b_.data());
  }
  typename Impl::const_sockaddr_ptr_t sa_ptr() const noexcept {
    return reinterpret_cast<typename Impl::const_sockaddr_ptr_t>(b_.data());
  }
  size_type sa_len() const noexcept { return Impl::size; }

public:
  address_with_family() noexcept : address(buffer{Impl::size}) {
    address::sa_ptr()->sa_family = Impl::address_family;
  }
  address_with_family(buffer b) noexcept : address(move(b)) {
    Ensures(address::sa_ptr()->sa_family == Impl::address_family);
  }

  address &to_address() noexcept { return *dynamic_cast<address *>(this); }

  static ec_or<address_with_family<Impl>> from_address(address addr) noexcept {
    if (addr.sa_ptr()->sa_family != Impl::address_family)
      return error_code(EAFNOSUPPORT, system_category());
    return address_with_family<Impl>(move(addr.b_));
  }

  ec_or<string> host() const noexcept { return Impl::host(sa_ptr()); }

  error_code host(string host_s) noexcept {
    return Impl::host(sa_ptr(), move(host_s));
  }

  unsigned short port() const noexcept { return Impl::port(sa_ptr()); }

  void port(unsigned short p) noexcept { Impl::port(sa_ptr(), p); }

  ec_or<string> to_string() const noexcept { return Impl::to_string(sa_ptr()); }
};

using inet_address = address_with_family<in_address_impl>;
using inet6_address = address_with_family<in6_address_impl>;

ec_or<string> to_string(address addr) {
  if (addr.sa_family() == AF_INET) {
    auto ret = inet_address::from_address(move(addr));
    if (!ret)
      return ret.ec();
    return ret.get().to_string();
  } else if (addr.sa_family() == AF_INET6) {
    auto ret = inet6_address::from_address(move(addr));
    if (!ret)
      return ret.ec();
    return ret.get().to_string();
  }
  return error_code(EAFNOSUPPORT, system_category());
}

} // namespace net
} // namespace iasr
