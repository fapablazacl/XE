#pragma once

#include "xe/glaze/model/Feature.h"

#include <string>
#include <vector>

namespace xe::glaze::model {

/**
 * @brief One <extension> element. Mirrors glaze/model.py::Extension.
 * Extended description: a <extension> declares a named GL extension with a
 * list of APIs it supports (e.g. {"gl", "gles2"}) and one or more <require>
 * blocks. Unlike <feature>, extensions never contain <remove> blocks.
 */
struct Extension {
    //! extension name (e.g. "GL_ARB_buffer_storage")
    std::string name;
    //! APIs this extension supports (parsed from the supported="..." attribute)
    std::vector<std::string> supported;
    //! ordered list of <require> blocks
    std::vector<Require> requireList;
};

} // namespace xe::glaze::model
