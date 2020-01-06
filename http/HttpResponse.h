#pragma once

#include <map>
#include <string>

class Buffer;
class HttpResponse {
   public:
    enum HttpStatusCode {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };

    explicit HttpResponse(HttpSessionPtr session, bool close) : statusCode_(kUnknown), closeConnection_(close),httpsession_(session) {}

    void setStatusCode(HttpStatusCode code) { statusCode_ = code; }

    void setStatusMessage(const std::string& message) { statusMessage_ = message; }

    void setCloseConnection(bool on) { closeConnection_ = on; }

    bool closeConnection() const { return closeConnection_; }

    void setContentType(const std::string& contentType) { addHeader("Content-Type", contentType); }

    // FIXME: replace std::string with StringPiece
    void addHeader(const std::string& key, const std::string& value) { headers_[key] = value; }

    void setBody(const std::string& body) { body_ = body; }

    void appendToBuffer(BufferPtr output) const;

    HttpSessionPtr getSessionPtr(){
        return httpsession_;
    }

    void setDelay(){
        delay_ = true;
    }

    bool delay(){
        return delay_;
    }

   private:
    std::map<std::string, std::string> headers_;
    HttpStatusCode statusCode_;
    // FIXME: add http version
    std::string statusMessage_;
    bool closeConnection_;
    std::string body_;
    bool delay_{false};
    HttpSessionPtr httpsession_;
};
