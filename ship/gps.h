#ifndef GPS_H_INCLUDED
#define GPS_H_INCLUDED

#include <string>
#include <atomic>

class GPSData
{
public:
    std::atomic<double> curPosLat {-114514.0};
    std::atomic<double> curPosLon {-114514.0};
    std::atomic<double> angleSpeed {-114514.0};
    std::atomic<double> groundDirection {-114514.0};
    std::atomic<double> groundSpeed {-114514.0};
    void (*dataCallback)() = NULL;

    void tryCallback()
    {
        if(dataCallback == NULL || curPosLat == -114514.0 || curPosLon == -114514.0 || angleSpeed == -114514.0 || groundDirection == -114514.0 || groundSpeed == -114514.0) return;
        dataCallback();
    }

    void setCurPosLat(double value)
    {
        if(value < -90.0 || value > 90.0) return;
        for(double g = curPosLat; !curPosLat.compare_exchange_strong(g, value););
    }

    void setCurPosLon(double value)
    {
        if(value < -180.0 || value > 180.0) return;
        for(double g = curPosLon; !curPosLon.compare_exchange_strong(g, value););
    }

    void setAngleSpeed(double value)
    {
        if(value < -720.0 || value > 720.0) return;
        for(double g = angleSpeed; !angleSpeed.compare_exchange_strong(g, value););
    }

    void setGroundDirection(double value)
    {
        if(value < 0.0 || value >= 360.0) return;
        for(double g = groundDirection; !groundDirection.compare_exchange_strong(g, value););
    }

    void setGroundSpeed(double value)
    {
        if(value < 0.0) return;
        for(double g = groundSpeed; !groundSpeed.compare_exchange_strong(g, value););
    }
};

#endif // GPS_H_INCLUDED
