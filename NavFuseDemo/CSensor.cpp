#include "pch.h"
#include "CSensor.h"
void CSensor::SetParam(double freq, double acc) {
	m_freq = freq;
	m_acc = acc;
	m_noiseSigma = sqrt(acc); // ���ȡ�������׼���ת��
}
double CSensor:: GenerateGaussNoise()
{
    std::normal_distribution<double> dist(0.0, m_noiseSigma); // ��̬�ֲ�����ֵ0����׼���
    return dist(m_gen); // ��������ֵ
}
bool CGPS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) {
    // 1. ����Ƿ�ﵽˢ��Ƶ��
    if (currentTime - m_lastTime < 1.0 / m_freq) {
        return false; // δ��ʱ�䣬���þ�����
    }
    // 2. �����ϴ�����ʱ��
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

    // Ư������ = �̶�Ư���ʷ��� + �����������
    double fixedDrift = m_driftRate * timeDelta; // ϵͳ��Ư��
    double randomDrift = GenerateGaussNoise() * 0.05; // ���Ư��
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

