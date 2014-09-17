#pragma once

#include <xzero/Api.h>
#include <functional>

namespace xzero {

typedef std::function<void(bool /*succeeded*/)> CompletionHandler;

} // namespace xzero
