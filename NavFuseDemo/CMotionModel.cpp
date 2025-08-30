#include "pch.h"
#include "CMotionModel.h"
#include <cmath>  // 用于数学计算（正弦、余弦等）

// 构造函数：初始化运动默认状态
CMotionModel::CMotionModel()
    : m_type(LINE),  // 默认直线运动
    m_time(0.0),   // 初始累计时间为0
    m_x(0.0),      // 初始X坐标
    m_y(0.0),      // 初始Y坐标
    m_radius(0.0), // 圆周运动半径
    m_omega(0.0),  // 圆周运动角速度
    m_sAmp(0.0),   // S型曲线振幅
    m_lineSpeed(0.0) // 直线运动速度
{
}

// 析构函数：无动态资源需要释放
CMotionModel::~CMotionModel()
{
}

// 设置运动类型及对应参数
void CMotionModel::SetMotionParam(MotionType type, double param1, double param2)
{
    m_type = type;  // 更新运动类型

    // 根据运动类型设置相应参数
    switch (type)
    {
    case LINE:
        m_lineSpeed = param1;  // param1 = 速度(m/s)
        // 直线运动不需要param2，忽略
        break;
    case ARC:
        m_radius = param1;     // param1 = 半径(m)
        m_omega = param2;      // param2 = 角速度(rad/s)
        break;
    case S_CURVE:
        m_sAmp = param1;       // param1 = 振幅(m)
        // S型曲线不需要param2，忽略
        break;
    }

    // 重置位置和时间，新运动从原点重新开始
    m_x = 0.0;
    m_y = 0.0;
    m_time = 0.0;
}

// 按时间步长更新真实位置
void CMotionModel::UpdateTruePos(double dt)
{
    // 累加时间
    m_time += dt;

    // 根据当前运动类型计算新位置
    switch (m_type)
    {
    case LINE:
        // 直线运动：x = 速度 * 累计时间，y保持0
        m_x = m_lineSpeed * m_time;
        m_y = 300.0;
        break;

    case ARC:
        // 圆周运动：x = 半径 * cos(角速度 * 时间)，y = 半径 * sin(角速度 * 时间)
        m_x = 300+m_radius * cos(m_omega * m_time);
        m_y = 300+m_radius * sin(m_omega * m_time);
        break;

    case S_CURVE:
        // S型曲线：x随时间线性增加，y按正弦曲线变化
        m_x = m_time*20;  // x方向匀速运动
        m_y = 300+m_sAmp * sin(m_time);  // y方向正弦曲线
        break;
    }
}

// 获取当前真实位置
void CMotionModel::GetTruePos(double& x, double& y)
{
    x = m_x;  // 返回当前X坐标
    y = m_y;  // 返回当前Y坐标
}
