#include "xe/glaze/model/EnumGroup.h"

namespace xe::glaze::model {

const Enum *findEnum(const EnumGroup &group, std::string_view name) noexcept {
    for (const auto &entry : group.enums) {
        if (entry.first == name) {
            return &entry.second;
        }
    }
    return nullptr;
}

} // namespace xe::glaze::model
