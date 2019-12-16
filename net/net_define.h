#pragma once
#include <functional>
#include <memory>
#include "asio.hpp"

class Loop;
class Session;

typedef std::shared_ptr<Session> SessionPtr;
typedef std::shared_ptr<Loop> LoopPtr;
typedef std::function<SessionPtr()> NewSessionCallback;
typedef std::function<void(void)> Callback;