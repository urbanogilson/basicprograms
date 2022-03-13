#ifndef KVDB_STATUS_
#define KVDB_STATUS_

#include <array>
#include <string>

namespace kvdb {

enum class Code {
  kOK = 0,
  kNotFound = 1,
  kRemoveOrder = 2,
  kInvalidArgument = 3,
  kIOError = 4
};

class Status {
 public:
  Status() : code_(Code::kOK), message_("") {}
  Status(Code code, const std::string& message)
      : code_(Code::kOK), message_(message) {}
  ~Status() {}

  static Status Ok() { return Status(); };
  static Status IOError(const std::string& message) {
    return Status(Code::kIOError, message);
  }

 private:
  Code code_;
  std::string message_;
};
}  // namespace kvdb

#endif  // KVDB_STATUS_