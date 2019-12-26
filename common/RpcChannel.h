#pragma once

#include "RpcCodec.h"

#include <google/protobuf/service.h>

#include <map>

// Service and RpcChannel classes are incorporated from
// google/protobuf/service.h

// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

namespace google {
namespace protobuf {

// Defined in other files.
class Descriptor;            // descriptor.h
class ServiceDescriptor;     // descriptor.h
class MethodDescriptor;      // descriptor.h
class Message;               // message.h

class Closure;

class RpcController;
class Service;

}  // namespace protobuf
}  // namespace google


typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

// Abstract interface for an RPC channel.  An RpcChannel represents a
// communication line to a Service which can be used to call that Service's
// methods.  The Service may be running on another machine.  Normally, you
// should not call an RpcChannel directly, but instead construct a stub Service
// wrapping it.  Example:
// FIXME: update here
//   RpcChannel* channel = new MyRpcChannel("remotehost.example.com:1234");
//   MyService* service = new MyService::Stub(channel);
//   service->MyMethod(request, &response, callback);
class RpcChannel : public ::google::protobuf::RpcChannel
{
  enum ErrorCode {
        NO_ERROR = 0,
        WRONG_PROTO,
        INVALID_REQUEST,
        NO_METHOD,
        NO_SERVICE,
        PARSE_ERROR,
    };
 public:
  RpcChannel();

  explicit RpcChannel(const SessionPtr& conn);

  ~RpcChannel() override;

  void setConnection(const SessionPtr& conn)
  {
    conn_ = conn;
  }

  void setServices(const std::map<std::string, ::google::protobuf::Service*>* services)
  {
    services_ = services;
  }

  // Call the given method of the remote service.  The signature of this
  // procedure looks the same as Service::CallMethod(), but the requirements
  // are less strict in one important way:  the request and response objects
  // need not be of any specific class as long as their descriptors are
  // method->input_type() and method->output_type().
  void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                  ::google::protobuf::RpcController* controller,
                  const ::google::protobuf::Message* request,
                  ::google::protobuf::Message* response,
                  ::google::protobuf::Closure* done) override;

  void onMessage(const SessionPtr& conn,
                 BufferPtr buf);

 private:
  void onRpcMessage(bool isresponse, uint64_t id, const std::string& servicename, const std::string& methodname, const SessionPtr& conn,
                    const char* data, int32_t datalen);

  void doneCallback(::google::protobuf::Message* response, string name);

  struct OutstandingCall
  {
    ::google::protobuf::Message* response;
    ::google::protobuf::Closure* done;
  };

  common::RpcCodec codec_;
  SessionPtr conn_;
  std::atomic<int64_t> id_;

  std::mutex mutex_;
  std::map<uint64_t, OutstandingCall> outstandings_;

  const std::map<std::string, ::google::protobuf::Service*>* services_;
};
typedef std::shared_ptr<RpcChannel> RpcChannelPtr;
