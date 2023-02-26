#ifndef KVDB_KVDB_
#define KVDB_KVDB_

#include <string>

namespace kvdb {
class Kvdb {
 public:
  Kvdb(const std::string &name) noexcept : name_(name) {}
  ~Kvdb() noexcept {}

 private:
  std::string name_;
};
}  // namespace kvdb

#endif  // KVDB_KVDB_