#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct _Destination
{
	double CurrentLocationX;    //当前经度坐标
	double CurrentLocationY;    //当前纬度坐标

	double DestinationLocationX;//目标地的经度坐标
	double DestinationLocationY;//目标地的纬度坐标

	double DirectionAtanAngle;//与船头朝向形成的反正切角
	double DestinationAtanAngle; //与目的地形成的反正切角（上位机发送的）
	double DestinationZAngle;//要转的Z轴角度（要偏转的角度）*（转向路程） (0°~180°)
	double DestinationZAngle1;//要转的Z轴角度（要偏转的角度）*（转向路程） (-180°~180°)
	double DestinationAngle;//要转到的目标角度（转向终点角度）*
	char DestinationWheelDirection;//要转的Z轴方向 0为左 1为右
};

struct _Coor
{
    double DestinationSX = 0.0; //存储经度地址
    double DestinationSY = 0.0; //存储纬度地址
};


class MotorFeeder
{
public:
    void InitDestination();
    void calculate(double DirectionAtanAngle, double CurrentLocationY, double CurrentLocationX, double V, double AV, double &increment_left, double &increment_right);
    MotorFeeder();
    ~MotorFeeder() = default;

//private:
    int index = 0;
    bool Flag = false;          //抵达标志
    bool back_flag = false;
    _Coor coor[10];
    _Destination Destination;

//private:
    double PIDRegulateLeft(double A, double L, double V, double AV);
    double PIDRegulateRight(double A, double L, double V, double AV);
    double ArriveOrLeave();
    double CheckRadiusDestination();  //检测到目的地 半径
    double CalculateUwbDestinationZAnglee2();
};
