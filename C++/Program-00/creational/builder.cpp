#include <iostream>
#include <memory>

class Burger;

class BurgerBuilder {
 public:
  int size;

  bool cheese;
  bool pepperoni;
  bool lettuce;
  bool tomato;

  BurgerBuilder(int size)
      : size(size),
        cheese(false),
        pepperoni(false),
        lettuce(false),
        tomato(false) {}

  BurgerBuilder &addCheese(void) {
    cheese = true;
    return *this;
  }

  BurgerBuilder &addPepperoni(void) {
    pepperoni = true;
    return *this;
  }

  BurgerBuilder &addLettuce(void) {
    lettuce = true;
    return *this;
  }

  BurgerBuilder &addTomato(void) {
    tomato = true;
    return *this;
  }

  std::shared_ptr<Burger> build(void) { return std::make_shared<Burger>(this); }
};

class Burger {
 protected:
  bool cheese;
  bool pepperoni;
  bool lettuce;
  bool tomato;

 public:
  Burger(BurgerBuilder *builder)
      : cheese(builder->cheese),
        pepperoni(builder->pepperoni),
        lettuce(builder->lettuce),
        tomato(builder->tomato) {}
};

int main(void) {
  std::shared_ptr<Burger> burger =
      BurgerBuilder(14).addPepperoni().addLettuce().addTomato().build();

  return 0;
}
