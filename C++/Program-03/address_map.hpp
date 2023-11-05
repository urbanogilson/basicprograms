#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace address {
class Pairing {
 public:
  virtual ~Pairing(void) = default;
  virtual uint32_t encode(uint32_t x, uint32_t y) noexcept = 0;
};

class Cantor : public Pairing {
 public:
  uint32_t encode(uint32_t x, uint32_t y) noexcept override {
    return (x + y) * (x + y + 1) / 2 + y;
  }
};

class Szudzik : public Pairing {
 public:
  uint32_t encode(uint32_t x, uint32_t y) noexcept override {
    return x >= y ? x * x + x + y : x + y * y;
  }
  ~Szudzik() {}
};

class RosenbergStrong : public Pairing {
 public:
  uint32_t encode(uint32_t x, uint32_t y) noexcept override {
    uint32_t a = x >= y ? x * x + x + y : x + y * y;
    return x > y ? a * a + x : a * a + x + y;
  }
};

class MortonEncoding : public Pairing {
 public:
  uint32_t encode(uint32_t x, uint32_t y) noexcept override {
    uint32_t answer = 0;
    for (uint32_t i = 0; i < sizeof(uint32_t) * 8; ++i) {
      answer |= ((x & 1U << i) << i) | ((y & 1U << i) << (i + 1));
    }
    return answer;
  }
};

class AddressMap {
 private:
  std::unordered_map<uint32_t, void*> m_HashMap;
  std::mutex m_Mutex;
  std::shared_ptr<address::Pairing> m_Pairing;

 public:
  AddressMap(const std::shared_ptr<address::Pairing>& pairing)
      : m_Pairing(pairing) {}

  uint32_t pointerToInteger(void* pointer) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    uint64_t value = reinterpret_cast<uint64_t>(pointer);
    uint32_t upper = static_cast<uint32_t>(value >> 32);  // Upper 4 bytes
    uint32_t lower = static_cast<uint32_t>(value);        // Lower 4 bytes
    uint32_t key = m_Pairing->encode(upper, lower);

    m_HashMap[key] = pointer;

    return key;
  }

  void* integerToPointer(uint32_t key) {
    std::lock_guard<std::mutex> lock(m_Mutex);

    auto it = m_HashMap.find(key);

    if (it == m_HashMap.end()) return nullptr;

    return it->second;
  }
};
}  // namespace address
