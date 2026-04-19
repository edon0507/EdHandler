#include <iostream>
#include "EDHandler.h"

class MyHandler : public EDHandler {
  virtual void HandleMessage(ed_msg_ptr msg) {
    std::cout << "default and empty message handler msg what: " << msg->what << " priority: " << msg->priority << std::endl;
  }
};

struct Data {
  int value;
};

int main() {
    MyHandler myHandler;
    myHandler.Start();
    ed_msg_ptr msg_without_cb = std::make_shared<EDMessage>();
    msg_without_cb->priority = 1;

    ed_msg_ptr msg1 = std::make_shared<EDMessage>();
    msg1->priority = 1;
    msg1->handle_cb = [](ed_msg_ptr msg) {
      std::cout << "message1 handler msg what: " << msg->what << " priority: " << msg->priority << std::endl;
    };
    ed_msg_ptr msg2 = std::make_shared<EDMessage>();
    msg2->priority = 2;
    msg2->handle_cb = [](ed_msg_ptr msg) {
      std::cout << "message2 handler msg what: " << msg->what << " priority: " << msg->priority << std::endl;
    };
    ed_msg_ptr msg_delay = std::make_shared<EDMessage>();
    msg_delay->handle_cb = [](ed_msg_ptr msg) {
      std::cout << "message delay handler msg what: " << msg->what << " priority: " << msg->priority << std::endl;
    };

    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->value = 200;
    ed_msg_ptr msg_with_data = std::make_shared<EDMessage>();
    msg_with_data->data = data;
    msg_with_data->handle_cb = [](ed_msg_ptr msg) {
      std::shared_ptr<Data> data = std::static_pointer_cast<Data>(msg->data);
      std::cout << "message with data handler msg what: " << msg->what << " priority: " << msg->priority << " data: " << data->value << std::endl;
    };
    
    myHandler.PostEmptyMessage(1);
    myHandler.PostMessageDelay(msg_without_cb);
    myHandler.PostMessageDelay(msg1);
    myHandler.PostMessageDelay(msg2);
    myHandler.PostMessageDelay(msg_delay, 1000000);  // 1s delay
    myHandler.PostMessageDelay(msg_with_data);
    
    while(true) {

    }
    return 0;
}

