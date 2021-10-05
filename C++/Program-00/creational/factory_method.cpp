#include <iostream>
#include <memory>

class Interviewer
{
public:
    virtual void askQuestions(void) = 0;
};

class Developer : public Interviewer
{
public:
    void askQuestions(void) override
    {
        std::cout << "Asking about design patterns!" << std::endl;
    }
};

class CommunityExecutive : public Interviewer
{
    void askQuestions(void) override
    {
        std::cout << "Asking about community building!" << std::endl;
    }
};

class HiringManager
{
protected:
    virtual std::shared_ptr<Interviewer> makeInterviewer(void) = 0;

public:
    void takeInterview(void)
    {
        std::shared_ptr<Interviewer> interviewer = makeInterviewer();
        interviewer->askQuestions();
    }
};

class DevelopmentManager : public HiringManager
{
    std::shared_ptr<Interviewer> makeInterviewer(void) override
    {
        return std::make_shared<Developer>();
    }
};

class MarketingManager : public HiringManager
{
    std::shared_ptr<Interviewer> makeInterviewer(void) override
    {
        return std::make_shared<CommunityExecutive>();
    }
};

int main()
{
    DevelopmentManager devManager = DevelopmentManager();
    devManager.takeInterview(); // Output: Asking about design patterns

    MarketingManager marketingManager = MarketingManager();
    marketingManager.takeInterview(); // Output

    return 0;
}
