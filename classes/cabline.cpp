#include "cabline.h"
#include "vec2.h"
#include "glm.h"
#include "cvehicle.h"
#include "cboundary.h"
#include "cyouturn.h"
#include "ctram.h"
#include "ccamera.h"
#include "cahrs.h"
#include "cguidance.h"
#include "aogproperty.h"
#include <QOpenGLFunctions>
#include <QColor>
#include "glutils.h"
#include "cnmea.h"
#include "ctrack.h"
#include "cworldgrid.h"

//??? why does CABLine refer to mf.ABLine? Isn't there only one instance and
//thus was can just use "this."  If this is wrong, we'll remove this and fix it.

CABLine::CABLine(QObject *parent) : QObject(parent)
{
    lineWidth = property_setDisplay_lineWidth;
    abLength = 2000;
}

void CABLine::BuildCurrentABLineList(Vec3 pivot,
                                     double secondsSinceStart,
                                     CTrack &trk,
                                     const CYouTurn &yt,
                                     const CVehicle &vehicle)
{
    double tool_width = property_setVehicle_toolWidth;
    double tool_overlap = property_setVehicle_toolOverlap;
    double tool_offset = property_setVehicle_toolOffset;

    double dx, dy;

    int idx = trk.idx;

    abHeading = trk.gArr[idx].heading;

    trk.gArr[idx].endPtA.easting = trk.gArr[idx].ptA.easting - (sin(abHeading) * abLength);
    trk.gArr[idx].endPtA.northing = trk.gArr[idx].ptA.northing - (cos(abHeading) * abLength);

    trk.gArr[idx].endPtB.easting = trk.gArr[idx].ptB.easting + (sin(abHeading) * abLength);
    trk.gArr[idx].endPtB.northing = trk.gArr[idx].ptB.northing + (cos(abHeading) * abLength);

    refNudgePtA = trk.gArr[idx].endPtA; refNudgePtB = trk.gArr[idx].endPtB;

    if (idx > -1 && trk.gArr[idx].nudgeDistance != 0)
    {
        refNudgePtA.easting += (sin(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);
        refNudgePtA.northing += (cos(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);

        refNudgePtB.easting += ( sin(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);
        refNudgePtB.northing += (cos(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);
    }

    lastSecond = secondsSinceStart;

    //move the ABLine over based on the overlap amount set in
    widthMinusOverlap = tool_width - tool_overlap;

    //x2-x1
    dx = refNudgePtB.easting - refNudgePtA.easting;
    //z2-z1
    dy = refNudgePtB.northing - refNudgePtA.northing;

    distanceFromRefLine = ((dy * vehicle.guidanceLookPos.easting) - (dx * vehicle.guidanceLookPos.northing) +
                           (refNudgePtB.easting * refNudgePtA.northing) -
                           (refNudgePtB.northing * refNudgePtA.easting)) /
                          sqrt((dy * dy) + (dx * dx));

    distanceFromRefLine -= (0.5 * widthMinusOverlap);

    isLateralTriggered = false;

    isHeadingSameWay = M_PI - fabs(fabs(pivot.heading - abHeading) - M_PI) < glm::PIBy2;

    if (yt.isYouTurnTriggered) isHeadingSameWay = !isHeadingSameWay;

    //Which ABLine is the vehicle on, negative is left and positive is right side
    double RefDist = (distanceFromRefLine + (isHeadingSameWay ? tool_offset : -tool_offset)) / widthMinusOverlap;

    if (RefDist < 0) howManyPathsAway = (int)(RefDist - 0.5);
    else howManyPathsAway = (int)(RefDist + 0.5);

    double distAway = widthMinusOverlap * howManyPathsAway;
    distAway += (0.5 * widthMinusOverlap);

    shadowOffset = isHeadingSameWay ? tool_offset : -tool_offset;

    //move the curline as well.
    Vec2 nudgePtA(trk.gArr[idx].ptA);
    Vec2 nudgePtB(trk.gArr[idx].ptB);

    nudgePtA.easting += (sin(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);
    nudgePtA.northing += (cos(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);

    nudgePtB.easting += (sin(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);
    nudgePtB.northing += (cos(abHeading + glm::PIBy2) * trk.gArr[idx].nudgeDistance);

    //depending which way you are going, the offset can be either side
    Vec2 point1((cos(-abHeading) * (distAway + (isHeadingSameWay ? -tool_offset : tool_offset))) + nudgePtA.easting,
                (sin(-abHeading) * (distAway + (isHeadingSameWay ? -tool_offset : tool_offset))) + nudgePtA.northing);

    Vec2 point2((cos(-abHeading) * (distAway + (isHeadingSameWay ? -tool_offset : tool_offset))) + nudgePtB.easting,
                (sin(-abHeading) * (distAway + (isHeadingSameWay ? -tool_offset : tool_offset))) + nudgePtB.northing);


    //create the new line extent points for current ABLine based on original heading of AB line
    currentLinePtA.easting = point1.easting - (sin(abHeading) * abLength);
    currentLinePtA.northing = point1.northing - (cos(abHeading) * abLength);

    currentLinePtB.easting = point2.easting + (sin(abHeading) * abLength);
    currentLinePtB.northing = point2.northing + (cos(abHeading) * abLength);

    currentLinePtA.heading = abHeading;
    currentLinePtB.heading = abHeading;

    isABValid = true;
    if (howManyPathsAway > -1) howManyPathsAway += 1;

}

void CABLine::GetCurrentABLine(Vec3 pivot, Vec3 steer,
                               double secondsSinceStart,
                               bool isAutoSteerBtnOn,
                               bool steerSwitchHigh,
                               CTrack &trk,
                               CVehicle &vehicle,
                               CYouTurn &yt,
                               const CAHRS &ahrs,
                               CGuidance &gyd,
                               CNMEA &pn)
{
    double dx, dy;
    double purePursuitIntegralGain = property_purePursuitIntegralGainAB;
    double wheelBase = property_setVehicle_wheelbase;
    double maxSteerAngle = property_setVehicle_maxSteerAngle;

    //build new current ref line if required
    if (!isABValid || ((secondsSinceStart - lastSecond) > 0.66
                       && (!isAutoSteerBtnOn || steerSwitchHigh)))
        BuildCurrentABLineList(pivot,secondsSinceStart,trk,yt,vehicle);

    //Check uturn first
    if (yt.isYouTurnTriggered && yt.DistanceFromYouTurnLine(vehicle,pn))//do the pure pursuit from youTurn
    {
        //now substitute what it thinks are AB line values with auto turn values
        steerAngleAB = yt.steerAngleYT;
        distanceFromCurrentLinePivot = yt.distanceFromCurrentLine;

        goalPointAB = yt.goalPointYT;
        radiusPointAB.easting = yt.radiusPointYT.easting;
        radiusPointAB.northing = yt.radiusPointYT.northing;
        ppRadiusAB = yt.ppRadiusYT;

        vehicle.modeTimeCounter = 0;
        vehicle.modeActualXTE = (distanceFromCurrentLinePivot);
    }

    //Stanley
    else if (property_setVehicle_isStanleyUsed)
        gyd.StanleyGuidanceABLine(currentLinePtA, currentLinePtB, pivot, steer, isAutoSteerBtnOn, vehicle,*this, ahrs,yt);

    //Pure Pursuit
    else
    {
        //get the distance from currently active AB line
        //x2-x1
        dx = currentLinePtB.easting - currentLinePtA.easting;
        //z2-z1
        dy = currentLinePtB.northing - currentLinePtA.northing;

        //save a copy of dx,dy in youTurn
        yt.dxAB = dx; yt.dyAB = dy;

        //how far from current AB Line is fix
        distanceFromCurrentLinePivot = ((dy * pivot.easting) - (dx * pivot.northing) +
                                        (currentLinePtB.easting * currentLinePtA.northing) -
                                        (currentLinePtB.northing * currentLinePtA.easting))
                                       / sqrt((dy * dy) + (dx * dx));

        //integral slider is set to 0
        if (purePursuitIntegralGain != 0 && !vehicle.isReverse)
        {
            pivotDistanceError = distanceFromCurrentLinePivot * 0.2 + pivotDistanceError * 0.8;

            if (counter2++ > 4)
            {
                pivotDerivative = pivotDistanceError - pivotDistanceErrorLast;
                pivotDistanceErrorLast = pivotDistanceError;
                counter2 = 0;
                pivotDerivative *= 2;

                //limit the derivative
                //if (pivotDerivative > 0.03) pivotDerivative = 0.03;
                //if (pivotDerivative < -0.03) pivotDerivative = -0.03;
                //if (fabs(pivotDerivative) < 0.01) pivotDerivative = 0;
            }

            //pivotErrorTotal = pivotDistanceError + pivotDerivative;

            if (isAutoSteerBtnOn
                && fabs(pivotDerivative) < (0.1)
                && vehicle.avgSpeed > 2.5
                && !yt.isYouTurnTriggered)
            //&& fabs(pivotDistanceError) < 0.2)

            {
                //if over the line heading wrong way, rapidly decrease integral
                if ((inty < 0 && distanceFromCurrentLinePivot < 0) || (inty > 0 && distanceFromCurrentLinePivot > 0))
                {
                    inty += pivotDistanceError * purePursuitIntegralGain * -0.04;
                }
                else
                {
                    if (fabs(distanceFromCurrentLinePivot) > 0.02)
                    {
                        inty += pivotDistanceError * purePursuitIntegralGain * -0.02;
                        if (inty > 0.2) inty = 0.2;
                        else if (inty < -0.2) inty = -0.2;
                    }
                }
            }
            else inty *= 0.95;
        }
        else inty = 0;

        //Subtract the two headings, if > 1.57 its going the opposite heading as refAB
        abFixHeadingDelta = (fabs(vehicle.fixHeading - abHeading));
        if (abFixHeadingDelta >= M_PI) abFixHeadingDelta = fabs(abFixHeadingDelta - glm::twoPI);

        // ** Pure pursuit ** - calc point on ABLine closest to current position
        double U = (((pivot.easting - currentLinePtA.easting) * dx)
                    + ((pivot.northing - currentLinePtA.northing) * dy))
                   / ((dx * dx) + (dy * dy));

        //point on AB line closest to pivot axle point
        rEastAB = currentLinePtA.easting + (U * dx);
        rNorthAB = currentLinePtA.northing + (U * dy);

        //update base on autosteer settings and distance from line
        double goalPointDistance = vehicle.UpdateGoalPointDistance();

        if (vehicle.isReverse ? isHeadingSameWay : !isHeadingSameWay)
        {
            goalPointAB.easting = rEastAB - (sin(abHeading) * goalPointDistance);
            goalPointAB.northing = rNorthAB - (cos(abHeading) * goalPointDistance);
        }
        else
        {
            goalPointAB.easting = rEastAB + (sin(abHeading) * goalPointDistance);
            goalPointAB.northing = rNorthAB + (cos(abHeading) * goalPointDistance);
        }

        //calc "D" the distance from pivot axle to lookahead point
        double goalPointDistanceDSquared
            = glm::DistanceSquared(goalPointAB.northing, goalPointAB.easting, pivot.northing, pivot.easting);

        //calculate the the new x in local coordinates and steering angle degrees based on wheelbase
        double localHeading;

        if (isHeadingSameWay) localHeading = glm::twoPI - vehicle.fixHeading + inty;
        else localHeading = glm::twoPI - vehicle.fixHeading - inty;

        ppRadiusAB = goalPointDistanceDSquared / (2 * (((goalPointAB.easting - pivot.easting) * cos(localHeading))
                                                       + ((goalPointAB.northing - pivot.northing) * sin(localHeading))));

        steerAngleAB = glm::toDegrees(atan(2 * (((goalPointAB.easting - pivot.easting) * cos(localHeading))
                                                    + ((goalPointAB.northing - pivot.northing) * sin(localHeading))) * wheelBase
                                               / goalPointDistanceDSquared));

        if (ahrs.imuRoll != 88888)
            steerAngleAB += ahrs.imuRoll * -(double)property_setAS_sideHillComp; /*mf.gyd.sideHillCompFactor;*/

        //steerAngleAB *= 1.4;

        if (steerAngleAB < -maxSteerAngle) steerAngleAB = -maxSteerAngle;
        if (steerAngleAB > maxSteerAngle) steerAngleAB = maxSteerAngle;

        //limit circle size for display purpose
        if (ppRadiusAB < -500) ppRadiusAB = -500;
        if (ppRadiusAB > 500) ppRadiusAB = 500;

        radiusPointAB.easting = pivot.easting + (ppRadiusAB * cos(localHeading));
        radiusPointAB.northing = pivot.northing + (ppRadiusAB * sin(localHeading));

        //if (mf.isConstantContourOn)
        //{
        //    //angular velocity in rads/sec  = 2PI * m/sec * radians/meters

        //    //clamp the steering angle to not exceed safe angular velocity
        //    if famf.setAngVel) > 1000)
        //    {
        //        //mf.setAngVel = mf.setAngVel < 0 ? -mf.vehicle.maxAngularVelocity : mf.vehicle.maxAngularVelocity;
        //        mf.setAngVel = mf.setAngVel < 0 ? -1000 : 1000;
        //    }
        //}

        //distance is negative if on left, positive if on right
        if (!isHeadingSameWay)
            distanceFromCurrentLinePivot *= -1.0;

        //used for acquire/hold mode
        vehicle.modeActualXTE = (distanceFromCurrentLinePivot);

        double steerHeadingError = (pivot.heading - abHeading);
        //Fix the circular error
        if (steerHeadingError > M_PI)
            steerHeadingError -= M_PI;
        else if (steerHeadingError < -M_PI)
            steerHeadingError += M_PI;

        if (steerHeadingError > glm::PIBy2)
            steerHeadingError -= M_PI;
        else if (steerHeadingError < -glm::PIBy2)
            steerHeadingError += M_PI;

        vehicle.modeActualHeadingError = glm::toDegrees(steerHeadingError);

        //Convert to millimeters
        vehicle.guidanceLineDistanceOff = (short)glm::roundMidAwayFromZero(distanceFromCurrentLinePivot * 1000.0);
        vehicle.guidanceLineSteerAngle = (short)(steerAngleAB * 100);
    }

    //mf.setAngVel = 0.277777 * mf.avgSpeed * (tan(glm::toRadians(steerAngleAB))) / mf.vehicle.wheelbase;
    //mf.setAngVel = glm::toDegrees(mf.setAngVel);
}

void CABLine::DrawABLineNew(QOpenGLFunctions *gl, const QMatrix4x4 &mvp, const CCamera &camera)
{
    GLHelperOneColor gldraw;

    //ABLine currently being designed
    gldraw.append(QVector3D(desLineEndA.easting, desLineEndA.northing, 0.0));
    gldraw.append(QVector3D(desLineEndB.easting, desLineEndB.northing, 0.0));
    gldraw.draw(gl, mvp, QColor::fromRgbF(0.95f, 0.70f, 0.50f),GL_LINES, lineWidth);

    drawText3D(camera, gl,mvp, desPtA.easting, desPtA.northing, "&A", 1.0, true, QColor::fromRgbF(0.2f, 0.950f, 0.20f));
    drawText3D(camera, gl,mvp, desPtB.easting, desPtB.northing, "&B", 1.0, true, QColor::fromRgbF(0.2f, 0.950f, 0.20f));
}

void CABLine::DrawABLines(QOpenGLFunctions *gl, const QMatrix4x4 &mvp,
                          bool isFontOn,
                          CTrack &trk,
                          CYouTurn &yt,
                          const CWorldGrid &worldGrid,
                          const CCamera &camera,
                          const CGuidance &gyd)
{
    double tool_toolWidth = property_setVehicle_toolWidth;
    double tool_toolOverlap = property_setVehicle_toolOverlap;
    double tool_toolOffset = property_setVehicle_toolOffset;
    bool isStanleyUsed = property_setVehicle_isStanleyUsed;
    bool isSideGuideLines = property_setMenu_isSideGuideLines;
    widthMinusOverlap = tool_toolWidth - tool_toolOverlap;

    GLHelperOneColor gldraw;
    GLHelperColors gldraw1;
    ColorVertex cv;
    QColor color;

    double lineWidth = property_setDisplay_lineWidth;

    //Draw AB Points
    cv.color = QVector4D(0.95f, 0.0f, 0.0f, 1.0);
    cv.vertex = QVector3D(trk.gArr[trk.idx].ptB.easting, trk.gArr[trk.idx].ptB.northing, 0.0);
    gldraw1.append(cv);
    cv.color = QVector4D(0.0f, 0.90f, 0.95f, 1.0);
    cv.vertex = QVector3D(trk.gArr[trk.idx].ptA.easting, trk.gArr[trk.idx].ptA.northing, 0.0);
    gldraw1.append(cv);
    //cv.color = QVector4D(0.00990f, 0.990f, 0.05f,1.0);
    //cv.vertex = QVector3D(bnd.iE, bnd.iN, 0.0);
    //gldraw1.append(cv);
    gldraw1.draw(gl,mvp,GL_POINTS,8.0f);

    if (isFontOn && !isMakingABLine)
    {
        color.setRgbF(0.00990f, 0.990f, 0.095f);
        drawText3D(camera,gl,mvp, trk.gArr[trk.idx].ptA.easting, trk.gArr[trk.idx].ptA.northing, "&A", 1.0, true, color);
        drawText3D(camera,gl,mvp, trk.gArr[trk.idx].ptB.easting, trk.gArr[trk.idx].ptB.northing, "&B", 1.0, true, color);
    }

    //Draw reference AB line
    color.setRgbF(0.930f, 0.2f, 0.2f);
    gldraw.append(QVector3D(trk.gArr[trk.idx].endPtA.easting, trk.gArr[trk.idx].endPtA.northing, 0));
    gldraw.append(QVector3D(trk.gArr[trk.idx].endPtB.easting, trk.gArr[trk.idx].endPtB.northing, 0));
    gldraw.draw(gl,mvp,color,GL_LINES,4);


    if (worldGrid.isRateMap)
    {
        double sinHR = sin(abHeading + glm::PIBy2) * (widthMinusOverlap * 0.5 + shadowOffset);
        double cosHR = cos(abHeading + glm::PIBy2) * (widthMinusOverlap * 0.5 + shadowOffset);
        double sinHL = sin(abHeading + glm::PIBy2) * (widthMinusOverlap * 0.5 - shadowOffset);
        double cosHL = cos(abHeading + glm::PIBy2) * (widthMinusOverlap * 0.5 - shadowOffset);

        //shadow
        color.setRgbF(0.5, 0.5, 0.5, 0.3);

        gldraw.clear();
        gldraw.append(QVector3D(currentLinePtA.easting - sinHL, currentLinePtA.northing - cosHL, 0));
        gldraw.append(QVector3D(currentLinePtA.easting + sinHR, currentLinePtA.northing + cosHR, 0));
        gldraw.append(QVector3D(currentLinePtB.easting + sinHR, currentLinePtB.northing + cosHR, 0));
        gldraw.append(QVector3D(currentLinePtB.easting - sinHL, currentLinePtB.northing - cosHR, 0));

        gldraw.draw(gl,mvp,color,GL_TRIANGLE_FAN,lineWidth);

        //shadow lines
        color.setRgbF(0.55, 0.55, 0.55, 0.3);
        //use the same vertices as the shadow
        gldraw.draw(gl,mvp,color,GL_LINE_LOOP,1.0f);
    }

    //draw current AB Line
    gldraw.clear();
    color.setRgbF(0.95f, 0.20f, 0.950f);

    gldraw.append(QVector3D(currentLinePtA.easting, currentLinePtA.northing, 0.0));
    gldraw.append(QVector3D(currentLinePtB.easting, currentLinePtB.northing, 0.0));
    gldraw.draw(gl,mvp,color,GL_LINES,lineWidth);

    if (isSideGuideLines && camera.camSetDistance > tool_toolWidth * -120)
    {
        //get the tool offset and width
        double toolOffset = tool_toolOffset * 2;
        double toolWidth = tool_toolWidth - tool_toolOverlap;
        double cosHeading = cos(-abHeading);
        double sinHeading = sin(-abHeading);

        color.setRgbF(0.756f, 0.7650f, 0.7650f);
        gldraw.clear();

        /*
                for (double i = -2.5; i < 3; i++)
                {
                    GL.Vertex3((cosHeading * ((mf.tool.toolWidth - mf.tool.toolOverlap) * (howManyPathsAway + i))) + refPoint1.easting, (sinHeading * ((mf.tool.toolWidth - mf.tool.toolOverlap) * (howManyPathsAway + i))) + refPoint1.northing, 0);
                    GL.Vertex3((cosHeading * ((mf.tool.toolWidth - mf.tool.toolOverlap) * (howManyPathsAway + i))) + refPoint2.easting, (sinHeading * ((mf.tool.toolWidth - mf.tool.toolOverlap) * (howManyPathsAway + i))) + refPoint2.northing, 0);
                }
                */

        if (isHeadingSameWay)
        {
            gldraw.append(QVector3D((cosHeading * (toolWidth + toolOffset)) + currentLinePtA.easting, (sinHeading * (toolWidth + toolOffset)) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * (toolWidth + toolOffset)) + currentLinePtB.easting, (sinHeading * (toolWidth + toolOffset)) + currentLinePtB.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth + toolOffset)) + currentLinePtA.easting, (sinHeading * (-toolWidth + toolOffset)) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth + toolOffset)) + currentLinePtB.easting, (sinHeading * (-toolWidth + toolOffset)) + currentLinePtB.northing, 0));

            toolWidth *= 2;
            gldraw.append(QVector3D((cosHeading * toolWidth) + currentLinePtA.easting, (sinHeading * toolWidth) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * toolWidth) + currentLinePtB.easting, (sinHeading * toolWidth) + currentLinePtB.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth)) + currentLinePtA.easting, (sinHeading * (-toolWidth)) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth)) + currentLinePtB.easting, (sinHeading * (-toolWidth)) + currentLinePtB.northing, 0));
        }

        else
        {
            gldraw.append(QVector3D((cosHeading * (toolWidth - toolOffset)) + currentLinePtA.easting, (sinHeading * (toolWidth - toolOffset)) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * (toolWidth - toolOffset)) + currentLinePtB.easting, (sinHeading * (toolWidth - toolOffset)) + currentLinePtB.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth - toolOffset)) + currentLinePtA.easting, (sinHeading * (-toolWidth - toolOffset)) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth - toolOffset)) + currentLinePtB.easting, (sinHeading * (-toolWidth - toolOffset)) + currentLinePtB.northing, 0));

            toolWidth *= 2;
            gldraw.append(QVector3D((cosHeading * toolWidth) + currentLinePtA.easting, (sinHeading * toolWidth) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * toolWidth) + currentLinePtB.easting, (sinHeading * toolWidth) + currentLinePtB.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth)) + currentLinePtA.easting, (sinHeading * (-toolWidth)) + currentLinePtA.northing, 0));
            gldraw.append(QVector3D((cosHeading * (-toolWidth)) + currentLinePtB.easting, (sinHeading * (-toolWidth)) + currentLinePtB.northing, 0));
        }

        gldraw.draw(gl,mvp,color,GL_LINES,lineWidth);
    }

    if (!isStanleyUsed && camera.camSetDistance > -200)
    {
        //Draw lookahead Point
        gldraw.clear();
        color.setRgbF(1.0f, 1.0f, 0.0f);
        gldraw.append(QVector3D(goalPointAB.easting, goalPointAB.northing, 0.0));
        gldraw.append(QVector3D(gyd.rEastSteer, gyd.rNorthSteer, 0.0));
        gldraw.append(QVector3D(gyd.rEastPivot, gyd.rNorthPivot, 0.0));
        gldraw.draw(gl,mvp,color,GL_POINTS,8.0f);

        if (ppRadiusAB < 50 && ppRadiusAB > -50)
        {
            const int numSegments = 200;
            double theta = glm::twoPI / numSegments;
            double c = cos(theta);//precalculate the sine and cosine
            double s = sin(theta);
            //double x = ppRadiusAB;//we start at angle = 0
            double x = 0;//we start at angle = 0
            double y = 0;

            gldraw.clear();
            color.setRgbF(0.53f, 0.530f, 0.950f);
            for (int ii = 0; ii < numSegments - 15; ii++)
            {
                //glVertex2f(x + cx, y + cy);//output vertex
                gldraw.append(QVector3D(x + radiusPointAB.easting, y + radiusPointAB.northing, 0));//output vertex
                double t = x;//apply the rotation matrix
                x = (c * x) - (s * y);
                y = (s * t) + (c * y);
            }
            gldraw.draw(gl,mvp,color,GL_LINES,2.0f);
        }
    }

    yt.DrawYouTurn(gl,mvp);
}

