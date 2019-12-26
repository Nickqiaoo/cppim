#pragma once

#include <functional>
#include <memory>
#include <asio.hpp>

class Loop;
class Session;
class Buffer;

typedef std::shared_ptr<Session> SessionPtr;
typedef std::shared_ptr<Buffer> BufferPtr;
typedef std::shared_ptr<Loop> LoopPtr;
typedef std::function<SessionPtr()> NewSessionCallback;
typedef std::function<void(const SessionPtr&)> onConnectionCallback;
typedef std::function<void(void)> Callback;
typedef std::function<void(SessionPtr, BufferPtr)> onMessageCallback;