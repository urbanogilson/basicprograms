#include "hash.h"

namespace kvdb {
Hash::Hash(ssize_t size) : _mask(size - 1), _size(0) {
  CHECK((size > 0) && ((size & (size - 1)) == 0))
      << "size must be a positive and power of two";

  _chain = std::vector<std::forward_list<struct Node>>(size);
}

Hash::~Hash() {}

void Hash::Insert(Node &node) {
  _chain[_Position(node)].push_front(node);
  _size++;
}

void Hash::Insert(const std::string &key, const std::string &value) {
  auto node = Node{.key = key, .value = value};
  _chain[_Position(node)].push_front(node);
  _size++;
}

std::optional<std::forward_list<Node>::iterator> Hash::Find(const Node &node) {
  auto chain = _chain[_Position(node)];
  auto result = std::find(std::begin(chain), std::end(chain), node);
  if (result != std::end(chain)) return result;
  return std::nullopt;
}

std::optional<std::forward_list<Node>::iterator> Hash::Find(
    const std::string &key) {
  return Find(Node{.key = key});
}

std::optional<std::string> Hash::Get(const std::string &key) {
  auto result = Find(key);

  if (result.has_value()) return result.value()->value;

  return std::nullopt;
}

int Hash::Delete(const std::string &key) {
  auto chain = _chain[_Position(key)];
  auto n = chain.remove(Node{.key = key});
  _size -= n;
  return n;
}

}  // namespace kvdb
