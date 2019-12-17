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
  if(++next_ == loops_.size()){
    next_ = 0;
  }
  return loops_[next_];
}