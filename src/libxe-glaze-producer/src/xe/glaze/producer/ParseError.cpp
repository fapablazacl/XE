#include "xe/glaze/producer/ParseError.h"

namespace xe::glaze::producer {

ParseError::ParseError(const std::string &message) : std::runtime_error(message) {}

} // namespace xe::glaze::producer
