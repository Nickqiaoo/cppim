#include "HttpContext.h"
#include "Buffer.h"

// GET /path?parm HTTP/1.1\r\n
bool HttpContext::parseRequestLine(const char *begin, const char *end) {
    bool success = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space)) {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end) {
            const char *query = std::find(start, space, '?');
            if (query != space) {
                request_.setPath(start, query);
                request_.setQuery(query + 1, space);
            } else {
                request_.setPath(start, query);
            }
            start = space + 1;
            success = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
            if (success) {
                if (*(end - 1) == '1') {
                    request_.setVersion(HttpRequest::kHttp11);
                } else if (*(end - 1) == '0') {
                    request_.setVersion(HttpRequest::kHttp10);
                } else {
                    success = false;
                }
            }
        }
    }
    return success;
}

bool HttpContext::parseRequest(Buffer *buf) {
    bool ok = true;
    bool hasMore = true;
    while (hasMore) {
        if (state_ == kExpectRequestLine) {
            const char *crlf = buf->findCRLF();
            if (crlf) {
                ok = parseRequestLine(buf->peek(), crlf);
                if (ok) {
                    buf->retrieveUntil(crlf + 2);
                    state_ = kExpectHeaders;
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
        } else if (state_ == kExpectHeaders) {
            const char *crlf = buf->findCRLF();
            if (crlf) {
                const char *colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf) {
                    request_.addHeader(buf->peek(), colon, crlf);
                } else {
                    if (buf->peek() == crlf) {
                        state_ = kExpectBody;
                    } else {
                        //状态变成kGotAll还是返回错误？
                        ok = false;
                        hasMore = false;
                    }
                }
                buf->retrieveUntil(crlf + 2);
            } else {
                hasMore = false;
            }
        } else if (state_ == kExpectBody) {
            int contentlength = atoi(request_.getHeader("Content-Length").c_str());
            if (contentlength > 0) {
                if (buf->beginWrite() - buf->peek() >= contentlength) {
                    state_ = kGotAll;
                    request_.setBody(buf->peek(), buf->peek() + contentlength);
                } else {
                    hasMore = false;
                }
            } else {
                state_ = kGotAll;
                hasMore = false;
            }
        }
    }
    return ok;
}