#include "cboundarylist.h"
#include "glm.h"

/*****************
 * CTurnLines.cs *
 *****************/
void CBoundaryList::CalculateTurnHeadings()
{
    //to calc heading based on next and previous points to give an average heading.
    int cnt = turnLine.count();
    QVector<Vec3> arr = turnLine;
    cnt--;
    turnLine.clear();

    //first point needs last, first, second points
    Vec3 pt3(arr[0]);
    pt3.heading = atan2(arr[1].easting - arr[cnt].easting,
                        arr[1].northing - arr[cnt].northing);
    if (pt3.heading < 0) pt3.heading += glm::twoPI;
    turnLine.append(pt3);

    //middle points
    for (int i = 1; i < cnt; i++)
    {
        pt3 = arr[i];
        pt3.heading = atan2(arr[i + 1].easting - arr[i - 1].easting,
                            arr[i + 1].northing - arr[i - 1].northing);
        if (pt3.heading < 0) pt3.heading += glm::twoPI;
        turnLine.append(pt3);
    }

    //first point
    Vec3 pt2 = arr[0];
    pt2.heading = atan2(arr[1].easting - arr[0].easting, arr[1].northing - arr[0].northing);
    if (pt2.heading < 0) pt2.heading += glm::twoPI;
    turnLine.insert(0, pt2);

    //last point
    pt2 = arr[arr.count() - 1];
    pt2.heading = atan2(arr[arr.count() - 1].easting - arr[arr.count() - 2].easting,
                        arr[arr.count() - 1].northing - arr[arr.count() - 2].northing);
    if (pt2.heading < 0) pt2.heading += glm::twoPI;
    turnLine.append(pt2);
}

void CBoundaryList::FixTurnLine(double totalHeadWidth, double spacing)
{
    //count the points from the boundary
    int lineCount = turnLine.count();

    totalHeadWidth *= totalHeadWidth;
    spacing *= spacing;

    //int headCount = mf.bndArr[inTurnNum].bndLine.Count;
    double distance;

    //remove the points too close to boundary
    for (int i = 0; i < fenceLine.count(); i++)
    {
        for (int j = 0; j < lineCount; j++)
        {
            //make sure distance between headland and boundary is not less then width
            distance = glm::DistanceSquared(fenceLine[i], turnLine[j]);
            if (distance < (totalHeadWidth * 0.99))
            {
                turnLine.removeAt(j);
                lineCount = turnLine.count();
                j = -1;
            }
        }
    }

    //make sure distance isn't too big between points on Turn
    int bndCount = turnLine.count();
    for (int i = 0; i < bndCount; i++)
    {
        int j = i + 1;
        if (j == bndCount) j = 0;
        distance = glm::DistanceSquared(turnLine[i], turnLine[j]);
        if (distance > (spacing * 1.8))
        {
            Vec3 pointB((turnLine[i].easting + turnLine[j].easting) / 2.0,
                        (turnLine[i].northing + turnLine[j].northing) / 2.0,
                        turnLine[i].heading);

            turnLine.insert(j, pointB);
            bndCount = turnLine.count();
            i--;
        }
    }

    //make sure distance isn't too small between points on turnLine
    bndCount = turnLine.count();
    for (int i = 0; i < bndCount - 1; i++)
    {
        distance = glm::DistanceSquared(turnLine[i], turnLine[i + 1]);
        if (distance < spacing)
        {
            turnLine.removeAt(i + 1);
            bndCount = turnLine.count();
            i--;
        }
    }

    //make sure headings are correct for calculated points
    if (turnLine.count() > 0)
    {
        CalculateTurnHeadings();
    }

}
