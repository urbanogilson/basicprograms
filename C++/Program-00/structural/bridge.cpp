#include <string>
#include <memory>
#include <iostream>

class Theme
{
public:
    virtual std::string getColor(void) = 0;
};

class DarkTheme : public Theme
{
public:
    std::string getColor(void) override
    {
        return "Dark black";
    }
};

class LightTheme : public Theme
{
public:
    std::string getColor(void) override
    {
        return "Off white";
    }
};

class AquaTheme : public Theme
{
public:
    std::string getColor(void) override
    {
        return "Light blue";
    }
};

class WebPage
{
public:
    virtual std::string getContent(void) = 0;
};

class About : public WebPage
{
private:
    std::shared_ptr<Theme> m_Theme;

public:
    About(std::shared_ptr<Theme> theme)
        : m_Theme(theme){

          };

    std::string getContent()
    {
        return "About page in " + m_Theme->getColor();
    };
};

class Careers : public WebPage
{
private:
    std::shared_ptr<Theme> m_Theme;

public:
    Careers(std::shared_ptr<Theme> theme)
        : m_Theme(theme){

          };

    std::string getContent()
    {
        return "Careers page in " + m_Theme->getColor();
    };
};

int main(void)
{
    std::shared_ptr<DarkTheme> darkTheme = std::make_shared<DarkTheme>();

    About about = About(darkTheme);
    Careers careers = Careers(darkTheme);

    std::cout << about.getContent() << std::endl;   // "About page in Dark Black";
    std::cout << careers.getContent() << std::endl; // "Careers page in Dark Black";
    return 0;
}
