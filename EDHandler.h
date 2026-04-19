// Copyright 2019 edon
#ifndef ED_HANDLER_H_
#define ED_HANDLER_H_
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <functional>
#include "Common.h"

struct EDMessage;
typedef std::shared_ptr<EDMessage> ed_msg_ptr;
// typedef void (* MessageHandleCB)(ed_msg_ptr msg);
typedef std::function<void(ed_msg_ptr msg)> MessageHandleCB;
struct EDMessage {
  int what;
  int priority;
  MessageHandleCB handle_cb;
  std::shared_ptr<void> data;
  EDMessage()
    : what(0),
      priority(0),
      handle_cb(nullptr),
      data(nullptr) {
  }
  EDMessage(const EDMessage& msg)
    : what(msg.what),
      priority(msg.priority),
      handle_cb(msg.handle_cb),
      data(msg.data) {
  }
};

struct EDEvent {
  ed_msg_ptr msg;
  int64_t when;
};
typedef std::shared_ptr<EDEvent> ed_ev_ptr;

class EDHandler {
 public:
  EDHandler();
  virtual ~EDHandler();
  /**
   * post a message
   * @msg : the message you wanna post
   * @delay : delay in milliseconds
   * */
  void PostMessageDelay(ed_msg_ptr msg, int64_t delay = 0);
  /**
   * post a empty message 
   * */
  void PostEmptyMessage(int what, int64_t delay = 0);

  size_t GetEventSize();

  /**
   * remove message
   * */
  bool RemoveMessage(int what);

  virtual bool Start(bool in_call_thr = false);
  /**
   * stop the loop
   * */
  bool Stop(bool safety);

  virtual void HandleMessage(ed_msg_ptr msg);

 private:
  /* the thread where this handler run on*/
  std::shared_ptr<std::thread> looper_;
  std::mutex mutex_;
  std::condition_variable condition_var_;
  std::list<ed_ev_ptr> event_queue_;
  bool stop_;
  bool stop_when_empty_;
  bool run_on_local_;
  void Loop();
  bool DispatchMessage(ed_msg_ptr msg);
  DISALLOW_EVIL_CONSTRUCTORS(EDHandler);
};
#endif
