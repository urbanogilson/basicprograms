#include <iostream>
#include <string>

class Sheep {
 private:
  std::string m_Name;
  std::string m_Category;

 public:
  Sheep(const std::string &name, const std::string &category = "Mountain Sheep")
      : m_Name(name), m_Category(category) {}

  void setName(std::string name) { m_Name = name; }

  std::string getName(void) const { return m_Name; }

  void setCategory(std::string category) { m_Name = category; }

  std::string getCategory(void) const { return m_Category; }
};

int main(void) {
  auto original = Sheep("Jolly");
  std::cout << original.getName() << std::endl;
  std::cout << original.getCategory() << std::endl;

  auto clone = original;
  clone.setName("Dolly");
  std::cout << clone.getName() << std::endl;
  std::cout << clone.getCategory() << std::endl;
}
