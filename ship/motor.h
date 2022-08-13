#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include <string>
#include <atomic>

#pragma pack(push)
#pragma pack(1)
struct diffdata
{
    float left = 0.0;
    float right = 0.0;
    u_char priority = 0;
    /* data */
};
#pragma pack(pop)

class MotorData
{
public:
    std::atomic<float> leftPercent {0.0};
    std::atomic<float> rightPercent {0.0};

    void updateMotor(float left, float right)
    {
        if(left < -1.0 || left > 1.0 || right < -1.0 || right > 1.0) return;
        for(float v = leftPercent; !leftPercent.compare_exchange_strong(v, left););
        for(float v = rightPercent; !rightPercent.compare_exchange_strong(v, right););
    }

    static float float_swap(float value)
    {
        union v
        {
            float f;
            uint32_t i;
        };
        v val;
        val.f = value;
        val.i = htonl(val.i);
        return val.f;
    }

    static std::string makeDiffPayload(const diffdata &d)
    {
        std::string result;
        diffdata data = d;
        data.left = float_swap(data.left * 0.9);
        data.right = float_swap(data.right * 0.9);
        result.assign(reinterpret_cast<const char*>(&data), 9);
        return result;
    }
};

#endif // MOTOR_H_INCLUDED
