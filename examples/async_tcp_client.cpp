#include <ark/buffer/buffer.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/io/async.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/async.hpp>

using ark::async_context;
using ark::buffer;
using ark::ec_or;
using ark::error_code;
using ark::mutable_buffer;
using ark::panic_on_ec;
using ark::transfer_at_least;
namespace async = ark::async;
namespace net = ark::net;

void read_and_print(net::tcp::socket &s) {
  static std::array<char, 1024> rd_buf;
  async::read(s, buffer(rd_buf), transfer_at_least(1), [&s](ec_or<size_t> ret) {
    size_t sz = panic_on_ec(ret);
    std::cout.write(rd_buf.data(), sz);
    std::cout.flush();
  });
}

int main(void) {

  net::inet_address ep;
  ep.host("198.199.109.141"); // isocpp.org
  ep.port(80);

  async_context ctx;
  panic_on_ec(ctx.init());

  net::tcp::socket s{panic_on_ec(net::tcp::socket::create(ctx))};

  net::tcp::async::connect(s, ep, [&ctx, &s](error_code ret) {
    panic_on_ec(ret);
    static const std::string wr_buf{
        "GET /index.html HTTP/1.1\r\nHost: isocpp.org\r\n\r\n"};
    async::write(s, buffer(wr_buf), [&ctx, &s](ec_or<size_t> ret) {
      panic_on_ec(ret);
      read_and_print(s);
    });
  });

  panic_on_ec(ctx.run());
}
