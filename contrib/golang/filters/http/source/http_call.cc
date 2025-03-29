#include "contrib/golang/filters/http/source/golang_filter.h"
#include "contrib/golang/filters/http/source/http_call.h"


namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Golang {

void HttpCallback::onSuccess(const Http::AsyncClient::Request&,
                             Envoy::Http::ResponseMessagePtr&& response) {
  auto filter = weak_filter_.lock();
  if (filter && !filter->hasDestroyed()) {
    this->response_ = std::move(response);
        

    filter->http_calls_.erase(call_id_);
    filter->dynamic_lib_->envoyGoRequestSemaDec(filter->req_);
  } else {
    ENVOY_LOG(info, "golang filter has gone or destroyed in async httpCall onSuccess callback");
  }
};
void HttpCallback::onFailure(const Http::AsyncClient::Request&,
                             Http::AsyncClient::FailureReason reason) {
  std::string reasonStr;
  switch (reason) {
  case Http::AsyncClient::FailureReason::Reset:
    reasonStr = "The stream has been reset.";
    break;
  case Http::AsyncClient::FailureReason::ExceedResponseBufferLimit:
    reasonStr = "The stream exceeds the response buffer limit.";
    break;
  default:
    reasonStr = "Unknown error.";
  }
  ENVOY_LOG(debug, "http request {} failed. {}", call_id_, reasonStr);
  auto filter = weak_filter_.lock();
  if (filter && !filter->hasDestroyed()) {
    filter->dynamic_lib_->envoyGoRequestSemaDec(filter->req_);
  } else {
    ENVOY_LOG(info, "golang filter has gone or destroyed in async httpCall onFailure callback");
  }
};

void HttpCallback::onBeforeFinalizeUpstreamSpan(
    Envoy::Tracing::Span& /* span */, const Http::ResponseHeaderMap* /* response_headers */) {}

} // namespace Golang
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
