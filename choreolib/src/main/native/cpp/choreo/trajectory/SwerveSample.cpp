// Copyright (c) Choreo contributors

#include "choreo/trajectory/SwerveSample.h"

#include <algorithm>

#include <wpi/util/json.hpp>

static inline double json_get_double(const wpi::util::json& j) {
  if (j.is_int()) return static_cast<double>(j.get_int());
  if (j.is_uint()) return static_cast<double>(j.get_uint());
  return j.get_double();
}

void choreo::to_json(wpi::util::json& json, const SwerveSample& trajectorySample) {
  std::array<double, 4> fx;
  std::transform(trajectorySample.moduleForcesX.begin(),
                 trajectorySample.moduleForcesX.end(), fx.begin(),
                 [](wpi::units::newton_t x) { return x.value(); });

  std::array<double, 4> fy;
  std::transform(trajectorySample.moduleForcesY.begin(),
                 trajectorySample.moduleForcesY.end(), fy.begin(),
                 [](wpi::units::newton_t x) { return x.value(); });

  json = wpi::util::json::object(
      "t", trajectorySample.timestamp.value(),
      "x", trajectorySample.x.value(),
      "y", trajectorySample.y.value(),
      "heading", trajectorySample.heading.value(),
      "vx", trajectorySample.vx.value(),
      "vy", trajectorySample.vy.value(),
      "omega", trajectorySample.omega.value(),
      "ax", trajectorySample.ax.value(),
      "ay", trajectorySample.ay.value(),
      "alpha", trajectorySample.alpha.value(),
      "fx", fx,
      "fy", fy);
}

void choreo::from_json(const wpi::util::json& json, SwerveSample& trajectorySample) {
  trajectorySample.timestamp = wpi::units::second_t{json_get_double(json.at("t"))};
  trajectorySample.x = wpi::units::meter_t{json_get_double(json.at("x"))};
  trajectorySample.y = wpi::units::meter_t{json_get_double(json.at("y"))};
  trajectorySample.heading = wpi::units::radian_t{json_get_double(json.at("heading"))};
  trajectorySample.vx = wpi::units::meters_per_second_t{json_get_double(json.at("vx"))};
  trajectorySample.vy = wpi::units::meters_per_second_t{json_get_double(json.at("vy"))};
  trajectorySample.omega =
      wpi::units::radians_per_second_t{json_get_double(json.at("omega"))};
  trajectorySample.ax =
      wpi::units::meters_per_second_squared_t{json_get_double(json.at("ax"))};
  trajectorySample.ay =
      wpi::units::meters_per_second_squared_t{json_get_double(json.at("ay"))};
  trajectorySample.alpha =
      wpi::units::radians_per_second_squared_t{json_get_double(json.at("alpha"))};
  const auto& fx = json.at("fx").get_array();
  const auto& fy = json.at("fy").get_array();
  for (int i = 0; i < 4; ++i) {
    trajectorySample.moduleForcesX[i] = wpi::units::newton_t{json_get_double(fx[i])};
    trajectorySample.moduleForcesY[i] = wpi::units::newton_t{json_get_double(fy[i])};
  }
}
