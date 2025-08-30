#include "pch.h"
#include "CSensor.h"
void CSensor::SetParam(double freq, double acc) {
	m_freq = freq;
	m_acc = acc;
	m_noiseSigma = sqrt(acc); // ���ȡ�������׼���ת�������ϸ�˹�ֲ����壩
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
    // 3. ���Ӹ�˹��������Ư�ƣ�
    simX = trueX + GenerateGaussNoise();
    simY = trueY + GenerateGaussNoise();
    return true; // ��������Ч
}
bool CINS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) {
    // 1. ����Ƿ�ﵽˢ��Ƶ��
    double timeDelta = currentTime - m_lastTime;
    if (timeDelta < 1.0 / m_freq) {
        return false;
    }
    // 2. ����Ư����������ʱ���ۻ���
    double driftDelta = m_driftRate * timeDelta;
    m_driftX += driftDelta; // X����Ư���ۻ�
    m_driftY += driftDelta; // Y����Ư���ۻ�������չΪ����Ư���ʣ�
    // 3. �����ϴ�����ʱ��
    m_lastTime = currentTime;
    // 4. �������� + �ۻ�Ư��
    simX = trueX + GenerateGaussNoise() + m_driftX;
    simY = trueY + GenerateGaussNoise() + m_driftY;
    return true;
}

void CINS::SetDriftRate(double driftRate) {
    m_driftRate = driftRate;
}