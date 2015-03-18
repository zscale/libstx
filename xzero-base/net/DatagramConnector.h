// This file is part of the "libxzero" project, http://xzero.io/
//   (c) 2009-2015 Christian Parpart <trapni@gmail.com>
//   (c) 2014-2015 Paul Asmuth
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT
#pragma once

#include <xzero-base/Api.h>
#include <xzero-base/RefPtr.h>
#include <functional>
#include <string>

namespace xzero {

class Executor;
class DatagramEndPoint;

typedef std::function<void(RefPtr<DatagramEndPoint>)> DatagramHandler;

class XZERO_API DatagramConnector {
 public:
  DatagramConnector(
      const std::string& name,
      DatagramHandler handler,
      Executor* executor);
  virtual ~DatagramConnector();

  virtual void start() = 0;
  virtual bool isStarted() const = 0;
  virtual void stop() = 0;

  void setHandler(DatagramHandler handler);

 private:
  std::string name_;
  DatagramHandler handler_;
  Executor* executor_;
};

} // namespace xzero
