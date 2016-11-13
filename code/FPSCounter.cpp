//
// FPSCounter.cpp
// ECS_Framework
//
// ----------------------------------------------------------------------------
//
// Created by Jacob Milligan on 30/10/2016.
// Copyright (c) 2016 Jacob Milligan All rights reserved.
//

#include "FPSCounter.hpp"

namespace ecs {

void FPSCounter::update()
{
  currentFrame_ = clock_.getElapsedTime().asSeconds();
  fps_ = 1.0f / (currentFrame_ - lastFrame_);
  lastFrame_ = currentFrame_;
  totalFps_ += fps_;
  totalFrames_++;
}

}