void CABLine::BuildTram(CTrack &trk, CBoundary &bnd, CTram &tram)
{
    double tramWidth = property_setTram_tramWidth;
    double tool_halfWidth = ((double)property_setVehicle_toolWidth - (double)property_setVehicle_toolOverlap) / 2.0;
    double halfWheelTrack = (double)property_setVehicle_trackWidth * 0.5;

    if (tram.generateMode != 1)
    {
        tram.BuildTramBnd(bnd);
    }
    else
    {
        tram.tramBndOuterArr.clear();
        tram.tramBndInnerArr.clear();
    }

    tram.tramList.clear();
    tram.tramArr->clear();

    if (tram.generateMode == 2) return;

    QVector<Vec2> tramRef;

    bool isBndExist = bnd.bndList.count() != 0;

    abHeading = trk.gArr[trk.idx].heading;

    double hsin = sin(abHeading);
    double hcos = cos(abHeading);

    double len = glm::Distance(trk.gArr[trk.idx].endPtA, trk.gArr[trk.idx].endPtB);
    //divide up the AB line into segments
    Vec2 P1;
    for (int i = 0; i < (int)len; i += 4)
    {
        P1.easting = (hsin * i) + trk.gArr[trk.idx].endPtA.easting;
        P1.northing = (hcos * i) + trk.gArr[trk.idx].endPtA.northing;
        tramRef.append(P1);
    }

    //create list of list of points of triangle strip of AB Highlight
    double headingCalc = abHeading + glm::PIBy2;

    hsin = sin(headingCalc);
    hcos = cos(headingCalc);

    tram.tramList.clear();
    tram.tramArr->clear();

    //no boundary starts on first pass
    int cntr = 0;
    if (isBndExist)
    {
        if (tram.generateMode == 1)
            cntr = 0;
        else
            cntr = 1;
    }

    double widd = 0;
    for (int i = cntr; i < (int)property_setTram_passes; i++)
    {
        tram.tramArr = QSharedPointer<QVector<Vec2>>(new QVector<Vec2>());
        tram.tramList.append(tram.tramArr);

        widd = (tramWidth * 0.5) - tool_halfWidth - halfWheelTrack;
        widd += (tramWidth * i);

        for (int j = 0; j < tramRef.count(); j++)
        {
            P1.easting = hsin * widd + tramRef[j].easting;
            P1.northing = (hcos * widd) + tramRef[j].northing;

            if (!isBndExist || glm::IsPointInPolygon(bnd.bndList[0].fenceLineEar,P1))
            {
                tram.tramArr->append(P1);
            }
        }
    }

    for (int i = cntr; i < (int)property_setTram_passes; i++)
    {
        tram.tramArr = QSharedPointer<QVector<Vec2>>(new QVector<Vec2>());
        tram.tramList.append(tram.tramArr);

        widd = (tramWidth * 0.5) - tool_halfWidth + halfWheelTrack;
        widd += (tramWidth * i);

        for (int j = 0; j < tramRef.count(); j++)
        {
            P1.easting = (hsin * widd) + tramRef[j].easting;
            P1.northing = (hcos * widd) + tramRef[j].northing;

            if (!isBndExist || glm::IsPointInPolygon(bnd.bndList[0].fenceLineEar,P1))
            {
                tram.tramArr->append(P1);
            }
        }
    }

    tramRef.clear();
    //outside tram

    if (bnd.bndList.count() == 0 || (int)property_setTram_passes != 0)
    {
        //return;
    }
}
