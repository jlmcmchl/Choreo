// Copyright (c) Choreo contributors

package choreo.auto;

import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.Alliance;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.command2.Subsystem;
import java.util.Optional;
import java.util.concurrent.atomic.AtomicReference;

public class AutoTestHelper {
  public static AutoFactory factory(
      boolean useAllianceFlipping, AtomicReference<Pose2d> robotPose) {
    // AtomicReference<Pose2d> pose = new AtomicReference<>(new Pose2d());
    return new AutoFactory(
        () -> robotPose.get(),
        newPose -> robotPose.set(newPose),
        sample -> robotPose.set(sample.getPose()),
        useAllianceFlipping,
        new Subsystem() {},
        (sample, isStart) -> {});
  }

  public static AutoFactory factory(boolean useAllianceFlipping) {
    AtomicReference<Pose2d> pose = new AtomicReference<>(new Pose2d());
    return factory(useAllianceFlipping, pose);
  }

  public static AutoFactory factory() {
    return factory(false);
  }

  public static void setAlliance(Optional<Alliance> alliance) {
    var id =
        alliance
            .map(
                all -> {
                  if (all.equals(Alliance.BLUE)) {
                    return AllianceStationID.Blue1;
                  } else {
                    return AllianceStationID.Red1;
                  }
                })
            .orElse(AllianceStationID.Unknown);
    DriverStationSim.setAllianceStationId(id);
    DriverStationSim.notifyNewData();
    DriverStation.refreshData();
  }
}
