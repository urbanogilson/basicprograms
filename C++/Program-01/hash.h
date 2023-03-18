#ifndef KVDB_HASH_
#define KVDB_HASH_

#include <glog/logging.h>

#include <algorithm>
#include <cassert>
#include <forward_list>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace kvdb {
static std::unordered_map<std::string, std::string> _map;  // ! To be removed

struct Node {
  std::string key;
  std::string value;
  bool operator==(const Node &n) const { return key == n.key; }
};

class Hash {
 private:
  std::vector<std::forward_list<Node>> _chain;
  std::hash<std::string> _hasher;
  size_t _mask;
  size_t _size;
  size_t _Position(const std::string &key) { return _hasher(key) & _mask; }
  size_t _Position(const Node &node) { return _Position(node.key); }

 public:
  Hash(ssize_t size);
  ~Hash();
  void Insert(Node &node);
  void Insert(const std::string &key, const std::string &value);
  std::optional<std::forward_list<Node>::iterator> Find(const Node &node);
  std::optional<std::forward_list<Node>::iterator> Find(const std::string &key);
  std::optional<std::string> Get(const std::string &key);
  int Delete(const std::string &key);
  size_t Size(void) const { return _size; };
};

}  // namespace kvdb

#endif  // KVDB_HASH_