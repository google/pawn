// Copyright 2014-2017 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Minimal, header-only implementation of commonly used Google3 utilities.

#ifndef SECURITY_ZYNAMICS_PAWN_MINI_GOOGLE_H_
#define SECURITY_ZYNAMICS_PAWN_MINI_GOOGLE_H_

#ifndef GOOGLE
#include <cstdint>
#include <iostream>
#include <string>

typedef std::int8_t int8;
typedef std::uint8_t uint8;
typedef std::int16_t int16;
typedef std::uint16_t uint16;
typedef std::int32_t int32;
typedef std::uint32_t uint32;
typedef std::int64_t int64;
typedef std::uint64_t uint64;

using std::string;

namespace zynamics {

enum LogSeverity {
  // Similar to //base/logging.h.
  kLogSeverityInfo,
  kLogSeverityWarning,
  kLogSeverityError,
  kLogSeverityFatal,
};

struct LogMessage {
  LogMessage(LogSeverity sev, const char* file, int line) {
    static constexpr char kSeverityLetter[] = "IWEF";
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

#define LOG(severity) ZYNAMICS_LOG_##severity
#define ZYNAMICS_LOG_INFO ::zynamics::LogMessage( \
    ::zynamics::kLogSeverityInfo, __FILE__, __LINE__).get()
#define ZYNAMICS_LOG_WARNING ::zynamics::LogMessage( \
    ::zynamics::kLogSeverityWarning, __FILE__, __LINE__).get()
#define ZYNAMICS_LOG_ERROR ::zynamics::LogMessage( \
    ::zynamics::kLogSeverityError, __FILE__, __LINE__).get()
#define ZYNAMICS_LOG_FATAL ::zynamics::LogFatal(__FILE__, __LINE__).get()

#define CHECK(cond) \
  if (!(cond)) { LOG(FATAL) << "Check failed: " #cond ""; }
#define QCHECK(cond) CHECK(cond)

template <typename T>
T&& CheckNotNull(const char* file, int line, const char* message, T&& t) {
  if (t == nullptr) {
    zynamics::LogFatal(file, line).get() << message;
  }
  return std::forward<T>(t);
}

#define CHECK_NOTNULL(val) ::zynamics::CheckNotNull(__FILE__, __LINE__, \
    "'" #val "' Must be non NULL", (val))

}  // namespace zynamics

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

namespace util {

class Status {
 public:
  class OK;

  Status() = default;
  Status(int /* code */, const char* message) : message_(message) {}

  bool ok() { return message_.empty(); }

  const std::string& error_message() { return message_; }

 private:
  std::string message_;
};

class Status::OK {};

namespace error {

enum {
  OK = 0,
  CANCELLED = 1,
  UNKNOWN = 2,
  INVALID_ARGUMENT = 3,
  DEADLINE_EXCEEDED = 4,
  NOT_FOUND = 5,
  ALREADY_EXISTS = 6,
  PERMISSION_DENIED = 7,
  UNAUTHENTICATED = 16,
  RESOURCE_EXHAUSTED = 8,
  FAILED_PRECONDITION = 9,
  ABORTED = 10,
  OUT_OF_RANGE = 11,
  UNIMPLEMENTED = 12,
  INTERNAL = 13,
  UNAVAILABLE = 14,
  DATA_LOSS = 15
};

}  // namespace error
}  // namespace util

#define CHECK_OK(status) \
  if (!(status).ok()) { LOG(FATAL) << status.error_message(); }
#define QCHECK_OK(status) CHECK_OK(status)

#endif  // GOOGLE
#endif  // SECURITY_ZYNAMICS_PAWN_MINI_GOOGLE_H_
