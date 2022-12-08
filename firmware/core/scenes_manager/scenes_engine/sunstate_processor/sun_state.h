/*
 * Provides the ability to calculate the local time for sunrise,
 * sunwet, and moonrise at any point in time at any location in the world
 *
 * Original work used with permission maintaining license
 * Copyright (GPL) 2004 Mike Chirico mchirico@comcast.net
 * Modifications copyright
 * Copyright (GPL) 2015 Peter Buelow
 *
 * This file is part of the Sunset library
 *
 * Sunset is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Sunset is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SUNPOSITION_H__
#define __SUNPOSITION_H__

#include <math.h>
#include <stdint.h>

class SunState {
public:
    struct Time {
        uint32_t hours;
        uint32_t minutes;
        uint32_t seconds;
    };

    constexpr static uint32_t NaN = 0x80000000;

public:
    SunState();
    SunState( double lat, double lon, int timeZone );
    ~SunState();

    void setPosition( double lat, double lon, int timeZone );
    void setTZOffset( int timeZone );
    double setCurrentDate( int year, int month, int day );
    double calcSunriseUTC();
    double calcSunsetUTC();
    /* return time in minutes from midnight */
    double getSunriseMinutes();
    /* return time in minutes from midnight */
    double getSunsetMinutes();
    /* return time in seconds from midnight */
    double getSunriseSeconds();
    /* return time in seconds from midnight */
    double getSunsetSeconds();
    int moonPhase( int fromepoch );
    int moonPhase();
    Time getSunriseTime();
    Time getSunsetTime();

private:
    double degToRad( double );
    double radToDeg( double );
    double calcMeanObliquityOfEcliptic( double );
    double calcGeomMeanLongSun( double );
    double calcObliquityCorrection( double );
    double calcEccentricityEarthOrbit( double );
    double calcGeomMeanAnomalySun( double );
    double calcEquationOfTime( double );
    double calcTimeJulianCent( double );
    double calcSunTrueLong( double );
    double calcSunApparentLong( double );
    double calcSunDeclination( double );
    double calcHourAngleSunrise( double, double );
    double calcHourAngleSunset( double, double );
    double calcJD( int, int, int );
    double calcJDFromJulianCent( double );
    double calcSunEqOfCenter( double );
    Time secondsToTime( double seconds );

    double latitude;
    double longitude;
    double julianDate;
    int m_year;
    int m_month;
    int m_day;
    int tzOffset;
};

#endif
