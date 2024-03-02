#include "ctrack.h"
#include "cvehicle.h"
#include "cabcurve.h"
#include "cabline.h"
#include "glm.h"

int CTrack::FindClosestRefTrack(Vec3 pivot, const CVehicle &vehicle)
{
    if (idx < 0 || gArr.count() == 0) return -1;

    //only 1 track
    if (gArr.count() == 1) return idx;

    int trak = -1;
    int cntr = 0;

    //Count visible
    for (int i = 0; i < gArr.count(); i++)
    {
        if (gArr[i].isVisible)
        {
            cntr++;
            trak = i;
        }
    }

    //only 1 track visible of the group
    if (cntr == 1) return trak;

    //no visible tracks
    if (cntr == 0) return -1;

    //determine if any aligned reasonably close
    QVector<bool> isAlignedArr(gArr.count());
    isAlignedArr.fill(false);

    for (int i = 0; i < gArr.count(); i++)
    {
        if (gArr[i].mode == (int)TrackMode::Curve) isAlignedArr[i] = true;
        else
        {
            double diff = M_PI - fabs(fabs(pivot.heading - gArr[i].heading) - M_PI);
            if ((diff < 0.6) || (diff > 2.54))
                isAlignedArr[i] = true;
            else
                isAlignedArr[i] = false;
        }
    }

    double minDistA = glm::DOUBLE_MAX;
    double dist;

    Vec2 endPtA, endPtB;

    for (int i = 0; i < gArr.count(); i++)
    {
        if (!isAlignedArr[i]) continue;
        if (!gArr[i].isVisible) continue;

        if (gArr[i].mode == (int)TrackMode::AB)
        {
            double abHeading = gArr[i].heading;

            endPtA.easting = gArr[i].ptA.easting - (sin(abHeading) * 2000);
            endPtA.northing = gArr[i].ptA.northing - (cos(abHeading) * 2000);

            endPtB.easting = gArr[i].ptB.easting + (sin(abHeading) * 2000);
            endPtB.northing = gArr[i].ptB.northing + (cos(abHeading) * 2000);

            //x2-x1
            double dx = endPtB.easting - endPtA.easting;
            //z2-z1
            double dy = endPtB.northing - endPtA.northing;

            dist = ((dy * vehicle.steerAxlePos.easting) - (dx * vehicle.steerAxlePos.northing) +
                    (endPtB.easting * endPtA.northing) - (endPtB.northing * endPtA.easting))
                   / sqrt((dy * dy) + (dx * dx));

            dist *= dist;

            if (dist < minDistA)
            {
                minDistA = dist;
                trak = i;
            }
        }
        else
        {
            for (int j = 0; j < gArr[i].curvePts.count(); j ++)
            {

                dist = glm::DistanceSquared(gArr[i].curvePts[j], pivot);

                if (dist < minDistA)
                {
                    minDistA = dist;
                    trak = i;
                }
            }
        }
    }

    return trak;
}

void CTrack::NudgeTrack(double dist, CABLine &ABLine, CABCurve &curve)
{
    if (idx > -1)
    {
        if (gArr[idx].mode == (int)TrackMode::AB)
        {
            ABLine.isABValid = false;
            ABLine.lastSecond = 0;
            gArr[idx].nudgeDistance += ABLine.isHeadingSameWay ? dist : -dist;
        }
        else
        {
            curve.isCurveValid = false;
            curve.lastSecond = 0;
            gArr[idx].nudgeDistance += curve.isHeadingSameWay ? dist : -dist;

        }

        //if (gArr[idx].nudgeDistance > 0.5 * mf.tool.width) gArr[idx].nudgeDistance -= mf.tool.width;
        //else if (gArr[idx].nudgeDistance < -0.5 * mf.tool.width) gArr[idx].nudgeDistance += mf.tool.width;
    }
}

void CTrack::NudgeDistanceReset(CABLine &ABLine, CABCurve &curve)
{
    if (idx > -1 && gArr.count() > 0)
    {
        if (gArr[idx].mode == (int)TrackMode::AB)
        {
            ABLine.isABValid = false;
            ABLine.lastSecond = 0;
        }
        else
        {
            curve.isCurveValid = false;
            curve.lastSecond = 0;
        }

        gArr[idx].nudgeDistance = 0;
    }
}

void CTrack::SnapToPivot(CABLine &ABLine, CABCurve &curve)
{
    //if (isBtnGuidanceOn)

    if (idx > -1)
    {
        if (gArr[idx].mode == (int)(TrackMode::AB))
        {
            NudgeTrack(ABLine.distanceFromCurrentLinePivot, ABLine, curve);

        }
        else
        {
            NudgeTrack(curve.distanceFromCurrentLinePivot, ABLine, curve);
        }

    }
}

