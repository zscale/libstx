/**
 * This file is part of the "libfnord" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord-base/util/binarymessagewriter.h"
#include "fnord-json/json.h"
#include <fnord-base/wallclock.h>
#include "fnord-msg/MessageEncoder.h"
#include "fnord-msg/MessagePrinter.h"
#include <fnord-base/util/Base64.h>
#include "fnord-feeds/BrokerServlet.h"

namespace fnord {
namespace feeds {

BrokerServlet::BrokerServlet(FeedService* service) : service_(service) {}

void BrokerServlet::handleHTTPRequest(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res) {
  URI uri(req->uri());

  res->addHeader("Access-Control-Allow-Origin", "*");

  try {
    if (StringUtil::endsWith(uri.path(), "/insert")) {
      return insertRecord(req, res, &uri);
    }

    res->setStatus(fnord::http::kStatusNotFound);
    res->addBody("not found");
  } catch (const Exception& e) {
    res->setStatus(http::kStatusInternalServerError);
    res->addBody(StringUtil::format("error: $0: $1", e.getTypeName(), e.getMessage()));
  }
}

void BrokerServlet::insertRecord(
    http::HTTPRequest* req,
    http::HTTPResponse* res,
    URI* uri) {
  const auto& params = uri->queryParams();

  String topic;
  if (!URI::getParam(params, "topic", &topic)) {
    res->setStatus(fnord::http::kStatusBadRequest);
    res->addBody("error: missing ?topic=... parameter");
    return;
  }

  if (req->body().size() == 0) {
    res->setStatus(fnord::http::kStatusBadRequest);
    res->addBody("error: empty record (body_size == 0)");
  }

  auto offset = service_->insert(topic, req->body());
  res->addHeader("X-Broker-Created-Offset", StringUtil::toString(offset));
  res->setStatus(http::kStatusCreated);
}

}
}

