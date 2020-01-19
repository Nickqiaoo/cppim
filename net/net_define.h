#pragma once

#include <functional>
#include <memory>
#include <asio.hpp>

class Loop;
class Session;
class RpcSession;
class HttpSession;
class Buffer;
class RpcChannel;
class HttpResponse;

typedef std::shared_ptr<Session> SessionPtr;
typedef std::shared_ptr<RpcSession> RpcSessionPtr;
typedef std::shared_ptr<HttpSession> HttpSessionPtr;
typedef std::shared_ptr<HttpResponse> HttpResponsePtr;
typedef std::shared_ptr<Buffer> BufferPtr;
typedef std::shared_ptr<Loop> LoopPtr;
typedef std::shared_ptr<RpcChannel> RpcChannelPtr;
typedef std::function<SessionPtr()> NewSessionCallback;
typedef std::function<void(const SessionPtr)> onConnectionCallback;
typedef std::function<void(void)> Callback;
typedef std::function<void(const SessionPtr, Buffer*)> onMessageCallback;
typedef std::function<void(const SessionPtr)> onUserDisconnectCallback;
typedef std::function<void(int)> onDisconnectCallback;