void CTrack::NudgeRefTrack(double dist, CABLine &ABLine, CABCurve &curve)
{

    if (idx > -1)
    {
        if (gArr[idx].mode == (int)TrackMode::AB)
        {
            ABLine.isABValid = false;
            ABLine.lastSecond = 0;
            NudgeRefABLine( ABLine.isHeadingSameWay ? dist : -dist);
        }
        else
        {
            curve.isCurveValid = false;
            curve.lastSecond = 0;
            NudgeRefCurve( curve.isHeadingSameWay ? dist : -dist, curve);
        }
    }
}

void CTrack::NudgeRefABLine(double dist)
{
    double head = gArr[idx].heading;

    gArr[idx].ptA.easting += (sin(head+glm::PIBy2) * (dist));
    gArr[idx].ptA.northing += (cos(head + glm::PIBy2) * (dist));

    gArr[idx].ptB.easting += (sin(head + glm::PIBy2) * (dist));
    gArr[idx].ptB.northing += (cos(head + glm::PIBy2) * (dist));
}

void CTrack::NudgeRefCurve(double distAway, CABCurve &curve)
{
    curve.isCurveValid = false;
    curve.lastSecond = 0;

    QVector<Vec3> curList;

    double distSqAway = (distAway * distAway) - 0.01;
    Vec3 point;

    for (int i = 0; i < gArr[idx].curvePts.count(); i++)
    {
        Vec3 point(
            gArr[idx].curvePts[i].easting + (sin(glm::PIBy2 + gArr[idx].curvePts[i].heading) * distAway),
            gArr[idx].curvePts[i].northing + (cos(glm::PIBy2 + gArr[idx].curvePts[i].heading) * distAway),
            gArr[idx].curvePts[i].heading);
        bool Add = true;

        for (int t = 0; t < gArr[idx].curvePts.count(); t++)
        {
            double dist = ((point.easting - gArr[idx].curvePts[t].easting) * (point.easting - gArr[idx].curvePts[t].easting))
                          + ((point.northing - gArr[idx].curvePts[t].northing) * (point.northing - gArr[idx].curvePts[t].northing));
            if (dist < distSqAway)
            {
                Add = false;
                break;
            }
        }

        if (Add)
        {
            if (curList.count() > 0)
            {
                double dist = ((point.easting - curList[curList.count() - 1].easting) * (point.easting - curList[curList.count() - 1].easting))
                              + ((point.northing - curList[curList.count() - 1].northing) * (point.northing - curList[curList.count() - 1].northing));
                if (dist > 1.0)
                    curList.append(point);
            }
            else curList.append(point);
        }
    }

    int cnt = curList.count();
    if (cnt > 6)
    {
        QVector<Vec3> arr = curList;

        curList.clear();

        for (int i = 0; i < (arr.count() - 1); i++)
        {
            arr[i].heading = atan2(arr[i + 1].easting - arr[i].easting, arr[i + 1].northing - arr[i].northing);
            if (arr[i].heading < 0) arr[i].heading += glm::twoPI;
            if (arr[i].heading >= glm::twoPI) arr[i].heading -= glm::twoPI;
        }

        arr[arr.count() - 1].heading = arr[arr.count() - 2].heading;

        //replace the array
        cnt = arr.count();
        double distance;
        double spacing = 1.2;

        //add the first point of loop - it will be p1
        curList.append(arr[0]);

        for (int i = 0; i < cnt - 3; i++)
        {
            // add p2
            curList.append(arr[i + 1]);

            distance = glm::Distance(arr[i + 1], arr[i + 2]);

            if (distance > spacing)
            {
                int loopTimes = (int)(distance / spacing + 1);
                for (int j = 1; j < loopTimes; j++)
                {
                    Vec3 pos(glm::Catmull(j / (double)(loopTimes), arr[i], arr[i + 1], arr[i + 2], arr[i + 3]));
                    curList.append(pos);
                }
            }
        }

        curList.append(arr[cnt - 2]);
        curList.append(arr[cnt - 1]);

        CalculateHeadings(curList);

        gArr[idx].curvePts.clear();

        for (auto item: curList)
        {
            gArr[idx].curvePts.append(item);
        }

        //for (int i = 0; i < cnt; i++)
        //{
        //    arr[i].easting += cos(arr[i].heading) * (dist);
        //    arr[i].northing -= sin(arr[i].heading) * (dist);
        //    gArr[idx].curvePts.append(arr[i]);
        //}
    }
}
