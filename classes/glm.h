#ifndef GLM_H
#define GLM_H

#include <math.h>
#include <limits>
#include <QVector>
#include "vec2.h"
#include "vecfix2fix.h"
#include "vec3.h"


#ifndef M_PI
#  ifndef PI
#     define M_PI 3.14159265358979323846
#  else
#     define M_PI PI
#  endif
#endif

namespace glm {
    static const double DOUBLE_EPSILON=std::numeric_limits<double>::epsilon();

    static const double DOUBLE_MAX = std::numeric_limits<double>::max();
    static const double DOUBLE_MIN = std::numeric_limits<double>::min();
    static const double FLOAT_MAX = std::numeric_limits<float>::max();
    static const double FLOAT_MIN = std::numeric_limits<float>::min();

    static const uchar BYTE_MAX = std::numeric_limits<uchar>::max();
    static const short SHORT_MAX = std::numeric_limits<short>::max();
    static const ushort USHORT_MAX = std::numeric_limits<ushort>::max();
    static const int INT_MAX = std::numeric_limits<int>::max();

    //inches to meters
    static const double in2m = 0.0254;

    //meters to inches
    static const double m2in = 39.3701;

    //meters to feet
    static const double m2ft = 3.28084;

    //feet to meters
    static const double ft2m = 0.3048;

    //the pi's
    static const double twoPI = 6.28318530717958647692;

    static const double PIBy2 = 1.57079632679489661923;

    //Hectare to Acres
    static const double ha2ac = 2.47105;

    //Acres to Hectare
    static const double ac2ha = 0.404686;

    //Meters to Acres
    static const double m2ac = 0.000247105;

    //Meters to Hectare
    static const double m2ha = 0.0001;

    // liters per hectare to us gal per acre
    static const double galAc2Lha = 9.35396;

    //us gal per acre to liters per hectare
    static const double LHa2galAc = 0.106907;

    //Liters to Gallons
    static const double L2Gal = 0.264172;

    //Gallons to Liters
    static const double Gal2L = 3.785412534258;

    //Distance calcs of all kinds
    static inline double Distance(double east1, double north1, double east2, double north2)
    {
        return sqrt( (east1 - east2) * (east1 - east2) +
                     (north1 - north2) * (north1 - north2) );
    }

    static inline double Distance(Vec2 first, Vec2 second)
    {
        return sqrt(pow(first.easting - second.easting, 2) +
                    pow(first.northing - second.northing, 2));
    }

    static inline double Distance(Vec2 first, Vec3 second)
    {
        return sqrt(pow(first.easting - second.easting, 2) +
                    pow(first.northing - second.northing, 2));
    }

    static inline double Distance(Vec3 first, Vec2 second)
    {
        return sqrt(pow(first.easting - second.easting, 2) +
                    pow(first.northing - second.northing, 2));
    }

    static inline double Distance(Vec3 first, Vec3 second)
    {
        return sqrt(pow(first.easting - second.easting, 2) +
                    pow(first.northing - second.northing, 2));
    }

    static inline double Distance(Vec2 first, double east, double north)
    {
        return sqrt(pow(first.easting - east, 2)+
                    pow(first.northing - north, 2));
    }

    static inline double Distance(Vec3 first, double east, double north)
    {
        return sqrt(pow(first.easting - east, 2) +
                    pow(first.northing - north, 2));
    }

    static inline double Distance(VecFix2Fix first, Vec2 second)
    {
        return sqrt(pow(first.easting - second.easting, 2) +
                    pow(first.northing - second.northing, 2));
    }

    static inline double Distance(VecFix2Fix first, VecFix2Fix second)
    {
        return sqrt(pow(first.easting - second.easting, 2) +
                    pow(first.northing - second.northing, 2));
    }

    //not normalized distance, no square root
    static inline double DistanceSquared(double northing1, double easting1, double northing2, double easting2)
    {
        return pow(easting1 - easting2, 2) + pow(northing1 - northing2, 2);
    }

    static inline double DistanceSquared(Vec3 first, Vec2 second)
    {
        return (pow(first.easting - second.easting, 2) +
                pow(first.northing - second.northing, 2));
    }

    static inline double DistanceSquared(Vec2 first, Vec3 second)
    {
        return (pow(first.easting - second.easting, 2) +
                pow(first.northing - second.northing, 2));
    }

