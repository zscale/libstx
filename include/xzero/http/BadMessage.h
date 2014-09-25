#include <xzero/Api.h>
#include <stdexcept>

namespace xzero {

/**
 * Helper exception that is thrown on semantic message errors by HttpChannel.
 */
class XZERO_API BadMessage : public std::runtime_error {
 public:
  explicit BadMessage(HttpStatus code)
      : BadMessage(code, to_string(code)) {}

  explicit BadMessage(HttpStatus code, const std::string& reason)
      : std::runtime_error(reason),
        code_(code) {}

  HttpStatus code() const noexcept { return code_; }

 private:
  HttpStatus code_;
};

} // namespace xzero
