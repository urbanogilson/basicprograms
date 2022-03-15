#include "threadpool.h"

namespace kvdb {
Threadpool::Threadpool(size_t num_threads) : num_threads_(num_threads) {}

Threadpool::~Threadpool() {
  for (auto &&thread : threads_) {
    thread.join();
  }
}

void Threadpool::ProcessingLoop(void) {
  while (true) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.size() == 0) {
      condition_variable_.wait(lock);
    }
    auto task = queue_.front();
    queue_.pop();
    task->RunInLock(std::this_thread::get_id());
    lock.unlock();
    task->Run(std::this_thread::get_id());
    delete task;
  }
}

void Threadpool::AddTask(Task *task) {
  std::unique_lock<std::mutex> lock(mutex_);
  queue_.push(task);
  condition_variable_.notify_one();
}

int Threadpool::Start(void) {
  for (size_t i = 0; i < num_threads_; i++) {
    threads_.push_back(std::thread(&Threadpool::ProcessingLoop, this));
  }
  return 0;
}

int Threadpool::Stop(void) { return 0; }
}  // namespace kvdb
