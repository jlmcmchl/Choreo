// Copyright (c) Choreo contributors

#include "choreo/trajectory/DifferentialSample.h"

#include <wpi/util/json.hpp>

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
  trajectorySample.timestamp = wpi::units::second_t{json.at("t").get_double()};
  trajectorySample.x = wpi::units::meter_t{json.at("x").get_double()};
  trajectorySample.y = wpi::units::meter_t{json.at("y").get_double()};
  trajectorySample.heading = wpi::units::radian_t{json.at("heading").get_double()};
  trajectorySample.vl = wpi::units::meters_per_second_t{json.at("vl").get_double()};
  trajectorySample.vr = wpi::units::meters_per_second_t{json.at("vr").get_double()};
  trajectorySample.omega =
      wpi::units::radians_per_second_t{json.at("omega").get_double()};
  trajectorySample.al =
      wpi::units::meters_per_second_squared_t{json.at("al").get_double()};
  trajectorySample.ar =
      wpi::units::meters_per_second_squared_t{json.at("ar").get_double()};
  trajectorySample.fl = wpi::units::newton_t{json.at("fl").get_double()};
  trajectorySample.fr = wpi::units::newton_t{json.at("fr").get_double()};
}
