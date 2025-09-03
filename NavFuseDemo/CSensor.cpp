#include "pch.h"
#include "CSensor.h"
void CSensor::SetParam(double freq, double acc) {
	m_freq = freq;
	m_acc = acc;
	m_noiseSigma = sqrt(acc); // 精度→噪声标准差的转换
}
double CSensor:: GenerateGaussNoise()
{
    std::normal_distribution<double> dist(0.0, m_noiseSigma); // 正态分布：均值0，标准差σ
    return dist(m_gen); // 生成噪声值
}
bool CGPS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) 
{
    // 1. 检查是否达到刷新频率
    if (currentTime - m_lastTime < 1.0 / m_freq) {
        return false; // 未到时间，沿用旧数据
    }
    // 2. 更新上次生成时间
    m_lastTime = currentTime;
  
    simX = trueX + GenerateGaussNoise();
    simY = trueY + GenerateGaussNoise();
    return true; 
}
CINS::CINS() : m_lastTrueX(0), m_lastTrueY(0), m_lastVelX(0), m_lastVelY(0),
m_accX(0), m_accY(0), m_velX(0), m_velY(0), m_posX(0), m_posY(0),  // m_posY初始为0
m_driftX(0), m_driftY(0), m_driftRate(0.01)  // m_driftY初始为0（无初始漂移）
{
}
bool CINS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) {
    double dt = (currentTime - m_lastTime);  // 时间增量（积分步长）
    if (dt < 1.0 / m_freq) {
        return false;  // 未到采样时间
    }

    // 1. 生成带噪声的加速度测量值（模拟加速度计输出）
    // 真实加速度可从运动模型反推（或假设匀速/匀加速运动）
    double currentVelX = (trueX - m_lastTrueX) / dt;  // 当前真实速度
    double currentVelY = (trueY - m_lastTrueY) / dt;
    double trueAccX = (currentVelX - m_lastVelX) / dt;  // 加速度=速度变化率
    double trueAccY = (currentVelY - m_lastVelY) / dt;
    m_accX = trueAccX + GenerateGaussNoise() * 0.01;  // 加速度噪声（σ=0.1）
    m_accY = trueAccY + GenerateGaussNoise() * 0.01;

    // 2. 一次积分：加速度→速度（v = v0 + a*dt）
    m_velX += m_accX * dt;
    m_velY += m_accY * dt;

    // 3. 二次积分：速度→位置（x = x0 + v*dt + 0.5*a*dt?）
    m_posX += m_velX * dt + 0.5 * m_accX * dt * dt;
    m_posY += m_velY * dt + 0.5 * m_accY * dt * dt;

    // 4. 叠加累积漂移（原逻辑保留，模拟积分误差放大）
    double driftDelta = m_driftRate * dt + GenerateGaussNoise() * 0.005;  // 漂移增量
    double driftDeltaX = m_driftRate * dt + GenerateGaussNoise() * 0.005;
    double driftDeltaY = m_driftRate * dt + GenerateGaussNoise() * 0.005;
    m_driftX += driftDeltaX;
    m_driftY += driftDeltaY;
    m_posX += m_driftX;  // 位置漂移
    m_posY += m_driftY;

    // 5. 输出模拟位置（积分结果）
    simX = m_posX;
    simY = m_posY;

    // 更新历史状态（用于下一次积分）
    m_lastTime = currentTime;
    m_lastTrueX = trueX;
    m_lastTrueY = trueY;
    m_lastVelX = m_velX;
    m_lastVelY = m_velY;

    return true;
}

void CINS::SetDriftRate(double driftRate) 
{
    m_driftRate = driftRate;
}

