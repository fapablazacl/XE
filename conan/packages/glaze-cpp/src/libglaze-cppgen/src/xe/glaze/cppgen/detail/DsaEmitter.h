#pragma once

#include "xe/glaze/cppgen/detail/FunctorEmitter.h"
#include "xe/glaze/model/ConsolidatedRequire.h"
#include "xe/glaze/model/Registry.h"

#include <nlohmann/json.hpp>

namespace xe::glaze::cppgen::detail {

/**
 * @brief Build the DSA (Direct State Access) class contexts.
 * Extended description: for each discovered handle class, groups every core
 * Named* command whose first parameter is that class into a method on a
 * wrapper class. Mirrors cpp_generator.py::_build_dsa_classes. The method
 * body forwards to the underlying functor, substituting m_id.id for the
 * leading handle parameter and passing the rest through.
 */
nlohmann::json buildDsaClasses(const model::Registry &registry,
                               const model::ConsolidatedRequire &consolidated,
                               const EmitterContext &ctx);

} // namespace xe::glaze::cppgen::detail
