#include "pch.h"
#include "CSensor.h"
void CSensor::SetParam(double freq, double acc) {
	m_freq = freq;
	m_acc = acc;
	m_noiseSigma = sqrt(acc); // 精度→噪声标准差的转换（符合高斯分布定义）
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
    // 3. 叠加高斯噪声（无漂移）
    simX = trueX + GenerateGaussNoise();
    simY = trueY + GenerateGaussNoise();
    return true; // 新数据有效
}
bool CINS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) {
    // 1. 检查是否达到刷新频率
    double timeDelta = currentTime - m_lastTime;
    if (timeDelta < 1.0 / m_freq) {
        return false;
    }
    // 2. 计算漂移增量（随时间累积）
    double driftDelta = m_driftRate * timeDelta;
    m_driftX += driftDelta; // X方向漂移累积
    m_driftY += driftDelta; // Y方向漂移累积（可扩展为独立漂移率）
    // 3. 更新上次生成时间
    m_lastTime = currentTime;
    // 4. 叠加噪声 + 累积漂移
    simX = trueX + GenerateGaussNoise() + m_driftX;
    simY = trueY + GenerateGaussNoise() + m_driftY;
    return true;
}

void CINS::SetDriftRate(double driftRate) {
    m_driftRate = driftRate;
}