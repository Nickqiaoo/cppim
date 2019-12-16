#include "Session.h"

Session::Session(asio::io_service& ios) : socket_(ios) {}