    static inline double DistanceSquared(Vec3 first, Vec3 second)
    {
        return (pow(first.easting - second.easting, 2) +
                pow(first.northing - second.northing, 2));
    }
    static inline double DistanceSquared(Vec2 first, Vec2 second)
    {
        return (pow(first.easting - second.easting, 2) +
                pow(first.northing - second.northing, 2));
    }

    static inline double DistanceSquared(VecFix2Fix first, Vec2 second)
    {
        return (pow(first.easting - second.easting, 2) +
                pow(first.northing - second.northing, 2));
    }

    static inline float toDegrees(float radians)
    {
        return radians * 180.0 / M_PI;
    }

    static inline double toDegrees(double radians)
    {
        return radians * 180.0 / M_PI;
    }

    static inline float toRadians(float degrees)
    {
        return degrees * M_PI / 180.0;
    }

    static inline double toRadians(double degrees)
    {
        return degrees * M_PI / 180.0;
    }

    static inline double roundMidAwayFromZero(double number) {
        return lround(number);
    }

    static inline void GetClosestSegmentLooping(Vec2 Point, QVector<Vec3> curList, int &AA, int &BB, double &minDistA, int Start = 0, int End = INT_MAX)
    {
        bool isLoop = true;
        AA = -1; BB = -1;
        minDistA = DOUBLE_MAX;
        int A = Start > 0 ? Start - 1 : curList.count() - 1;
        bool looping = isLoop & Start > End;
        for (int B = (Start > 0 ? Start : 0); B < End || looping; A = B++)
        {
            if (B == curList.count())
            {
                if (looping)
                {
                    B = -1; looping = false;
                    continue;
                }
                else break;
            }
            if (B == 0 && !isLoop) continue;
            else if (B == 0) A = curList.count() - 1;

            double dist2 = DistanceSquared(curList[A], curList[B]);
            if (dist2 < minDistA)
            {
                minDistA = dist2; AA = A;
                BB = B;
            }
        }
    }

    static inline bool IsPointInPolygon(QVector<Vec3> polygon, Vec3 testPoint) {
        bool result = false;
        int j = polygon.count() - 1;
        for (int i = 0; i < polygon.count(); i++)
        {
            if ((polygon[i].easting < testPoint.easting &&
                 polygon[j].easting >= testPoint.easting) ||
                (polygon[j].easting < testPoint.easting &&
                 polygon[i].easting >= testPoint.easting))
            {
                if (polygon[i].northing + (testPoint.easting - polygon[i].easting) /
                    (polygon[j].easting - polygon[i].easting) * (polygon[j].northing - polygon[i].northing)
                    < testPoint.northing)
                {
                    result = !result;
                }
            }
            j = i;
        }
        return result;
    }

    static inline bool IsPointInPolygon(QVector<Vec3> polygon, Vec2 testPoint) {
        bool result = false;
        int j = polygon.count() - 1;
        for (int i = 0; i < polygon.count(); i++)
        {
            if ((polygon[i].easting < testPoint.easting &&
                 polygon[j].easting >= testPoint.easting) ||
                (polygon[j].easting < testPoint.easting &&
                 polygon[i].easting >= testPoint.easting))
            {
                if (polygon[i].northing + (testPoint.easting - polygon[i].easting) /
                                              (polygon[j].easting - polygon[i].easting) * (polygon[j].northing - polygon[i].northing)
                    < testPoint.northing)
                {
                    result = !result;
                }
            }
            j = i;
        }
        return result;
    }

    static inline bool IsPointInPolygon(QVector<Vec2> polygon, Vec2 testPoint) {
        bool result = false;
        int j = polygon.count() - 1;
        for (int i = 0; i < polygon.count(); i++)
        {
            if ((polygon[i].easting < testPoint.easting &&
                 polygon[j].easting >= testPoint.easting) ||
                (polygon[j].easting < testPoint.easting &&
                 polygon[i].easting >= testPoint.easting))
            {
                if (polygon[i].northing + (testPoint.easting - polygon[i].easting) /
                    (polygon[j].easting - polygon[i].easting) * (polygon[j].northing - polygon[i].northing)
                    < testPoint.northing)
                {
                    result = !result;
                }
            }
            j = i;
        }
        return result;
    }

    static inline bool IsPointInPolygon(QVector<Vec2> polygon, Vec3 testPoint) {
        bool result = false;
        int j = polygon.count() - 1;
        for (int i = 0; i < polygon.count(); i++)
        {
            if ((polygon[i].easting < testPoint.easting &&
                 polygon[j].easting >= testPoint.easting) ||
                (polygon[j].easting < testPoint.easting &&
                 polygon[i].easting >= testPoint.easting))
            {
                if (polygon[i].northing + (testPoint.easting - polygon[i].easting) /
                                              (polygon[j].easting - polygon[i].easting) * (polygon[j].northing - polygon[i].northing)
                    < testPoint.northing)
                {
                    result = !result;
                }
            }
            j = i;
        }
        return result;
    }

