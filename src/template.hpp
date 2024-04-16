#pragma once

#include <string_view>

namespace pgen {

auto validate_template(std::string_view tmpl) -> bool;

}