#include "pch.h"
#include "CMotionModel.h"
#include <cmath>  // ������ѧ���㣨���ҡ����ҵȣ�

// ���캯������ʼ���˶�Ĭ��״̬
CMotionModel::CMotionModel()
    : m_type(LINE),  // Ĭ��ֱ���˶�
    m_time(0.0),   // ��ʼ�ۼ�ʱ��Ϊ0
    m_x(0.0),      // ��ʼX����
    m_y(0.0),      // ��ʼY����
    m_radius(0.0), // Բ���˶��뾶
    m_omega(0.0),  // Բ���˶����ٶ�
    m_sAmp(0.0),   // S���������
    m_lineSpeed(0.0) // ֱ���˶��ٶ�
{
}

// �����������޶�̬��Դ��Ҫ�ͷ�
CMotionModel::~CMotionModel()
{
}

// �����˶����ͼ���Ӧ����
void CMotionModel::SetMotionParam(MotionType type, double param1, double param2)
{
    m_type = type;  // �����˶�����

    // �����˶�����������Ӧ����
    switch (type)
    {
    case LINE:
        m_lineSpeed = param1;  // param1 = �ٶ�(m/s)
        // ֱ���˶�����Ҫparam2������
        break;
    case ARC:
        m_radius = param1;     // param1 = �뾶(m)
        m_omega = param2;      // param2 = ���ٶ�(rad/s)
        break;
    case S_CURVE:
        m_sAmp = param1;       // param1 = ���(m)
        // S�����߲���Ҫparam2������
        break;
    }

    // ����λ�ú�ʱ�䣬���˶���ԭ�����¿�ʼ
    m_x = 0.0;
    m_y = 0.0;
    m_time = 0.0;
}

// ��ʱ�䲽��������ʵλ��
void CMotionModel::UpdateTruePos(double dt)
{
    // �ۼ�ʱ��
    m_time += dt;

    // ���ݵ�ǰ�˶����ͼ�����λ��
    switch (m_type)
    {
    case LINE:
        // ֱ���˶���x = �ٶ� * �ۼ�ʱ�䣬y����0
        m_x = m_lineSpeed * m_time;
        m_y = 300.0;
        break;

    case ARC:
        // Բ���˶���x = �뾶 * cos(���ٶ� * ʱ��)��y = �뾶 * sin(���ٶ� * ʱ��)
        m_x = 300+m_radius * cos(m_omega * m_time);
        m_y = 300+m_radius * sin(m_omega * m_time);
        break;

    case S_CURVE:
        // S�����ߣ�x��ʱ���������ӣ�y���������߱仯
        m_x = m_time*20;  // x���������˶�
        m_y = 300+m_sAmp * sin(m_time);  // y������������
        break;
    }
}

// ��ȡ��ǰ��ʵλ��
void CMotionModel::GetTruePos(double& x, double& y)
{
    x = m_x;  // ���ص�ǰX����
    y = m_y;  // ���ص�ǰY����
}
