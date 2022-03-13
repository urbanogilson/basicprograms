#ifndef KVDB_KVDB_
#define KVDB_KVDB_

#include <string>

namespace kvdb {
class Kvdb {
 public:
  Kvdb(const std::string &name) : name_(name) {}
  ~Kvdb() {}

 private:
  std::string name_;
};
}  // namespace kvdb

#endif  // KVDB_KVDB_