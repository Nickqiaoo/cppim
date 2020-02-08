#include "LoopMgr.h"
#include "Loop.h"

LoopMgr::LoopMgr(size_t thrnum) {
  for (size_t i = 0; i < thrnum; i++) {
    loops_.emplace_back(new Loop());
  }
}

LoopMgr::~LoopMgr() {
  for(auto loop : loops_){
    delete loop;
  }
}

void LoopMgr::start(){
  for(auto loop : loops_){
    loop->start();
  }
}

void LoopMgr::stop(){
  for(auto loop : loops_){
    loop->stop();
  }
}

Loop* LoopMgr::findNextLoop() {
  if(++next_ == loops_.size()){
    next_ = 0;
  }
  return loops_[next_];
}