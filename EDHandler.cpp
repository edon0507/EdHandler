#include "EDHandler.h"
#include "EDTimeUtils.h"

EDHandler::EDHandler()
    : stop_(false),
      stop_when_empty_(false),
			looper_(nullptr),
			run_on_local_(false) {
}

EDHandler::~EDHandler() {
	Stop(false);
//	DEBUG_PRINT("~EDHandler()..\n");
}

bool EDHandler::Start(bool in_call_thr) {
//	DEBUG_PRINT(" EDHandler() Start..\n");
	if (in_call_thr) {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if ( nullptr != looper_ || run_on_local_) {
				return false;
			}
			run_on_local_ = true;
		}
		Loop();
		return true;
	}
	std::unique_lock<std::mutex> lock(mutex_);
	if ( nullptr != looper_ || run_on_local_) {
		return false;
	}
	looper_ = std::make_shared<std::thread>([this](){
		this->Loop();
	});
	return true;
}

void EDHandler::Loop() {
  for (;;) {
		std::shared_ptr<EDEvent> event = nullptr;
		{
			std::unique_lock<std::mutex> lock(this->mutex_);
			bool times_up = true;
			if (this->event_queue_.empty()) {
				this->condition_var_.wait(lock, [this]{ return this->stop_ ||
																								this->stop_when_empty_ ||
																								!this->event_queue_.empty(); }
																	);
				times_up = false;
			} else {
				uint64 now_us = NowUs();
				int64_t when_us = this->event_queue_.front()->when;
				int64 delay = when_us - now_us;
				if (delay > 0) {
					this->condition_var_.wait_until(
						lock, ClockTimeFromMicroseconds(when_us),
						[this]{
							if (this->stop_) return true;
							else if (this->event_queue_.empty()) return false;
							else {
								uint64 now_us = NowUs();
								int64_t when_us = this->event_queue_.front()->when;
								int64 delay = when_us - now_us;
								return delay <= 0;
							}});
					times_up = false;
				}
			}

			if (this->stop_when_empty_ && this->event_queue_.empty()) {
				looper_ = nullptr;
				run_on_local_ = false;
				stop_when_empty_ = false;
				stop_ = false;
				return;
			}
				
			if (stop_) {
				looper_ = nullptr;
				run_on_local_ = false;
				stop_when_empty_ = false;
				stop_ = false;
				event_queue_.clear();
//        DEBUG_PRINT(" Loop stop..\n");
				return;
			}
			if (!times_up) continue;
			event = event_queue_.front();
			event_queue_.pop_front();
		}
		this->DispatchMessage(event->msg);
	}
//	DEBUG_PRINT(" Loop end..\n");
}

bool EDHandler::DispatchMessage(ed_msg_ptr msg) {
	if (nullptr != msg->handle_cb) {
		msg->handle_cb(msg);
	} else {
		HandleMessage(msg);
	}
	return true;
}

size_t EDHandler::GetEventSize() {
	return event_queue_.size();
}

void EDHandler::HandleMessage(ed_msg_ptr msg) {
}

void EDHandler::PostEmptyMessage(int what, int64_t delay) {
	ed_msg_ptr msg = std::make_shared<EDMessage>();
	msg->what = what;
	PostMessageDelay(msg, delay);
}

void EDHandler::PostMessageDelay(ed_msg_ptr msg, int64_t delay) {
	int64 now_us = NowUs();
	std::lock_guard<std::mutex> lock(mutex_);
//  if (stop_ || stop_when_empty_) return;
	ed_ev_ptr ev = std::make_shared<EDEvent>();
	ev->msg = msg;
	if (delay <= 0) {
		ev->when = NowUs();
	} else {
		ev->when = NowUs() + delay;
	}

	if (msg->priority < 0) {
		ev->when -= 300000000;
	}

	std::list<ed_ev_ptr>::iterator it = event_queue_.begin();
	// if (delay > 0 || msg->priority >= 0) {
	while (it != event_queue_.end() && ((*it)->when < ev->when
		|| ((*it)->when == ev->when
			&& (*it)->msg->priority >= ev->msg->priority))) {
		++it;
	}
	//} else {
	//	// msg->priority < 0 && delay <= 0
	//	while (it != event_queue_.end()
	//		&& (*it)->msg->priority < 0
	//		&& ((*it)->msg->priority > ev->msg->priority
	//			|| ((*it)->msg->priority == ev->msg->priority
	//				&& (*it)->when <= ev->when))) {
	//		++it;
	//	}
	//}
	// bool is_beg = it == event_queue_.begin();
	if (it == event_queue_.begin()) {
		condition_var_.notify_one();
	}
	/*DEBUG_PRINT(
		"%p PostMessageDelay msg what %d insert queue count %ld end use %lld NowUs %lld>\n",
		this, msg->what, event_queue_.size(), NowUs() - now_us, NowUs());*/
	event_queue_.insert(it, ev);
}

bool EDHandler::RemoveMessage(int what) {
	std::unique_lock<std::mutex> lock(mutex_);
//  if (stop_ || stop_when_empty_) return false;
	std::list<ed_ev_ptr>::iterator it = event_queue_.begin();
	bool is_find = false;
	while (it != event_queue_.end()) {
		if ((*it)->msg->what == what) {
			// must increment the iterator first
			event_queue_.erase(it++);
			is_find = true;
		} else {
			++it;
		}
	}
	return is_find;
}

bool EDHandler::Stop(bool safety) {
//	DEBUG_PRINT("Stop %d..\n", safety);
	std::shared_ptr<std::thread> looper;
	bool run_on_local;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		looper = looper_;
    looper_ = nullptr;
		run_on_local = run_on_local_;
		if (safety) {
			stop_when_empty_ = true;
		} else {
			stop_ = true;
		}
	}
	condition_var_.notify_all();
	if (nullptr == looper && !run_on_local) {
		return false;
	}
	if (nullptr != looper && looper->joinable()) {
		looper->join();
	}
	return true;
}
