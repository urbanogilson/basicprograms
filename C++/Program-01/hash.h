#ifndef KVDB_HASH_
#define KVDB_HASH_

#include <glog/logging.h>

#include <cassert>
#include <functional>
#include <string>
#include <unordered_map>

namespace kvdb {
static std::unordered_map<std::string, std::string> _map;  // ! To be removed

struct Node {
  struct Node *next = nullptr;
  uint64_t code = 0;
};

struct Tab {
  struct Node **tab = nullptr;
  size_t mask = 0;
  size_t size = 0;
};

class Hash {
 private:
  struct Tab _htab {};

 public:
  Hash(size_t size);
  ~Hash();
  struct Node **Tab(void) const;
  size_t Size(void) const;
  size_t Mask(void) const;
  void Insert(struct Node *node);
  struct Node **Lookup(struct Node *key,
                       bool (*cmp)(struct Node *, struct Node *));
  struct Node *Detach(struct Node **from);
  void Traverse(std::function<void(struct Node *)> func);
};

class Map {
 private:
  const size_t _k_resizing_work = 128;
  const size_t _k_max_load_factor = 8;
  Hash _ht1;
  Hash _ht2;
  size_t _resizing_pos = 0;
  void HelpResizing(void) {
    if (_ht2.Tab() == nullptr) return;

    size_t nwork = 0;

    while (nwork < _k_resizing_work && _ht2.Size() > 0) {
      struct Node **from = &_ht2.Tab()[_resizing_pos];
      if (!*from) {
        _resizing_pos++;
        continue;
      }

      _ht1.Insert(_ht2.Detach(from));
      nwork++;
    }

    if (_ht2.Size() == 0) {
      _ht2 = Hash(1);
    }
  }

  void StartResizing(void) {
    CHECK_EQ(_ht2.Tab(), nullptr);
    _ht2 = _ht1;
    _ht1 = Hash((_ht1.Mask() + 1) * 2);
    _resizing_pos = 0;
  }

 public:
  struct Node **Lookup(struct Map *map, struct Node *key,
                       bool (*cmp)(struct Node *, struct Node *));
  void Insert(struct Map *map, struct Node *node);
  struct Node *Pop(struct Map *map, struct Node *key,
                   bool (*cmp)(struct Node *, struct Node *));
  struct Node *Destroy(struct Map *map);
};

}  // namespace kvdb

#endif  // KVDB_HASH_