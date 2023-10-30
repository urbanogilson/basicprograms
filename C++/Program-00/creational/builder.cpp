#include <iostream>
#include <memory>

class Burger;

class BurgerBuilder {
 public:
  int m_Size;

  bool m_Cheese;
  bool m_Pepperoni;
  bool m_Lettuce;
  bool m_Tomato;

  BurgerBuilder(int size)
      : m_Size(size),
        m_Cheese(false),
        m_Pepperoni(false),
        m_Lettuce(false),
        m_Tomato(false) {}

  BurgerBuilder &addCheese(void) {
    m_Cheese = true;
    return *this;
  }

  BurgerBuilder &addPepperoni(void) {
    m_Pepperoni = true;
    return *this;
  }

  BurgerBuilder &addLettuce(void) {
    m_Lettuce = true;
    return *this;
  }

  BurgerBuilder &addTomato(void) {
    m_Tomato = true;
    return *this;
  }

  std::shared_ptr<Burger> build(void) { return std::make_shared<Burger>(this); }
};

class Burger {
 protected:
  bool m_Cheese;
  bool m_Pepperoni;
  bool m_Lettuce;
  bool m_Tomato;

 public:
  Burger(BurgerBuilder *builder)
      : m_Cheese(builder->m_Cheese),
        m_Pepperoni(builder->m_Pepperoni),
        m_Lettuce(builder->m_Lettuce),
        m_Tomato(builder->m_Tomato) {}
};

int main(void) {
  auto burger =
      BurgerBuilder(14).addPepperoni().addLettuce().addTomato().build();
}
