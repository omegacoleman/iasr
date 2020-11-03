#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <function2/function2.hpp>
#include <gsl/gsl>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <outcome.hpp>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

namespace outcome = OUTCOME_V2_NAMESPACE;

namespace ark {
using byte = char;
using fu2::function;
using fu2::unique_function;
using gsl::dynamic_extent;
using gsl::not_null;
using gsl::span;
using gsl::strict_not_null;
using outcome::result;
using outcome::success;
using std::abort;
using std::add_const_t;
using std::addressof;
using std::array;
using std::basic_string;
using std::basic_string_view;
using std::begin;
using std::cbegin;
using std::cend;
using std::cerr;
using std::conditional_t;
using std::copy;
using std::declval;
using std::enable_if_t;
using std::end;
using std::endl;
using std::errc;
using std::error_code;
using std::exchange;
using std::false_type;
using std::fill;
using std::forward;
using std::is_const_v;
using std::is_convertible_v;
using std::is_pointer_v;
using std::is_standard_layout_v;
using std::is_trivially_copyable_v;
using std::is_void_v;
using std::istringstream;
using std::list;
using std::lock_guard;
using std::make_pair;
using std::make_unique;
using std::map;
using std::max;
using std::min;
using std::move;
using std::mutex;
using std::numeric_limits;
using std::optional;
using std::ostringstream;
using std::pair;
using std::remove_const_t;
using std::size_t;
using std::string;
using std::stringstream;
using std::system_category;
using std::system_error;
using std::terminate;
using std::true_type;
using std::unique_ptr;
using std::vector;
} // namespace ark

#include <ark/misc/ign_pipe.hpp>
