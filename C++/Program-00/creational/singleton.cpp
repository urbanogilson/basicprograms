#include <iostream>
#include <memory>

class President {
 private:
  President(){};

 public:
  President(President const &) = delete;
  President &operator=(President const &) = delete;

  static std::shared_ptr<President> getInstance() {
    static std::shared_ptr<President> president{new President};
    return president;
  }
};

int main() {
  auto president1 = President::getInstance();
  auto president2 = President::getInstance();

  std::cout << std::boolalpha << (president1 == president2) << std::endl;
}
