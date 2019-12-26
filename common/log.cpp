/*
 * Copyright 2019, Shengqu Games Inc.
 * All rights reserved.
 */

#include "log.h"

//#include "hourly_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

namespace common {

Log::Log() : logger_(nullptr) {}

Log& Log::Instance() {
  static Log instance;
  return instance;
}

spdlog::async_logger& Log::get_logger() {
  assert(this->logger_);
  return *this->logger_;
}

void Log::Init(const std::string& log_name, const std::string& log_path,
               const spdlog::level::level_enum& log_level,
               const spdlog::level::level_enum& flush_level,
               const bool& stdout_switch, const uint16_t& thread_num) {
  spdlog::init_thread_pool(8192, thread_num);
  std::vector<spdlog::sink_ptr> sinks;
  if (stdout_switch) {
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    stdout_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%L%$] %v");
    sinks.push_back(stdout_sink);
  }
/*
  auto hourly_file_sink =
      std::make_shared<spdlog::sinks::hourly_file_sink_mt>(log_path);
  hourly_file_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%L%$] %v");
  */
  auto daily_file_sink =
      std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_path, 1, 0);
  daily_file_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%L%$] %v");
  sinks.push_back(daily_file_sink);

  auto logger = std::make_shared<spdlog::async_logger>(
      log_name, begin(sinks), end(sinks), spdlog::thread_pool());

  logger->set_level(log_level);
  logger->flush_on(flush_level);

  this->set_logger(logger);
}

void Log::Init(const std::string& log_name, const std::string& log_path,
               const std::string& log_level_name,
               const std::string& flush_level_name, const bool& stdout_switch,
               const uint16_t& thread_num) {
  spdlog::level::level_enum log_level = spdlog::level::from_str(log_level_name);
  spdlog::level::level_enum flush_level =
      spdlog::level::from_str(flush_level_name);
  this->Init(log_name, log_path, log_level, flush_level, stdout_switch,
             thread_num);
}

void Log::set_logger(const std::shared_ptr<spdlog::async_logger>& logger) {
  this->logger_ = logger;
}

}  // namespace common
