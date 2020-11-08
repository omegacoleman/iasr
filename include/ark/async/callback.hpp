#pragma once

#include <ark/bindings.hpp>

#include <ark/error/ec_or.hpp>

namespace ark {

template <typename RetType>
struct callback : public unique_function<void(RetType result)> {
  using unique_function<void(RetType result)>::unique_function;
  using result_type = RetType;
};

template <> struct callback<void> : public unique_function<void()> {
  using unique_function<void()>::unique_function;
  using result_type = void;
};

using syscall_callback_t = callback<ec_or<long>>;

} // namespace ark