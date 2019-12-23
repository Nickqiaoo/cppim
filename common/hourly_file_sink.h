/*
 * Copyright 2019, Shengqu Games Inc.
 * All rights reserved.
 */

#pragma once

#include <chrono>  // NOLINT
#include <cstdio>
#include <ctime>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <utility>

#include "spdlog/details/file_helper.h"
#include "spdlog/details/null_mutex.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/spdlog.h"

namespace spdlog {
namespace sinks {

struct hourly_filename_calculator {
  // Create filename for the form basename.YYYY-MM-DD
  static filename_t calc_filename(const filename_t &filename,
                                  const tm &now_tm) {
    filename_t basename, ext;
    std::tie(basename, ext) =
        details::file_helper::split_by_extension(filename);
    std::conditional<std::is_same<filename_t::value_type, char>::value,
                     fmt::memory_buffer, fmt::wmemory_buffer>::type w;
    fmt::format_to(w, SPDLOG_FILENAME_T("{}_{:04d}-{:02d}-{:02d}_{:02d}{}"),
                   basename, now_tm.tm_year + 1900, now_tm.tm_mon + 1,
                   now_tm.tm_mday, now_tm.tm_hour, ext);
    return fmt::to_string(w);
  }
};

template <typename Mutex, typename FileNameCalc = hourly_filename_calculator>
class hourly_file_sink final : public base_sink<Mutex> {
 public:
  hourly_file_sink(filename_t base_filename, int rotation_interval = 1,
                   bool truncate = false)
      : base_filename_(std::move(base_filename)),
        rotation_interval_(rotation_interval),
        truncate_(truncate) {
    if (rotation_interval < 1 || rotation_interval > 24) {
      throw spdlog_ex("hourly_file_sink: Invalid rotation time in ctor");
    }
    auto now = log_clock::now();
    file_helper_.open(FileNameCalc::calc_filename(base_filename_, now_tm(now)),
                      truncate_);
    rotation_tp_ = next_rotation_tp_();
  }

 protected:
  void sink_it_(const details::log_msg &msg) override {
    if (msg.time >= rotation_tp_) {
      file_helper_.open(
          FileNameCalc::calc_filename(base_filename_, now_tm(msg.time)),
          truncate_);
      rotation_tp_ = next_rotation_tp_();
    }
    fmt::memory_buffer formatted;
    sink::formatter_->format(msg, formatted);
    file_helper_.write(formatted);
  }

  void flush_() override { file_helper_.flush(); }

 private:
  tm now_tm(log_clock::time_point tp) {
    time_t tnow = log_clock::to_time_t(tp);
    return spdlog::details::os::localtime(tnow);
  }

  log_clock::time_point next_rotation_tp_() {
    auto now = log_clock::now();
    tm date = now_tm(now);
    date.tm_min = 0;
    date.tm_sec = 0;
    auto rotation_time = log_clock::from_time_t(std::mktime(&date));
    if (rotation_time > now) {
      return rotation_time;
    }
    return {rotation_time + std::chrono::hours(rotation_interval_)};
  }

  filename_t base_filename_;
  int rotation_interval_;
  log_clock::time_point rotation_tp_;
  details::file_helper file_helper_;
  bool truncate_;
};

using hourly_file_sink_mt = hourly_file_sink<std::mutex>;
using hourly_file_sink_st = hourly_file_sink<details::null_mutex>;

template <typename Factory = default_factory>
inline std::shared_ptr<logger> hourly_logger_mt(const std::string &logger_name,
                                                const filename_t &filename,
                                                int interval = 1,
                                                bool truncate = false) {
  return Factory::template create<sinks::hourly_file_sink_mt>(
      logger_name, filename, interval, truncate);
}

template <typename Factory = default_factory>
inline std::shared_ptr<logger> hourly_logger_st(const std::string &logger_name,
                                                const filename_t &filename,
                                                int interval = 1,
                                                bool truncate = false) {
  return Factory::template create<sinks::hourly_file_sink_st>(
      logger_name, filename, interval, truncate);
}

}  // namespace sinks
}  // namespace spdlog
