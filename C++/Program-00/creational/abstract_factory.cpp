#include <iostream>
#include <memory>

class Door {
 public:
  virtual void getDescription(void) = 0;
};

class WoodenDoor : public Door {
 public:
  void getDescription(void) override { std::cout << "I am a wooden door\n"; }
};

class IronDoor : public Door {
 public:
  void getDescription(void) override { std::cout << "I am a iron door\n"; }
};

class DoorFittingExpert {
 public:
  virtual void getDescription(void) = 0;
};

class Welder : public DoorFittingExpert {
 public:
  void getDescription(void) override {
    std::cout << "I can only fit iron doors\n";
  };
};

class Carpenter : public DoorFittingExpert {
 public:
  void getDescription(void) override {
    std::cout << "I can only fit wooden doors\n";
  };
};

class DoorFactory {
 public:
  virtual std::shared_ptr<Door> makeDoor(void) = 0;
  virtual std::shared_ptr<DoorFittingExpert> makeFittingExpert(void) = 0;
};

class WoodenDoorFactory : public DoorFactory {
 public:
  std::shared_ptr<Door> makeDoor(void) override {
    return std::make_shared<WoodenDoor>();
  };

  std::shared_ptr<DoorFittingExpert> makeFittingExpert(void) override {
    return std::make_shared<Carpenter>();
  };
};

class IronDoorFactory : public DoorFactory {
 public:
  std::shared_ptr<Door> makeDoor(void) override {
    return std::make_shared<IronDoor>();
  };

  std::shared_ptr<DoorFittingExpert> makeFittingExpert(void) override {
    return std::make_shared<Welder>();
  };
};

int main(void) {
  auto woodenFactory = WoodenDoorFactory();

  auto door = woodenFactory.makeDoor();
  auto expert = woodenFactory.makeFittingExpert();

  door->getDescription();    // Output: I am a wooden door
  expert->getDescription();  // Output: I can only fit wooden doors

  // Same for Iron Factory
  auto ironFactory = IronDoorFactory();

  door = ironFactory.makeDoor();
  expert = ironFactory.makeFittingExpert();

  door->getDescription();    // Output: I am an iron door
  expert->getDescription();  // Output: I can only fit iron doors
}
