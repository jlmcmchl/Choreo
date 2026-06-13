// Copyright (c) Choreo contributors

#pragma once

#include <concepts>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <fmt/format.h>
#include <wpi/system/Errors.hpp>
#include <wpi/system/Filesystem.hpp>
#include <wpi/hal/UsageReporting.h>
#include <wpi/util/MemoryBuffer.hpp>
#include <wpi/util/json.hpp>
#include <wpi/util/string.hpp>

#include "choreo/trajectory/DifferentialSample.h"
#include "choreo/trajectory/SwerveSample.h"
#include "choreo/trajectory/Trajectory.h"
#include "choreo/trajectory/TrajectorySample.h"
#include "choreo/util/TrajSchemaVersion.h"

namespace choreo {

/**
 * A class that handles loading choreo and caching choreo trajectories.
 */
class Choreo {
 public:
  /**
   * Load a trajectory from the deploy directory. Choreolib expects .traj files
   * to be placed in src/main/deploy/choreo/[trajectoryName].traj.
   *
   * @tparam SampleType The type of samples in the trajectory.
   * @param trajectoryName The path name in Choreo, which matches the file name
   *   in the deploy directory, file extension is optional.
   * @return The loaded trajectory, or `empty std::optional` if the trajectory
   *   could not be loaded.
   */
  template <TrajectorySample SampleType>
  static std::optional<Trajectory<SampleType>> LoadTrajectory(
      std::string_view trajectoryName) {
    if (trajectoryName.ends_with(TRAJECTORY_FILE_EXTENSION)) {
      trajectoryName = trajectoryName.substr(
          0, trajectoryName.size() - TRAJECTORY_FILE_EXTENSION.size());
    }

    std::string trajectoryFileName = fmt::format(
        "{}/{}{}", CHOREO_DIR, trajectoryName, TRAJECTORY_FILE_EXTENSION);

    auto fileBuffer = wpi::util::MemoryBuffer::GetFile(trajectoryFileName);
    if (!fileBuffer) {
      WPILIB_ReportError(wpi::warn::Warning, "Could not find trajectory file: {}",
                      trajectoryName);
      return {};
    }

    auto& buf = *fileBuffer;
    return LoadTrajectoryString<SampleType>(
        std::string_view{buf->GetCharBuffer().data(), buf->size()},
        trajectoryName);
  }

  /**
   * Load a trajectory from a string.
   *
   * @tparam SampleType The type of samples in the trajectory.
   * @param trajectoryJsonString The JSON string.
   * @param trajectoryName The path name in Choreo, which matches the file name
   *   in the deploy directory, file extension is optional.
   * @return The loaded trajectory, or `empty std::optional` if the trajectory
   *   could not be loaded.
   */
  template <TrajectorySample SampleType>
  static std::optional<Trajectory<SampleType>> LoadTrajectoryString(
      std::string_view trajectoryJsonString, std::string_view trajectoryName) {
    if constexpr (std::same_as<SampleType, SwerveSample>) {
      auto resource = wpi::util::make_string("ChoreoLib/SwerveTrajectory");
      auto data = wpi::util::make_string("1");
      HAL_ReportUsage(&resource, &data);
    } else if constexpr (std::same_as<SampleType, DifferentialSample>) {
      auto resource = wpi::util::make_string("ChoreoLib/DifferentialTrajectory");
      auto data = wpi::util::make_string("2");
      HAL_ReportUsage(&resource, &data);
    }

    auto jsonResult = wpi::util::json::parse(trajectoryJsonString);
    if (!jsonResult) {
      WPILIB_ReportError(wpi::warn::Warning, "Could not parse trajectory file: {}",
                      trajectoryName);
      return {};
    }
    wpi::util::json json = std::move(*jsonResult);
    uint32_t version = static_cast<uint32_t>(json.at("version").get_double());
    if (version != kTrajSchemaVersion) {
      throw fmt::format("{}.traj: Wrong version {}. Expected {}",
                        trajectoryName, version, kTrajSchemaVersion);
    }
    Trajectory<SampleType> trajectory;
    from_json(json, trajectory);
    return trajectory;
  }

  /**
   * A utility for caching loaded trajectories. This allows for loading
   * trajectories only once, and then reusing them.
   */
  template <TrajectorySample SampleType>
  class TrajectoryCache {
   public:
    /**
     * Load a trajectory from the deploy directory. Choreolib expects .traj
     * files to be placed in src/main/deploy/choreo/[trajectoryName].traj.
     *
     * This method will cache the loaded trajectory and reused it if it is
     * requested again.
     *
     * @param trajectoryName the path name in Choreo, which matches the file
     *     name in the deploy directory, file extension is optional.
     * @return the loaded trajectory, or `empty std::optional` if the trajectory
     *     could not be loaded.
     * @see Choreo#LoadTrajectory(std::string_view)
     */
    static std::optional<Trajectory<SampleType>> LoadTrajectory(
        std::string_view trajectoryName) {
      if (cache.contains(std::string{trajectoryName})) {
        return cache[std::string{trajectoryName}];
      } else {
        cache[std::string{trajectoryName}] =
            Choreo::LoadTrajectory<SampleType>(trajectoryName);
        return cache[std::string{trajectoryName}];
      }
    }

    /**
     * Load a section of a split trajectory from the deploy directory. Choreolib
     * expects .traj files to be placed in
     * src/main/deploy/choreo/[trajectoryName].traj.
     *
     * This method will cache the loaded trajectory and reused it if it is
     * requested again. The trajectory that is split off of will also be cached.
     *
     * @param trajectoryName the path name in Choreo, which matches the file
     *     name in the deploy directory, file extension is optional.
     * @param splitIndex the index of the split trajectory to load
     * @return the loaded trajectory, or `empty std::optional` if the trajectory
     *     could not be loaded.
     * @see Choreo#LoadTrajectory(std::string_view)
     */
    static std::optional<Trajectory<SampleType>> LoadTrajectory(
        std::string_view trajectoryName, int splitIndex) {
      std::string key = fmt::format("{}.:.{}", trajectoryName, splitIndex);

      if (!cache.contains(key)) {
        if (cache.contains(std::string{trajectoryName})) {
          cache[key] =
              cache[std::string{trajectoryName}].value().GetSplit(splitIndex);
        } else {
          auto possibleTrajectory = LoadTrajectory(trajectoryName);
          cache[std::string{trajectoryName}] = possibleTrajectory;

          if (possibleTrajectory.has_value()) {
            cache[key] = possibleTrajectory.value().GetSplit(splitIndex);
          }
        }
      }

      return cache[key];
    }

    /**
     * Clears the trajectory cache.
     */
    static void Clear() { cache.clear(); }

   private:
    static inline std::unordered_map<std::string,
                                     std::optional<Trajectory<SampleType>>>
        cache;
  };

 private:
  static constexpr std::string_view TRAJECTORY_FILE_EXTENSION = ".traj";

  static inline const std::string CHOREO_DIR =
      wpi::filesystem::GetDeployDirectory() + "/choreo";

  Choreo();
};

}  // namespace choreo
