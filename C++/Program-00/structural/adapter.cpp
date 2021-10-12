#include <memory>

class Lion
{
public:
    virtual void roar(void) = 0;
};

class AfricanLion : public Lion
{
public:
    void roar(void) override
    {
    }
};

class AsianLion : public Lion
{
public:
    void roar(void) override
    {
    }
};

class Hunter
{
public:
    void hunt(Lion &lion)
    {
        lion.roar();
    }
};

class WildDog
{
public:
    void bark(void)
    {
    }
};

class WildDogAdapter : public Lion
{
private:
    std::shared_ptr<WildDog> m_Dog;

public:
    WildDogAdapter(std::shared_ptr<WildDog> dog)
        : m_Dog(dog)
    {
    }

    void roar(void) override
    {
        m_Dog->bark();
    }
};

int main(void)
{
    std::shared_ptr<WildDog> wildDog = std::make_shared<WildDog>();
    WildDogAdapter wildDogAdapter = WildDogAdapter(wildDog);

    Hunter hunter = Hunter();
    hunter.hunt(wildDogAdapter);

    return 0;
}
