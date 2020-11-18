#pragma once

#include <ark/bindings.hpp>

namespace ark {

/*! \addtogroup io
 *  @{
 */

/*! \cond HIDDEN_CLASSES */

struct transfer_all_t {
  template <class ConstBufferSequence>
  size_t operator()(const ConstBufferSequence &b, size_t done) noexcept {
    return (done < buffer_size(b)) ? numeric_limits<size_t>::max() : 0;
  }
};

struct transfer_at_least_t {
  size_t n;
  transfer_at_least_t(size_t n) : n(n) {}

  template <class ConstBufferSequence>
  size_t operator()(const ConstBufferSequence &b, size_t done) noexcept {
    return (done < min(n, buffer_size(b))) ? numeric_limits<size_t>::max() : 0;
  }
};

struct transfer_exactly_t {
  size_t n;
  transfer_exactly_t(size_t n) : n(n) {}

  template <class ConstBufferSequence>
  size_t operator()(const ConstBufferSequence &b, size_t done) noexcept {
    size_t sz = buffer_size(b);
    return (done < sz) ? (done - sz) : 0;
  }
};

/*! \endcond */

/*!
 * \brief returns a CompletionCondition which denotes transfering until all
 * bytes in provided buffer is done.
 *
 * \remark see \ref info_network
 */
constexpr auto transfer_all() noexcept { return transfer_all_t{}; }

/*!
 * \brief returns a CompletionCondition which denotes transfering at least n
 * bytes, or until all bytes in provided buffer is done.
 *
 * \remark see \ref info_network
 */
auto transfer_at_least(size_t n) noexcept { return transfer_at_least_t{n}; }

/*!
 * \brief returns a CompletionCondition which denotes transfering exactly n
 * bytes, or until all bytes in provided buffer is done.
 *
 * \remark see \ref info_network
 */
auto transfer_exactly(size_t n) noexcept { return transfer_exactly_t{n}; }

/*! @} */

} // namespace ark
