#pragma once

#include <xzero/Api.h>
#include <string>

namespace xzero {

/**
 * Callback API for HTTP message body availability.
 */
class XZERO_API HttpInputListener {
 public:
  virtual ~HttpInputListener() {}

  /**
   * Invoked on some content data being available.
   */
  virtual void onContentAvailable() = 0;

  /**
   * Invoked when all data has been fully consumed.
   */
  virtual void onAllDataRead() = 0;

  /**
   * Invoked when an input read error occured.
   *
   * @param errorMessage a describing error message for logging.
   *
   * No other callbacks will be invoked once this one got called.
   */
  virtual void onError(const std::string& errorMessage) = 0;
};

} // namespace xzero
