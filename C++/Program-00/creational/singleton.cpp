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
  std::shared_ptr<President> president1 = President::getInstance();
  std::shared_ptr<President> president2 = President::getInstance();

  std::cout << (president1 == president2) << std::endl;

  return 0;
}
