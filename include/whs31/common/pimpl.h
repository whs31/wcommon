#pragma once

#include <memory>

#include <whs31/common/memory/propagate_const.h>
#include <whs31/common/detail/namespace.h>

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Pointer-to-implementation pattern trait.
 * @details Allows to use **Pimpl** pattern in custom types and classes.
 *
 * Example declaration:
 *
 * @code {.cpp}
 *    using namespace whs31;
 *
 *    class TestPimpl
 *    {
 *      public:
 *         TestPimpl() = default;
 *
 *         [[nodiscard]] int add(int a, int b) const;
 *
 *      private:
 *        struct impl;
 *        Pimpl<struct impl> impl_;
 *    };
 *
 *    struct TestPimpl::impl
 *    {
 *        int add(int a, int b) const { return a + b; }
 *    };
 *
 *    int TestPimpl::add(int a, int b) const {
 *      return this->impl_->add(a, b);
 *    }
 * @endcode
 * @tparam T Type to implement.
 * @ingroup core_memory
 * @see propagate_const
 * @see SharedPimpl
 * @see DECLARE_PRIVATE
 * @see DECLARE_PRIVATE_AS
 */
template <typename T>
using Pimpl = propagate_const<std::unique_ptr<T>>;

/**
 * @brief Shared pointer-to-implementation pattern trait.
 * @details Equivalent to `Pimpl`, but stores the implementation in a
 * `std::shared_ptr` when shared ownership is required.
 * @tparam T Implementation type.
 * @ingroup core_memory
 * @see Pimpl
 * @see DECLARE_PRIVATE_SHARED
 * @see DECLARE_PRIVATE_SHARED_AS
 */
template <typename T>
using SharedPimpl = propagate_const<std::shared_ptr<T>>;

WHS31_COMMON_NAMESPACE_END

// NOLINTBEGIN(*-reserved-identifier, *-identifier-naming, *-macro-usage)

/**
 * @ingroup macros
 * @brief Declares a conventional private implementation member.
 * @details Expands to a forward declaration of `classname Private` and a
 * `Pimpl<classname Private>` member named `d`.
 * @param classname Public class name.
 */
#define DECLARE_PRIVATE(classname)                                                            \
  struct classname## Private;                                                                 \
  ::whs31::common::Pimpl<classname## Private> d;

/**
 * @ingroup macros
 * @brief Declares a private implementation member with an explicit type name.
 * @details Expands to a forward declaration of `alias` and a `Pimpl<alias>`
 * member named `d`.
 * @param alias Private implementation type name.
 */
#define DECLARE_PRIVATE_AS(alias)                                                             \
  struct alias;                                                                               \
  ::whs31::common::Pimpl<alias> d;

/**
 * @ingroup macros
 * @brief Declares a shared private implementation member.
 * @details Expands to a forward declaration of `classname Private` and a
 * `SharedPimpl<classname Private>` member named `d`.
 * @param classname Public class name.
 */
#define DECLARE_PRIVATE_SHARED(classname)                                                     \
  struct classname## Private;                                                                 \
  ::whs31::common::SharedPimpl<classname## Private> d;

/**
 * @ingroup macros
 * @brief Declares a shared private implementation member with an explicit type name.
 * @details Expands to a forward declaration of `alias` and a `SharedPimpl<alias>`
 * member named `d`.
 * @param alias Private implementation type name.
 */
#define DECLARE_PRIVATE_SHARED_AS(alias)                                                      \
  struct alias;                                                                               \
  ::whs31::common::SharedPimpl<alias> d;

// NOLINTEND(*-reserved-identifier, *-identifier-naming, *-macro-usage)
