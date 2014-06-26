#ifndef __ANGLEROUTINES__
#define __ANGLEROUTINES__

#include <math.h>
/* anglestuff.h
*  aloitettu 10.8
*  -Toni Sanio
*/

const double pi = 3.141; // far from accurate value

double DegToRad(double deg) { return (deg/180.0) * pi; }

double RadToDeg(double rad) { return (rad*180.0) / pi; }

double getRandomRad() { return (rand () % int(pi*100000.0d))/100000.0d; }

double distance(double x, double y, double x2, double y2)
{
    double deltaX=0;
    double deltaY=0;
    // Funktio etsii kahden pisteen välisen kulman ja palauttaa sen
    // Ensiksi tarvitaan delta-arvot:
    if (x > x2) deltaX = x - x2;
    if (x2 > x) deltaX = x2 - x;
    if (y > y2) deltaY = y - y2;
    if (y2 > y) deltaY = y2 - y;
    // Näiden avulla voidaan ottaa sitten etäisyys
    if (deltaX == 0 && deltaY == 0)
       return 0;
    else
       return sqrt( deltaX*deltaX+deltaY*deltaY );
}

double angleBetween(double x, double y, double x2, double y2)
{
if (x2-x != 0)
   x += 1;

   double angle = atan2(y2 - y, x2 - x) ;


    return angle;
}

double angleBetweenInDegrees(double x, double y, double x2, double y2) {
    return RadToDeg(angleBetween(x,y,x2,y2));
}

int distAngleToAngle(int angle1, int angle2) {
    // Väliaikainen kikkailu:
    angle1 += 180;
    angle2 += 180;

    int result;
    result = 0;

    if (angle1 > angle2) {
          if ( (angle1 - angle2) > (360-angle1+angle2) )
             result = (360-angle1+angle2); // Pienin on aina tietenkin se haluttu etäisyys
          else
             result = (angle1 - angle2);
    }

    if (angle2 > angle1) {
          if ( (angle2 - angle1) > (360-angle2+angle1) )
             result = (360-angle2+angle1); // Pienin on aina tietenkin se haluttu etäisyys
          else
             result = (angle2 - angle1);
    }

    return result;

}

int shortestWayTo(int angle1, int angle2) {
    // Väliaikainen kikkailu:
    angle1 += 180;
    angle2 += 180;

    int result;
    result = 0;

    if (angle1 > angle2) {
          if ( (angle1 - angle2) > (360-angle1+angle2) )
             result = 1; // Pienin on aina tietenkin se haluttu etäisyys
          else
             result = -1;
    }

    if (angle2 > angle1) {
          if ( (angle2 - angle1) > (360-angle2+angle1) )
             result = -1;
          else
             result = 1;
    }

    return result;

}

#endif
