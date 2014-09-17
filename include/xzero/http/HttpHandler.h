#pragma once

#include <functional>

namespace xzero {

class HttpRequest;
class HttpResponse;

typedef std::function<void(HttpRequest*, HttpResponse*)> HttpHandler;

}  // namespace xzero
