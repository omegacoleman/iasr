#include <functional>
#include <ark/buffer/buffer.hpp>
#include <ark/error/ec_or.hpp>
#include <ark/io/async.hpp>
#include <ark/net/address.hpp>
#include <ark/net/tcp/acceptor.hpp>
#include <ark/net/tcp/async.hpp>
#include <ark/net/tcp/general.hpp>
#include <ark/net/tcp/socket.hpp>

#define PRINT_ACCESS_LOG

using ark::async_context;
using ark::buffer;
using ark::ec_or;
using ark::panic_on_ec;
using ark::transfer_at_least;
namespace async = ark::async;
namespace net = ark::net;
namespace tcp = net::tcp;

struct echo_service : public std::enable_shared_from_this<echo_service> {
  tcp::socket s_;
  std::array<char, 1024> buf_;

#ifdef PRINT_ACCESS_LOG
  net::address addr_;

  echo_service(tcp::socket s, net::address addr)
      : s_(std::move(s)), addr_(std::move(addr)) {
    std::cout << "connected with peer " << peer_name() << std::endl;
  }

  ~echo_service() {
    std::cout << "closed connection to " << peer_name() << std::endl;
  }

  std::string peer_name() {
    auto ret = to_string(addr_);
    if (!ret)
      return std::string{"["} + ret.ec().message() + "]";
    return ret.get();
  }

#else
  echo_service(async_context &ctx, tcp::socket s) : s_(std::move(s)) {}
#endif

  void do_echo() {
    async::read(s_, buffer(buf_), transfer_at_least(1),
                std::bind(&echo_service::handle_read, shared_from_this(),
                          std::placeholders::_1));
  }

  void handle_write(ec_or<size_t> ret) {
    if (!ret) {
      std::cerr << ret.ec().message() << std::endl;
      return;
    }
    do_echo();
  }

  void handle_read(ec_or<size_t> ret) {
    if (!ret) {
      std::cerr << ret.ec().message() << std::endl;
      return;
    }
    size_t sz = ret.get();
    if (sz == 0)
      return;
    async::write(s_, buffer(buf_, sz),
                 std::bind(&echo_service::handle_write, shared_from_this(),
                           std::placeholders::_1));
  }
};

struct echo_server : public std::enable_shared_from_this<echo_server> {
  tcp::acceptor ac_;
  net::address addr_;

  echo_server(tcp::acceptor ac) : ac_(std::move(ac)) {}

  void run() {
#ifdef PRINT_ACCESS_LOG
    tcp::async::accept(ac_, addr_,
                       std::bind(&echo_server::handle_connection,
                                 shared_from_this(), std::placeholders::_1));
#else
    tcp::async::accept(ac_,
                       std::bind(&echo_server::handle_connection,
                                 shared_from_this(), std::placeholders::_1));
#endif
  }

  void handle_connection(ec_or<tcp::socket> ret) {
    if (!ret) {
      std::cerr << ret.ec().message() << std::endl;
      return;
    }
#ifdef PRINT_ACCESS_LOG
    auto svc = std::make_shared<echo_service>(ret.get(), std::move(addr_));
#else
    auto svc = std::make_shared<echo_service>(ret.get());
#endif
    svc->do_echo();

    run();
  }
};

int main(void) {
  async_context ctx;
  panic_on_ec(ctx.init());

  net::inet_address ep;
  ep.host("127.0.0.1");
  ep.port(8080);

  tcp::acceptor ac{panic_on_ec(tcp::acceptor::create(ctx))};
  panic_on_ec(tcp::bind(ac, ep));
  panic_on_ec(tcp::listen(ac));

  auto srv = std::make_shared<echo_server>(std::move(ac));
  srv->run();

  ctx.run();

  return 0;
}
