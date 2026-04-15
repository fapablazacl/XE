#include "xe/glaze/model/TypeDecl.h"

namespace xe::glaze::model {

std::string toCString(const TypeDecl &decl) {
    std::string out;
    if (decl.isConst) {
        out.append("const ");
    }
    out.append(decl.name);
    if (decl.isPointer) {
        out.push_back('*');
    }
    return out;
}

} // namespace xe::glaze::model
