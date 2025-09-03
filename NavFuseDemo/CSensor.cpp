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
bool CGPS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) 
{
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
CINS::CINS() : m_lastTrueX(0), m_lastTrueY(0), m_lastVelX(0), m_lastVelY(0),
m_accX(0), m_accY(0), m_velX(0), m_velY(0), m_posX(0), m_posY(0),  // m_posY��ʼΪ0
m_driftX(0), m_driftY(0), m_driftRate(0.01)  // m_driftY��ʼΪ0���޳�ʼƯ�ƣ�
{
}
bool CINS::GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) {
    double dt = (currentTime - m_lastTime);  // ʱ�����������ֲ�����
    if (dt < 1.0 / m_freq) {
        return false;  // δ������ʱ��
    }

    // 1. ���ɴ������ļ��ٶȲ���ֵ��ģ����ٶȼ������
    // ��ʵ���ٶȿɴ��˶�ģ�ͷ��ƣ����������/�ȼ����˶���
    double currentVelX = (trueX - m_lastTrueX) / dt;  // ��ǰ��ʵ�ٶ�
    double currentVelY = (trueY - m_lastTrueY) / dt;
    double trueAccX = (currentVelX - m_lastVelX) / dt;  // ���ٶ�=�ٶȱ仯��
    double trueAccY = (currentVelY - m_lastVelY) / dt;
    m_accX = trueAccX + GenerateGaussNoise() * 0.01;  // ���ٶ���������=0.1��
    m_accY = trueAccY + GenerateGaussNoise() * 0.01;

    // 2. һ�λ��֣����ٶȡ��ٶȣ�v = v0 + a*dt��
    m_velX += m_accX * dt;
    m_velY += m_accY * dt;

    // 3. ���λ��֣��ٶȡ�λ�ã�x = x0 + v*dt + 0.5*a*dt?��
    m_posX += m_velX * dt + 0.5 * m_accX * dt * dt;
    m_posY += m_velY * dt + 0.5 * m_accY * dt * dt;

    // 4. �����ۻ�Ư�ƣ�ԭ�߼�������ģ��������Ŵ�
    double driftDelta = m_driftRate * dt + GenerateGaussNoise() * 0.005;  // Ư������
    double driftDeltaX = m_driftRate * dt + GenerateGaussNoise() * 0.005;
    double driftDeltaY = m_driftRate * dt + GenerateGaussNoise() * 0.005;
    m_driftX += driftDeltaX;
    m_driftY += driftDeltaY;
    m_posX += m_driftX;  // λ��Ư��
    m_posY += m_driftY;

    // 5. ���ģ��λ�ã����ֽ����
    simX = m_posX;
    simY = m_posY;

    // ������ʷ״̬��������һ�λ��֣�
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

