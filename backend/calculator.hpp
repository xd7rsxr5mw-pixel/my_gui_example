#pragma once
#include <stop_token>
#include <vector>

namespace showcase {
struct Sample {
  double x;
  double y;
};
class Calculator {
public:
  double add(double a, double b) const;
  std::vector<Sample> curve(double amplitude, double frequency,
                            int count = 500) const;
  Sample sample(double time, double amplitude = 1.0,
                double frequency = 0.5) const;
  double meanSquare(std::stop_token stop, int count = 2000000) const;
};
} // namespace showcase
