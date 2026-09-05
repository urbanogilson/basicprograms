#include <iostream>

#include "raii.hpp"

class Resource {
public:
  Resource() { std::cout << "Resource acquired\n"; }
  ~Resource() { std::cout << "Resource released\n"; }
};

raii::AutoPtr<Resource> createResource() {
  raii::AutoPtr<Resource> res{new Resource()};
  return res;
}

int main(void) {
  std::cout << "program begin\n";

  {
    std::cout << "scope begin\n";
    raii::AutoPtr<Resource> resource(new Resource());
    raii::AutoPtr<Resource> resource2;

    std::cout << "resource1 is null: " << resource.isNull() << "\n";
    std::cout << "resource2 is null: " << resource2.isNull() << "\n";

    resource2 = std::move(resource);

    std::cout << "resource1 is null: " << resource.isNull() << "\n";
    std::cout << "resource2 is null: " << resource2.isNull() << "\n";
    std::cout << "scope end\n";
  }

  {
    std::cout << "scope begin\n";
    raii::AutoPtr<Resource> resource = createResource();
    std::cout << "scope end\n";
  }

  std::cout << "program end\n";
}
