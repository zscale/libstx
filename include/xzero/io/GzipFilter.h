#include <xzero/Api.h>
#include <xzero/io/Filter.h>
#include <xzero/sysconfig.h>
#include <string>

#include <zlib.h>

namespace xzero {

/**
 * Gzip encoding filter.
 */
class XZERO_API GzipFilter : public Filter {
 public:
  explicit GzipFilter(int level);
  ~GzipFilter();

  void filter(const BufferRef& input, Buffer* output, bool last) override;

 private:
  std::string z_code(int code) const;

 private:
  z_stream z_;
};

} // namespace xzero
