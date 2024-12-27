// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Interface between formgps_ui_tracks and frontend tracks stuff. 
// Should be renamed to TracksInterface maybe?
import QtQuick

Item {
    id: tracksInterface

    //Assume trk.model is always updated, although I guess we could call for an update?
    //trk.model is like this:
    /*
    property var model: [
        {index: 0, name: "one", isVisible: true, mode: 2 },
        {index: 1, name: "two", isVisible: false, mode: 4 },
        {index: 2, name: "three", isVisible: true, mode: 64}
    ]*/

    //to select a track, set trk.idx to the index. -1 means tracks are off.

    //modes:
    //AB=2, Curve = 4, waterPivot = 64
    signal tracks_start_new(int mode)
    signal tracks_mark_start(double easting, double northing, double heading)
    //ref_side is negative for left, positive for right, zero for centered
    signal tracks_mark_end(double easting, double northing)
    signal tracks_finish_new(string name)
    signal tracks_cancel_new()
    signal tracks_pause_or_resume(bool pause)
    signal tracks_add_point(double easting, double northing, double heading)

    signal tracks_select(int index)
    signal tracks_swapAB(int index)
    signal tracks_changeName(int index, string new_name)
    signal tracks_delete(int index)

    signal tracks_updateModel()

    //offset and nudge signals
    signal ref_nudge(double dist_m) //negative left, positive right

    signal nudge_zero()
    signal nudge_center()
    signal nudge(double dist_m)

}
