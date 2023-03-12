#include "hash.h"

namespace kvdb {
Hash::Hash(size_t size) {
  CHECK((size > 0) && ((size & (size - 1)) == 0))
      << "size must be a positive and power of two";
  _htab.tab = new struct Node *[size]();
  _htab.mask = size - 1;
}

Hash::~Hash() { delete[] _htab.tab; }

struct Node **Hash::Tab(void) const { return _htab.tab; };

size_t Hash::Size(void) const { return _htab.size; };

size_t Hash::Mask(void) const { return _htab.mask; };

void Hash::Insert(struct Node *node) {
  size_t position = node->code & _htab.mask;
  struct Node *next = _htab.tab[position];
  node->next = next;
  _htab.tab[position] = node;
  _htab.size++;
}

void Hash::Traverse(std::function<void(struct Node *)> func) {
  if (!_htab.tab) return;

  for (size_t size = 0; size < _htab.size; size++) func(_htab.tab[size]);
}

struct Node **Hash::Lookup(struct Node *key,
                           bool (*cmp)(struct Node *, struct Node *)) {
  if (!_htab.tab) return nullptr;

  size_t position = key->code & _htab.mask;
  struct Node **from = &_htab.tab[position];

  while (*from) {
    if (cmp(*from, key)) return from;

    from = &(*from)->next;
  }

  return nullptr;
}

struct Node *Hash::Detach(struct Node **from) {
  struct Node *node = *from;
  *from = (*from)->next;
  _htab.size--;
  return node;
}

struct Node **Map::Lookup(struct Map *map, struct Node *key,
                          bool (*cmp)(struct Node *, struct Node *)) {
  HelpResizing();
  struct Node **from = _ht1.Lookup(key, cmp);
  if (!from) {
    from = _ht2.Lookup(key, cmp);
  }

  return from;
}
void Map::Insert(struct Map *map, struct Node *node) {
  if (!_ht1.Tab()) {
    _ht1 = Hash(4);
  }
  _ht1.Insert(node);

  if (!_ht2.Tab()) {
    // check whether we need to resize
    size_t load_factor = _ht1.Size() / (_ht1.Mask() + 1);
    if (load_factor >= _k_max_load_factor) {
      StartResizing();
    }
  }
  HelpResizing();
}
struct Node *Map::Pop(struct Map *map, struct Node *key,
                      bool (*cmp)(struct Node *, struct Node *)) {
  HelpResizing();

  struct Node **from = _ht1.Lookup(key, cmp);
  if (from) {
    return _ht1.Detach(from);
  }

  from = _ht2.Lookup(key, cmp);
  if (from) {
    return _ht2.Detach(from);
  }

  return nullptr;
}
}  // namespace kvdb
