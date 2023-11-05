#include <iostream>
#include <vector>

#include "address_map.hpp"

int main(void) {
  auto pairing = std::make_shared<address::Cantor>();
  address::AddressMap addressMap(pairing);

  auto address = (void *)0x0123456789;
  auto encoded = addressMap.pointerToInteger(address);
  auto decoded = addressMap.integerToPointer(encoded);

  std::cout << "original: " << std::hex << address << "\n";
  std::cout << "encoded:  " << encoded << "\n";
  std::cout << "decoded:  " << std::hex << decoded << "\n";
  std::cout << "match:    " << std::boolalpha << (decoded == address) << "\n";

  return 0;
}
