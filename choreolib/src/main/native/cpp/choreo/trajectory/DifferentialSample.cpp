// Copyright (c) Choreo contributors

#include "choreo/trajectory/DifferentialSample.h"

#include <wpi/util/json.hpp>

static inline double json_get_double(const wpi::util::json& j) {
  if (j.is_int()) return static_cast<double>(j.get_int());
  if (j.is_uint()) return static_cast<double>(j.get_uint());
  return j.get_double();
}

void choreo::to_json(wpi::util::json& json,
                     const DifferentialSample& trajectorySample) {
  json = wpi::util::json::object(
      "t", trajectorySample.timestamp.value(),
      "x", trajectorySample.x.value(),
      "y", trajectorySample.y.value(),
      "heading", trajectorySample.heading.value(),
      "vl", trajectorySample.vl.value(),
      "vr", trajectorySample.vr.value(),
      "omega", trajectorySample.omega.value(),
      "al", trajectorySample.al.value(),
      "ar", trajectorySample.ar.value(),
      "fl", trajectorySample.fl.value(),
      "fr", trajectorySample.fr.value());
}

void choreo::from_json(const wpi::util::json& json,
                       DifferentialSample& trajectorySample) {
  trajectorySample.timestamp = wpi::units::second_t{json_get_double(json.at("t"))};
  trajectorySample.x = wpi::units::meter_t{json_get_double(json.at("x"))};
  trajectorySample.y = wpi::units::meter_t{json_get_double(json.at("y"))};
  trajectorySample.heading = wpi::units::radian_t{json_get_double(json.at("heading"))};
  trajectorySample.vl = wpi::units::meters_per_second_t{json_get_double(json.at("vl"))};
  trajectorySample.vr = wpi::units::meters_per_second_t{json_get_double(json.at("vr"))};
  trajectorySample.omega =
      wpi::units::radians_per_second_t{json_get_double(json.at("omega"))};
  trajectorySample.al =
      wpi::units::meters_per_second_squared_t{json_get_double(json.at("al"))};
  trajectorySample.ar =
      wpi::units::meters_per_second_squared_t{json_get_double(json.at("ar"))};
  trajectorySample.fl = wpi::units::newton_t{json_get_double(json.at("fl"))};
  trajectorySample.fr = wpi::units::newton_t{json_get_double(json.at("fr"))};
}
