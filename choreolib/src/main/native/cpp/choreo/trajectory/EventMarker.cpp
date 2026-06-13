// Copyright (c) Choreo contributors

#include "choreo/trajectory/EventMarker.h"

#include <string>

#include <wpi/util/json.hpp>

static inline double json_get_double(const wpi::util::json& j) {
  if (j.is_int()) return static_cast<double>(j.get_int());
  if (j.is_uint()) return static_cast<double>(j.get_uint());
  return j.get_double();
}

void choreo::to_json(wpi::util::json& json, const EventMarker& event) {
  json = wpi::util::json::object(
      "data", wpi::util::json::object("t", event.timestamp.value()),
      "event", wpi::util::json::object("name", event.event));
}

void choreo::from_json(const wpi::util::json& json, EventMarker& event) {
  auto targetTimestamp = json.at("from").at("targetTimestamp");
  if (!targetTimestamp.is_number()) {
    event.timestamp = wpi::units::second_t{-1};
    event.event = "";
  } else {
    event.timestamp =
        wpi::units::second_t{json_get_double(json.at("from").at("offset").at("val")) +
                        json_get_double(targetTimestamp)};
    event.event = json.at("name").get_string();
  }
}
