#ifndef KVDB_THREADPOOL_
#define KVDB_THREADPOOL_

#include <condition_variable>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>

namespace kvdb {

class Task {
 public:
  Task() {}
  virtual ~Task() {}
  virtual void RunInLock(std::thread::id thread_id) = 0;
  virtual void Run(std::thread::id thread_id) = 0;
};

class Threadpool {
 public:
  Threadpool(size_t num_threads);
  ~Threadpool();
  void ProcessingLoop(void);
  void AddTask(Task *task);
  int Start(void);
  int Stop(void);

 private:
  size_t num_threads_;
  std::queue<Task *> queue_;
  std::condition_variable condition_variable_;
  std::mutex mutex_;
  std::vector<std::thread> threads_;
};
}  // namespace kvdb

#endif  // KVDB_THREADPOOL_