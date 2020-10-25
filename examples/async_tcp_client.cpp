#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/buffer_view.hpp>
#include <iasr/error/ec_or.hpp>
#include <iasr/io/async.hpp>
#include <iasr/net/address.hpp>
#include <iasr/net/tcp/async.hpp>

using iasr::async_context;
using iasr::buffer;
using iasr::buffer_view;
using iasr::ec_or;
using iasr::error_code;
using iasr::panic_on_ec;
namespace async = iasr::async;
namespace net = iasr::net;

void read_and_print(net::tcp::socket &s) {
  static buffer rd_buf{1024};
  async::read_some(s, rd_buf, [&s](ec_or<size_t> ret) {
    size_t sz = panic_on_ec(ret);
    std::cout.write(rd_buf.data(), sz);
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
    async::write(s, wr_buf, [&ctx, &s](ec_or<size_t> ret) {
      panic_on_ec(ret);
      read_and_print(s);
    });
  });

  panic_on_ec(ctx.run());
}
