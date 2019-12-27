#pragma once

#include <functional>
#include <memory>
#include <asio.hpp>

class Loop;
class Session;
class Buffer;
class RpcChannel;

typedef std::shared_ptr<Session> SessionPtr;
typedef std::shared_ptr<Buffer> BufferPtr;
typedef std::shared_ptr<Loop> LoopPtr;
typedef std::shared_ptr<RpcChannel> RpcChannelPtr;
typedef std::function<SessionPtr()> NewSessionCallback;
typedef std::function<void(const SessionPtr&)> onConnectionCallback;
typedef std::function<void(void)> Callback;
typedef std::function<void(SessionPtr, BufferPtr)> onMessageCallback;