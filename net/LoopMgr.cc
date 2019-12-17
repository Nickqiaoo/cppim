#include "LoopMgr.h"
#include "Loop.h"

LoopMgr::LoopMgr(size_t thrnum) {
  for (int i = 0; i < thrnum; i++) {
    loops_.emplace_back(std::make_shared<Loop>());
  }
}

LoopMgr::~LoopMgr() {}

void LoopMgr::start(){
  for(auto loop : loops_){
    loop->start();
  }
}

LoopPtr LoopMgr::findNextLoop() {
  next_ = ++next_ >= loops_.size() ? 1 : next_;
  return loops_[next_];
}