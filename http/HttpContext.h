#pragma once

#include "HttpRequest.h"

class Buffer;

class HttpContext {
  public:
    enum HttpRequestParseState {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext() : state_(kExpectRequestLine) {}

    // default copy-ctor, dtor and assignment are fine

    // return false if any error
    bool parseRequest(BufferPtr buf);

    bool gotAll() const { return state_ == kGotAll; }

    void reset() {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest &request() const { return request_; }

    HttpRequest &request() { return request_; }

  private:
    bool parseRequestLine(const char *begin, const char *end);

    HttpRequestParseState state_;
    HttpRequest request_;
};
