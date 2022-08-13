#include <iostream>
#include <string>
#include <atomic>
#include <thread>

#include "socket.h"
#include "gps.h"
#include "motor.h"
#include "final.h"
#include "ini_reader.h"

std::atomic_bool GPS_EXIT_FLAG {false};
std::atomic_bool MOTOR_EXIT_FLAG {false};

extern GPSData gpsData;
extern MotorData motorData;

bool enable_motor = true;

MotorFeeder feed;

void callbackFun()
{
    double increment_left = -2.0, increment_right = -2.0;
    feed.calculate(gpsData.groundDirection, gpsData.curPosLat, gpsData.curPosLon, gpsData.groundSpeed, gpsData.angleSpeed, increment_left, increment_right);
    motorData.updateMotor(increment_left, increment_right);
}

int _thread_gps_updater();
int _thread_motor_updater(const std::string &address);

void readConf()
{
    INIReader ini;
    if(ini.ParseFile("settings.ini") != INIREADER_EXCEPTION_NONE)
        return;
    ini.EnterSection("common");
    string_array destinations;
    ini.GetAll("destination", destinations);
    size_t index = 0;
    for(std::string &x : destinations)
    {
        if(index >= 10) break;
        string_array vArray = split(x, ",");
        if(vArray.size() != 2)
            continue;
        double lat = to_number<double>(vArray[0]);
        double lon = to_number<double>(vArray[1]);
        feed.coor[index].DestinationSX = lat;
        feed.coor[index].DestinationSY = lon;
        index++;
    }
    ini.GetBoolIfExist("enable_motor", enable_motor);
}

int main()
{
    #ifdef _WIN32
    //start up windows socket library first
    WSADATA wsd;
    if(WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        std::cerr<<"wsa startup error"<<std::endl;
        return -1;
    }
    #endif // _WIN32
    readConf();
    feed.InitDestination();
    gpsData.dataCallback = callbackFun;
    std::thread gpsThread = std::thread(_thread_gps_updater);
   // motorData.updateMotor(1,-1);
    if(enable_motor)
    {
        std::thread motorThread = std::thread(_thread_motor_updater, "tcp://192.168.0.81:1883");
        motorThread.join();
    }
    gpsThread.join();
    #ifdef _WIN32
    //stop socket library before exit
    WSACleanup();
    #endif
    return 0;
}
