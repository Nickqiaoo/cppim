#pragma once

#include "HttpResponse.h"

class Buffer;

class HttpResponseContext {
  public:
    enum HttpResponseParseState {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpResponseContext() : state_(kExpectRequestLine) {}

    // default copy-ctor, dtor and assignment are fine

    // return false if any error
    bool parseRequest(Buffer* buf);

    bool gotAll() const { return state_ == kGotAll; }

    void reset() {
        state_ = kExpectRequestLine;
        HttpResponse dummy;
        response_.swap(dummy);
    }

    const HttpResponse &response() const { return response_; }

    HttpResponse &response() { return response_; }

  private:
    bool parseRequestLine(const char *begin, const char *end);

    HttpResponseParseState state_;
    HttpResponse response_;
};