    static inline int GetLineIntersection(double p0x, double p0y, double p1x, double p1y,
                                      double p2x, double p2y, double p3x, double p3y,
                                      double &iEast, double &iNorth)
    {
        double s1x, s1y, s2x, s2y;
        s1x = p1x - p0x;
        s1y = p1y - p0y;

        s2x = p3x - p2x;
        s2y = p3y - p2y;

        double s, t;
        s = (-s1y * (p0x - p2x) + s1x * (p0y - p2y)) / (-s2x * s1y + s1x * s2y);

        if (s >= 0 && s <= 1)
        {
            //check oher side
            t = (s2x * (p0y - p2y) - s2y * (p0x - p2x)) / (-s2x * s1y + s1x * s2y);
            if (t >= 0 && t <= 1)
            {
                // Collision detected
                iEast = p0x + (t * s1x);
                iNorth = p0y + (t * s1y);
                return 1;
            }
        }

        return 0; // No collision
    }

    inline static Vec3 Catmull(double t, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3)
    {
        double tt = t * t;
        double ttt = tt * t;

        double q1 = -ttt + 2.0f * tt - t;
        double q2 = 3.0f * ttt - 5.0f * tt + 2.0f;
        double q3 = -3.0f * ttt + 4.0f * tt + t;
        double q4 = ttt - tt;

        double tx = 0.5f * (p0.easting * q1 + p1.easting * q2 + p2.easting * q3 + p3.easting * q4);
        double ty = 0.5f * (p0.northing * q1 + p1.northing * q2 + p2.northing * q3 + p3.northing * q4);

        return Vec3(tx, ty, 0);

        //f(t) = a_3 * t^3 + a_2 * t^2 + a_1 * t + a_0  cubic polynomial
        //vec3 a = 2.0 * p1;
        //vec3 b = p2 - p0;
        //vec3 c = 2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3;
        //vec3 d = -1.0 * p0 + 3.0 * p1 - 3.0 * p2 + p3;

        //return (0.5 * (a + (t * b) + (t * t * c) + (t * t * t * d)));
        //

        //vec2 p0 = new vec2(1, 0);
        //vec2 p1 = new vec2(3, 2);
        //vec2 p2 = new vec2(5, 3);
        //vec2 p3 = new vec2(6, 1);

        //vec2 a = 2.0 * p1;
        //vec2 b = p2 - p0;
        //vec2 c = 2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3;
        //vec2 d = -1.0 * p0 + 3.0 * p1 - 3.0 * p2 + p3;

        //double tt = 0.25;

        //vec2 pos = 0.5 * (a + (tt*b) + (tt * tt * c) + (tt * tt * tt * d));
    }

    inline static double CatmullGradient(double t, Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3)
    {
        double tt = t * t;

        double q1 = -3.0f * tt + 4.0f * t - 1;
        double q2 = 9.0f * tt - 10.0f * t;
        double q3 = -9.0f * tt + 8.0f * t + 1.0f;
        double q4 = 3.0f * tt - 2.0f * t;

        double tx = 0.5f * (p0.easting * q1 + p1.easting * q2 + p2.easting * q3 + p3.easting * q4);
        double ty = 0.5f * (p0.northing * q1 + p1.northing * q2 + p2.northing * q3 + p3.northing * q4);

        return atan2(tx, ty);

        //f(t) = a_3 * t^3 + a_2 * t^2 + a_1 * t + a_0  cubic polynomial
        //vec3 a = 2.0 * p1;
        //vec3 b = p2 - p0;
        //vec3 c = 2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3;
        //vec3 d = -1.0 * p0 + 3.0 * p1 - 3.0 * p2 + p3;

        //return (0.5 * (a + (t * b) + (t * t * c) + (t * t * t * d)));
        //
    }

    inline static bool InRangeBetweenAB(double start_x, double start_y, double end_x, double end_y,
                                   double point_x, double point_y)
    {
        double dx = end_x - start_x;
        double dy = end_y - start_y;
        double innerProduct = (point_x - start_x) * dx + (point_y - start_y) * dy;
        return (0 <= innerProduct) && (innerProduct <= dx * dx + dy * dy);
    }

}
#endif // GLM_H
