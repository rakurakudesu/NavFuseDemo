// CKalmanFilter.cpp 实现卡尔曼滤波器的核心逻辑
#include "pch.h"
#include "CKalmanFilter.h"

CKalmanFilter::CKalmanFilter() 
{
    // 初始化状态向量和协方差矩阵
    m_x = Eigen::VectorXd(6);
    m_x.setZero();

    m_P = Eigen::MatrixXd(6, 6);
    m_P.setIdentity();
    m_P *= 10.0; // 初始协方差
}

CKalmanFilter::~CKalmanFilter() {}

void CKalmanFilter::SetParam(Eigen::MatrixXd F, 
    Eigen::MatrixXd H, 
    Eigen::MatrixXd Q, 
    Eigen::MatrixXd R) 
{
    m_F = F;
    m_H = H;
    m_Q = Q;
    m_R = R;
}

void CKalmanFilter::Predict()
{
    // 预测状态: x = F * x
    m_x = m_F * m_x;

    // 预测协方差: P = F * P * F^T + Q
    m_P = m_F * m_P * m_F.transpose() + m_Q;
}

void CKalmanFilter::Update(double zX, double zY) 
{
    // 创建观测向量
    Eigen::VectorXd z(2);
    z << zX, zY;

    // 计算卡尔曼增益: K = P * H^T * (H * P * H^T + R)^-1
    Eigen::MatrixXd Ht = m_H.transpose();
    Eigen::MatrixXd S = m_H * m_P * Ht + m_R;
    Eigen::MatrixXd K = m_P * Ht * S.inverse();

    // 更新状态估计
    Eigen::VectorXd y = z - m_H * m_x;
    m_x += K * y;

    // 更新协方差矩阵
    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(m_x.size(), m_x.size());
    m_P = (I - K * m_H) * m_P;
}

Eigen::VectorXd CKalmanFilter::GetState() const
{
    return m_x;
}