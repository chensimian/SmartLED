#include <string>
#include <iostream>
#include <atomic>

#include "socket.h"
#include "misc.h"
#include "string_hash.h"
#include "gps.h"

extern std::atomic_bool GPS_EXIT_FLAG;

GPSData gpsData;

#define RECV_BUF_SIZE 1024

std::string formatAngle(double angle)
{
    int degree = (int)angle, minute = (int)((angle - degree) * 60);
    float second = ((angle - degree) * 60 - minute) * 60.0;
    return std::to_string(degree) + "бу" + std::to_string(minute) + "'" + std::to_string(second) + "\"";
}

double degMinToDeg(double angle)
{
    int degree = angle / 100.0, minute = angle - (degree * 100.0);
    double second = (angle - (degree * 100.0) - minute) * 60.0;
    return degree + (minute / 60.0) + (second / 3600.0);
}

int _thread_gps_updater()
{
    const std::string server = "192.168.0.81";
    uint16_t port = 4004;

    SOCKET sHost = initSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sHost == INVALID_SOCKET)
    {
        std::cerr << "init socket failed" << std::endl;
        return 1;
    }
    defer(closesocket(sHost));
    setTimeout(sHost, 500);
    setSocketBlocking(sHost, false);
    if(startConnect(sHost, server, port) == SOCKET_ERROR)
    {
        std::cerr << "start connect error" << std::endl;
        return 1;
    }
    int cur_len;
    char buffer[RECV_BUF_SIZE];
    std::string remnant;
    while(1)
    {
        if(GPS_EXIT_FLAG) break;
        cur_len = Recv(sHost, buffer, RECV_BUF_SIZE, 0);
        if(cur_len < 0)
            continue;
        if(cur_len == 0)
            break;
        std::string data = remnant;
        data.append(buffer, cur_len);
        size_t begin_pos = 0, end_pos = data.find("\r\n");
        while(end_pos != data.npos)
        {
            std::string cmd = data.substr(begin_pos, end_pos - begin_pos);
            if(cmd[0] == '$' && cmd[1] == 'G' && !count_least(cmd, '$', 2) && regMatch(cmd, R"(^([\dA-Z0-9.\-,$*])*$)"))
            {
                //std::cout<<"received line: "<<cmd<<std::endl;
                string_array args = split(cmd, ",");
                switch(hash_(cmd.substr(1, 5)))
                {
                    /*
                    case "GPFPS"_hash: {
                        if(args.size() < 8) break;
                        std::string &sCurDirection = args[3], &sCurPosLat = args[6], &sCurPosLon = args[7];
                        double dCurDirection = to_number<double>(sCurDirection, -1.0), dCurPosLat = to_number<double>(sCurPosLat, 200.0), dCurPosLon = to_number<double>(sCurPosLon, 100.0);
                        //std::cout<<"\t\t\t\t\tlat: "<<sCurPosLat<<"  lon: "<<sCurPosLon<<"  direction: "<<sCurDirection<<std::endl;
                        //std::cout<<"\t\t\t\t\tlat: "<<formatAngle(dCurPosLat)<<"  lon: "<<formatAngle(dCurPosLon)<<"  direction: "<<formatAngle(dCurDirection)<<std::endl;
                        gpsData.setGroundDirection(dCurDirection);
                        //gpsData.setCurPosLat(dCurPosLat);
                        //gpsData.setCurPosLon(dCurPosLon);
                        gpsData.tryCallback();
                        break;
                    }
                    */
                    case "GPGGA"_hash: {
                        if(args.size() < 6) break;
                        std::string &sCurPosLat = args[2], &sCurPosLon = args[4];
                        double dCurPosLat = degMinToDeg(to_number<double>(sCurPosLat, 200.0)), dCurPosLon = degMinToDeg(to_number<double>(sCurPosLon, 100.0));
                        std::cout<<"\t\t\t\t\tlat: "<<sCurPosLat<<"  lon: "<<sCurPosLon<<std::endl;
                        std::cout<<"\t\t\t\t\tlat: "<<formatAngle(dCurPosLat)<<"  lon: "<<formatAngle(dCurPosLon)<<std::endl;
                        //gpsData.setGroundDirection(dCurDirection);
                        gpsData.setCurPosLat(dCurPosLat);
                        gpsData.setCurPosLon(dCurPosLon);
                        gpsData.tryCallback();
                        break;
                    }
                    case "GTIMU"_hash: {
                        if(args.size() < 6) break;
                        std::string &sAngleSpeed = args[5];
                        double dAngleSpeed = to_number<double>(sAngleSpeed, 1000.0);
                        std::cout<<"\t\tangle speed: "<<sAngleSpeed<<std::endl;
                        gpsData.setAngleSpeed(dAngleSpeed);
                        gpsData.tryCallback();
                        break;
                    }
                    case "GPVTG"_hash: {
                        if(args.size() < 8) break;
                        std::string &sGroundDirection = args[1], &sGroundSpeed = args[6];
                        double dGroundDirection = to_number<double>(sGroundDirection, -1.0), dGroundSpeed = to_number<double>(sGroundSpeed, -1.0);
                        std::cout<<"\t\t\t\t\t\t\tground speed: "<<sGroundSpeed<<std::endl;
                        //gpsData.setGroundDirection(dGroundDirection);
                        gpsData.setGroundSpeed(dGroundSpeed);
                        gpsData.tryCallback();
                        break;
                    }
                    case "GPHDT"_hash: {
                        if(args.size() < 3) break;
                        std::string &sCurDirection = args[1];
                        double dCurDirection = to_number<double>(sCurDirection, -1.0);
                        std::cout<<"\t\t\t\t\t\t\tground direction: "<<sCurDirection<<std::endl;
                        gpsData.setGroundDirection(dCurDirection);
                        gpsData.tryCallback();
                        break;
                    }
                }
            }
            begin_pos = end_pos + 2;
            end_pos = data.find("\r\n", begin_pos);
        }
        if(begin_pos < data.size() - 1) /// incomplete data
            remnant = data.substr(begin_pos);
        else
            remnant.clear();
    }
    return 0;
}
