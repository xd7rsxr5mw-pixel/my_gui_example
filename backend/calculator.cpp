#include "calculator.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace showcase {
double Calculator::add(double a, double b) const {
  if (!std::isfinite(a) || !std::isfinite(b))
    throw std::invalid_argument("Enter two finite numbers.");
  const double sum = a + b;
  if (!std::isfinite(sum))
    throw std::overflow_error("The sum is outside the numeric range.");
  return sum;
}
Sample Calculator::sample(double time, double amplitude,
                          double frequency) const {
  return {time,
          amplitude * std::sin(2.0 * std::numbers::pi * frequency * time)};
}
std::vector<Sample> Calculator::curve(double amplitude, double frequency,
                                      int count) const {
  count = std::clamp(count, 2, 100000);
  std::vector<Sample> points;
  points.reserve(count);
  for (int index = 0; index < count; ++index)
    points.push_back(sample(10.0 * index / (count - 1), amplitude, frequency));
  return points;
}
double Calculator::meanSquare(std::stop_token stop, int count) const {
  if (count <= 0)
    throw std::invalid_argument("The sample count must be positive.");
  double sum = 0;
  for (int index = 0; index < count; ++index) {
    if (index % 1024 == 0 && stop.stop_requested())
      return 0;
    const double value = sample(static_cast<double>(index) / count * 10).y;
    sum += value * value;
  }
  return sum / count;
}
} // namespace showcase
