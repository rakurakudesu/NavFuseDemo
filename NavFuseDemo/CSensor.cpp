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
bool CGPS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) {
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
bool CINS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) {
    double timeDelta = currentTime - m_lastTime;
    if (timeDelta < 1.0 / m_freq) {
        return false;
    }

    // 漂移增量 = 固定漂移率分量 + 随机波动分量
    double fixedDrift = m_driftRate * timeDelta; // 系统性漂移
    double randomDrift = GenerateGaussNoise() * 0.05; // 随机漂移
    double driftDelta = fixedDrift + randomDrift;

    m_driftX += driftDelta;
    m_driftY += driftDelta; 

    m_lastTime = currentTime;
    simX = trueX + GenerateGaussNoise() + m_driftX;
    simY = trueY + GenerateGaussNoise() + m_driftY;
    return true;
}

void CINS::SetDriftRate(double driftRate) 
{
    m_driftRate = driftRate;
}

