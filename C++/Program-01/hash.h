#ifndef KVDB_HASH_
#define KVDB_HASH_

#include <string>
#include <unordered_map>

namespace kvdb {

static std::unordered_map<std::string, std::string> _map;

class Hash {
 private:
  /* data */
 public:
  Hash(/* args */);
  ~Hash();
};

}  // namespace kvdb

#endif  // KVDB_HASH_