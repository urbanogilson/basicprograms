#include <iostream>
#include <memory>

class Door {
 public:
  virtual float getWidth(void) = 0;
  virtual float getHeight(void) = 0;
  virtual ~Door(){};
};

class WoodenDoor : public Door {
 private:
  float m_Width, m_Height;

 public:
  WoodenDoor(float width, float height) : m_Width(width), m_Height(height) {}
  ~WoodenDoor() override {}

  float getWidth(void) override { return m_Width; }

  float getHeight(void) override { return m_Height; }
};

class DoorFactory {
 public:
  static std::shared_ptr<Door> makeDoor(float width, float height) {
    return std::make_shared<WoodenDoor>(WoodenDoor(width, height));
  }
};

int main() {
  auto door = DoorFactory::makeDoor(1.5f, 2.3f);

  std::cout << "Width: " << door->getWidth() << std::endl;
  std::cout << "Height: " << door->getHeight() << std::endl;
}
