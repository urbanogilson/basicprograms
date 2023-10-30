#include <iostream>
#include <memory>

class Interviewer {
 public:
  virtual void askQuestions(void) = 0;
};

class Developer : public Interviewer {
 public:
  void askQuestions(void) override {
    std::cout << "Asking about design patterns!\n";
  }
};

class CommunityExecutive : public Interviewer {
  void askQuestions(void) override {
    std::cout << "Asking about community building!\n";
  }
};

class HiringManager {
 protected:
  virtual std::shared_ptr<Interviewer> makeInterviewer(void) = 0;

 public:
  void takeInterview(void) {
    std::shared_ptr<Interviewer> interviewer = makeInterviewer();
    interviewer->askQuestions();
  }
};

class DevelopmentManager : public HiringManager {
  std::shared_ptr<Interviewer> makeInterviewer(void) override {
    return std::make_shared<Developer>();
  }
};

class MarketingManager : public HiringManager {
  std::shared_ptr<Interviewer> makeInterviewer(void) override {
    return std::make_shared<CommunityExecutive>();
  }
};

int main(void) {
  auto devManager = DevelopmentManager();
  devManager.takeInterview();  // Output: Asking about design patterns!

  auto marketingManager = MarketingManager();
  marketingManager.takeInterview();  // Output: Asking about community building!
}
