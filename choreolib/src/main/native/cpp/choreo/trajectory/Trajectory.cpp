// Copyright (c) Choreo contributors

#include "choreo/trajectory/Trajectory.h"

#include <string>

#include <wpi/util/json.hpp>

static std::vector<int> json_to_int_vector(const wpi::util::json& j) {
  std::vector<int> out;
  for (const auto& elem : j.get_array()) {
    out.push_back(static_cast<int>(elem.get_int()));
  }
  return out;
}

template <typename T>
static std::vector<T> json_to_sample_vector(const wpi::util::json& j) {
  std::vector<T> out;
  for (const auto& elem : j.get_array()) {
    T sample;
    choreo::from_json(elem, sample);
    out.push_back(std::move(sample));
  }
  return out;
}

static std::vector<choreo::EventMarker> json_to_event_vector(
    const wpi::util::json& j) {
  std::vector<choreo::EventMarker> out;
  for (const auto& elem : j.get_array()) {
    choreo::EventMarker event;
    choreo::from_json(elem, event);
    out.push_back(std::move(event));
  }
  return out;
}

void choreo::to_json(wpi::util::json& json,
                     const Trajectory<SwerveSample>& trajectory) {
  wpi::util::json samples_json;
  samples_json.set_array();
  for (const auto& s : trajectory.samples) {
    wpi::util::json sj;
    to_json(sj, s);
    samples_json.emplace_back(std::move(sj));
  }
  wpi::util::json splits_json{
      std::vector<wpi::util::json>(trajectory.splits.begin(),
                                    trajectory.splits.end())};
  wpi::util::json events_json;
  events_json.set_array();
  for (const auto& e : trajectory.events) {
    wpi::util::json ej;
    to_json(ej, e);
    events_json.emplace_back(std::move(ej));
  }
  json = wpi::util::json::object("name", trajectory.name, "samples",
                                  std::move(samples_json), "splits",
                                  std::move(splits_json), "events",
                                  std::move(events_json));
}

void choreo::from_json(const wpi::util::json& json,
                       Trajectory<SwerveSample>& trajectory) {
  trajectory.name = json.at("name").get_string();
  trajectory.samples =
      json_to_sample_vector<SwerveSample>(json.at("trajectory").at("samples"));
  trajectory.splits =
      json_to_int_vector(json.at("trajectory").at("splits"));
  if (trajectory.splits.empty() || trajectory.splits.at(0) != 0) {
    trajectory.splits.insert(trajectory.splits.begin(), 0);
  }
  auto events = json_to_event_vector(json.at("events"));
  trajectory.events.clear();
  for (EventMarker event : events) {
    if (event.timestamp >= wpi::units::second_t{0} || event.event.size() == 0) {
      trajectory.events.push_back(event);
    }
  }
}

void choreo::to_json(wpi::util::json& json,
                     const Trajectory<DifferentialSample>& trajectory) {
  wpi::util::json samples_json;
  samples_json.set_array();
  for (const auto& s : trajectory.samples) {
    wpi::util::json sj;
    to_json(sj, s);
    samples_json.emplace_back(std::move(sj));
  }
  wpi::util::json splits_json{
      std::vector<wpi::util::json>(trajectory.splits.begin(),
                                    trajectory.splits.end())};
  wpi::util::json events_json;
  events_json.set_array();
  for (const auto& e : trajectory.events) {
    wpi::util::json ej;
    to_json(ej, e);
    events_json.emplace_back(std::move(ej));
  }
  json = wpi::util::json::object("name", trajectory.name, "samples",
                                  std::move(samples_json), "splits",
                                  std::move(splits_json), "events",
                                  std::move(events_json));
}

void choreo::from_json(const wpi::util::json& json,
                       Trajectory<DifferentialSample>& trajectory) {
  trajectory.samples = json_to_sample_vector<DifferentialSample>(
      json.at("trajectory").at("samples"));
  trajectory.splits =
      json_to_int_vector(json.at("trajectory").at("splits"));
  if (trajectory.splits.empty() || trajectory.splits.at(0) != 0) {
    trajectory.splits.insert(trajectory.splits.begin(), 0);
  }
  auto events = json_to_event_vector(json.at("events"));
  trajectory.events.clear();
  for (EventMarker event : events) {
    if (event.timestamp >= wpi::units::second_t{0} || event.event.size() == 0) {
      trajectory.events.push_back(event);
    }
  }
}
