// Copyright 2014-2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Minimal, header-only implementation of commonly used Google3 utilities.

#ifndef PAWN_MINI_GOOGLE_H_
#define PAWN_MINI_GOOGLE_H_

#ifndef GOOGLE
#include <cstdint>
#include <iostream>
#include <string>

namespace security::pawn {

enum LogSeverity {
  // Similar to //base/logging.h.
  kLogSeverityInfo,
  kLogSeverityWarning,
  kLogSeverityError,
  kLogSeverityFatal,
};

struct LogMessage {
  LogMessage(LogSeverity sev, const char* file, int line) {
    constexpr char kSeverityLetter[] = "IWEF";
    get() << kSeverityLetter[sev] << " " << file << ":" << line << "] ";
  }

  std::ostream& get() { return std::cerr; }

  ~LogMessage() { std::cerr << std::endl; }
};

struct LogFatal : public LogMessage {
  LogFatal(const char* file, int line)
    : LogMessage(kLogSeverityFatal, file, line) {}

  ~LogFatal() { abort(); }
};

#define LOG(severity) PAWN_LOG_##severity
#define PAWN_LOG_INFO                                                        \
  ::security::pawn::LogMessage(::security::pawn::kLogSeverityInfo, __FILE__, \
                               __LINE__)                                     \
      .get()
#define PAWN_LOG_WARNING                                              \
  ::security::pawn::LogMessage(::security::pawn::kLogSeverityWarning, \
                               __FILE__, __LINE__)                    \
      .get()
#define PAWN_LOG_ERROR                                                        \
  ::security::pawn::LogMessage(::security::pawn::kLogSeverityError, __FILE__, \
                               __LINE__)                                      \
      .get()
#define PAWN_LOG_FATAL ::security::pawn::LogFatal(__FILE__, __LINE__).get()

#define CHECK(cond)                          \
  if (!(cond)) {                             \
    LOG(FATAL) << "Check failed: " #cond ""; \
  }
#define QCHECK(cond) CHECK(cond)

template <typename T>
T&& CheckNotNull(const char* file, int line, const char* message, T&& t) {
  if (t == nullptr) {
    ::security::pawn::LogFatal(file, line).get() << message;
  }
  return std::forward<T>(t);
}

#define CHECK_NOTNULL(val)                           \
  ::security::pawn::CheckNotNull(__FILE__, __LINE__, \
                                 "'" #val "' Must be non NULL", (val))

}  // namespace security::pawn

#define CHECK_OK(status)                                          \
  {                                                               \
    if (const auto _pawn_status = (status); !_pawn_status.ok()) { \
      LOG(FATAL) << _pawn_status.message();                       \
    }                                                             \
  }
#define QCHECK_OK(status) CHECK_OK(status)

#endif  // GOOGLE
#endif  // PAWN_MINI_GOOGLE_H_
