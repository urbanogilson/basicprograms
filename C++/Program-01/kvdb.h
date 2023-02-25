#ifndef KVDB_KVDB_
#define KVDB_KVDB_

#include <string>

namespace kvdb {
class Kvdb {
 public:
  Kvdb(const std::string &name) : name_(name) {}
  ~Kvdb() {}
  std::string GetName(void) { return name_; }

 private:
  std::string name_;
};
}  // namespace kvdb

#endif  // KVDB_KVDB_