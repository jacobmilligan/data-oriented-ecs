//
// FPSCounter.hpp
// ECS_Framework
//
// ----------------------------------------------------------------------------
//
// Created by Jacob Milligan on 30/10/2016.
// Copyright (c) 2016 Jacob Milligan All rights reserved.
//

#ifndef ECS_FRAMEWORK_FPSCOUNTER_HPP
#define ECS_FRAMEWORK_FPSCOUNTER_HPP


#include <SFML/System/Clock.hpp>

namespace ecs {

class FPSCounter {
public:
  FPSCounter()
  {
    currentFrame_ = clock_.restart().asSeconds();
  }

  void update();

  inline float frames_per_second() { return fps_; }

  inline float average_frames_per_second() { return totalFps_ / totalFrames_; }

  inline float delta_time()
  {
    return clock_.getElapsedTime().asSeconds() - currentFrame_;
  }

  inline float average_delta_time()
  {
    return clock_.getElapsedTime().asSeconds() / totalFrames_;
  }

  inline float total_time()
  {
    return clock_.getElapsedTime().asSeconds();
  }

  inline float total_frames() { return totalFrames_; }

  inline void reset()
  {
    clock_.restart();
    currentFrame_ = 0;
    fps_ = 0;
    lastFrame_ = 0;
    totalFrames_ = 0;
    totalFps_ = 0;
  }

private:
  float currentFrame_, fps_, lastFrame_, totalFrames_, totalFps_;
  sf::Clock clock_;
};

}


#endif //ECS_FRAMEWORK_FPSCOUNTER_HPP
