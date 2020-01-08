/*
 * Copyright 2019, Shengqu Games Inc.
 * All rights reserved.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "spdlog/async.h"

namespace common {


//#define __FILENAME__ __FILE__
class Log {
 public:
  Log();

  static Log& Instance();

  spdlog::async_logger& get_logger();

  void Init(const std::string& log_name, const std::string& log_path,
            const spdlog::level::level_enum& log_level = spdlog::level::trace,
            const spdlog::level::level_enum& flush_level = spdlog::level::info,
            const bool& stdout_switch = false, const uint16_t& thread_num = 1);
  void Init(const std::string& log_name, const std::string& log_path,
            const std::string& log_level_name,
            const std::string& flush_level_name,
            const bool& stdout_switch = false, const uint16_t& thread_num = 1);

 protected:
  void set_logger(const std::shared_ptr<spdlog::async_logger>& logger);

 private:
  std::shared_ptr<spdlog::async_logger> logger_;
};

#define LOG_TRACE(fmt, ...)                   \
  common::Log::Instance().get_logger().trace( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)                   \
  common::Log::Instance().get_logger().debug( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                   \
  common::Log::Instance().get_logger().info( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)                   \
  common::Log::Instance().get_logger().warn( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                   \
  common::Log::Instance().get_logger().error( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...)                   \
  common::Log::Instance().get_logger().critical( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

}  // namespace common
