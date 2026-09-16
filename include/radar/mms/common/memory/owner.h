#pragma once

#include <radar/mms/common/concepts.h>
#include <radar/mms/common/detail/namespace.h>

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief GSL-like owning raw pointer typedef.
 * @details `Owner<T>` is designed as a safety mechanism for code that must deal
 * directly with raw pointers that own memory.
 *
 * Ideally such code should be restricted to the implementation of low-level abstractions.
 * `Owner<T>` can also be used as a stepping point in converting legacy code
 * to use more modern RAII constructs, such as smart pointers.
 * @tparam T Pointer type.
 * @ingroup core_memory
 * @see observer_ptr
 * @sa https://github.com/microsoft/GSL/blob/main/include/gsl/pointers
 */
template <Pointer T>
using Owner = T;

RADAR_COMMON_NAMESPACE_END
