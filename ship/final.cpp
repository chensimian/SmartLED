#include "final.h"
#include <iostream>

#define PI 3.14159

double Kp = 30;
const double Kl = 0.75;
const double Ka = 0.5;
double K3 = 90;

MotorFeeder::MotorFeeder()
{
    /*
    coor[0].DestinationSX = 113.61128916666667;
    coor[0].DestinationSY = 22.37449722222222;
    coor[1].DestinationSX = 113.61139194444445;
    coor[1].DestinationSY = 22.37476;
    coor[2].DestinationSX = 113.61137;
    coor[2].DestinationSY = 22.374347222222223;
    coor[3].DestinationSX = 113.61170305555555;
    coor[3].DestinationSY = 22.374565;
    coor[4].DestinationSX = 113.61150111111111;
    coor[4].DestinationSY = 22.374192222222224;

    coor[0].DestinationSX = 113.61140225;
    coor[0].DestinationSY = 22.37436129;
	Destination.DestinationLocationX = coor[0].DestinationSX;
    Destination.DestinationLocationY = coor[0].DestinationSY;
    */
}

void MotorFeeder::InitDestination()
{
    Destination.DestinationLocationX = coor[0].DestinationSX;
    Destination.DestinationLocationY = coor[0].DestinationSY;
}

double MotorFeeder::PIDRegulateLeft(double A,double L,double V,double AV)
{
	double increment_left;
	if(A>170||A<-170)//防止角度跳变
	{
		back_flag = true;
	}
	increment_left=Kp*(Kl*(L-3)-V)+K3*(Ka*A-AV);
	if(L<2&&L>-2)//停车范围
    {
        increment_left=0;
    }
	return increment_left;
}

double MotorFeeder::PIDRegulateRight(double A,double L,double V,double AV)
{
	double increment_right;
	increment_right=Kp*(Kl*(L-3)-V)-K3*(Ka*A-AV);
	if(L<2&&L>-2)
    {
        increment_right=0;
    }
	return increment_right;
}

double MotorFeeder::CheckRadiusDestination()//检测到目的地 半径
{
	/*
	 *  距离计算函数
	 *
	 *  DestinationLocationY    目标点纬度坐标
	 *  CurrentLocationY        当前船纬度坐标
	 *  DestinationLocationX    目标点经度坐标
	 *  CurrentLocationX        当前船经度坐标
	 * \return Radius           距离半径
	 */
	double Radius = 0;
    Radius=sqrt(pow((Destination.DestinationLocationY-Destination.CurrentLocationY)*111000, 2)
    +pow((Destination.DestinationLocationX-Destination.CurrentLocationX)*(cos(Destination.CurrentLocationY)*111000), 2));
    return Radius;
}

double MotorFeeder::CalculateUwbDestinationZAnglee2()
{
	/*
	*使用 0 - 360° 的方法,当前所有程序对应这个方法
	* 返回Destination.DestinationZAngle1            最终要偏转角度
	* Destination.DestinationZAngle				    要转的Z轴角度（要偏转的角度）*（转向路程）
	* Destination.DestinationWheelDirection:        要转的Z轴方向 0为左 1为右
	*/
	double &dirAtan = Destination.DirectionAtanAngle, &curPosX = Destination.CurrentLocationX, &curPosY = Destination.CurrentLocationY;
	double &dstPosX = Destination.DestinationLocationX, &dstPosY = Destination.DestinationLocationY, &dstZAngle = Destination.DestinationZAngle;
	double &dstAtan = Destination.DestinationAtanAngle, &dstZAngle1 = Destination.DestinationZAngle1;
	char &dstWheelDir = Destination.DestinationWheelDirection;
	/*
    Destination.DirectionAtanAngle = 180 - (Destination.DirectionAtanAngle - 90);
    if(Destination.DirectionAtanAngle < 0)
    {
        Destination.DirectionAtanAngle += 360;
    }
    */
    Destination.DirectionAtanAngle += 90;
    if(Destination.DirectionAtanAngle >= 360)
    {
        Destination.DirectionAtanAngle -= 360;
    }

    Destination.DestinationAtanAngle = atan2(curPosY - dstPosY, curPosX - dstPosX)* (180 / PI);

	if (Destination.DestinationAtanAngle < 0)
	{
		Destination.DestinationAtanAngle = 360 + Destination.DestinationAtanAngle;
	}

	if (Destination.DirectionAtanAngle > Destination.DestinationAtanAngle)
	{
		Destination.DestinationZAngle = Destination.DirectionAtanAngle - Destination.DestinationAtanAngle;
		dstWheelDir = 1;
	}
	else
	{
		Destination.DestinationZAngle = Destination.DestinationAtanAngle - Destination.DirectionAtanAngle;
		dstWheelDir = 0;
	}

	if (Destination.DestinationZAngle > 180)//用最小的角度去偏转
	{
		Destination.DestinationZAngle = 360.0 - Destination.DestinationZAngle;
		dstWheelDir = (~dstWheelDir) & 1;
	}

	if(dstWheelDir == 0)
	{
		Destination.DestinationZAngle1 = -Destination.DestinationZAngle;
	}
	if(dstWheelDir == 1)
	{
		Destination.DestinationZAngle1 = Destination.DestinationZAngle;
	}


	Destination.DestinationZAngle1 += 180;
	if(Destination.DestinationZAngle1 > 180)
    {
        Destination.DestinationZAngle1 -= 360;
    }

	return Destination.DestinationZAngle1;
}

double MotorFeeder::ArriveOrLeave()
{
	double _L = CheckRadiusDestination();

	if(_L <= 2.0 && Flag == false)//到达目的点
	{
		Flag = true;
      	index++;
       	Destination.DestinationLocationX = coor[index].DestinationSX;
      	Destination.DestinationLocationY = coor[index].DestinationSY;

		return CheckRadiusDestination();
	}
	if(_L >= 3.0)//离开目的点
	{
		Flag = false;
	}

	return _L;
}

void MotorFeeder::calculate(double DirectionAtanAngle, double CurrentLocationY, double CurrentLocationX, double V, double AV, double &increment_left, double &increment_right)
{
	if(back_flag)
    {
        increment_left= -1;
        increment_right= -1;
		back_flag = false;

		return;
	}
	Destination.DirectionAtanAngle = DirectionAtanAngle;
	Destination.CurrentLocationX = CurrentLocationX;
	Destination.CurrentLocationY = CurrentLocationY;
	if(Destination.DestinationLocationX == 0.0 && Destination.DestinationLocationY == 0.0)
    {
        increment_left = 0.0;
        increment_right = 0.0;
        return;
    }
	double L = ArriveOrLeave();
	double A = CalculateUwbDestinationZAnglee2();
	if(A > 90 || A < -90)
    {
        Kp = 20;
        K3 = 80;
    }
    else
    {
        Kp = 40;
        K3 = 60;
    }
	std::cout<<"L: "<<L<<" A:"<<A<<std::endl;
	double temp_left = PIDRegulateLeft(A, L, V, AV);
	double temp_right = PIDRegulateRight(A, L, V, AV);
	if(temp_left > 1000 || temp_right > 1000)
	{
		if(temp_right > temp_left)
        {
            increment_left = temp_left / temp_right;
            increment_right = 1;
        }
        else if(temp_right < temp_left)
        {
            increment_right = temp_right / temp_left;
            increment_left = 1;
        }
	}
	else
	{
		increment_left = temp_left / 1000;
		increment_right = temp_right / 1000;
	}
}
