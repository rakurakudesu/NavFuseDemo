// CKalmanFilter.cpp 实现卡尔曼滤波器的核心算法
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

void CKalmanFilter::Init(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP) {
    m_x = initialState;
    m_P = initialP;

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
    // 构造观测向量
    Eigen::VectorXd z(2);
    z << zX, zY;

    // 计算卡尔曼增益: K = P * H^T * (H * P * H^T + R)^-1
    Eigen::MatrixXd Ht = m_H.transpose();
    Eigen::MatrixXd S = m_H * m_P * Ht + m_R;
    // 使用Cholesky分解提高数值稳定性
    Eigen::MatrixXd K = m_P * Ht * S.ldlt().solve(Eigen::MatrixXd::Identity(S.rows(), S.cols()));

    // 更新状态估计
    Eigen::VectorXd y = z - m_H * m_x;
    m_x += K * y;

    // 更新协方差矩阵
    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(m_x.size(), m_x.size());
    m_P = (I - K * m_H) * m_P;
}

// 新增：使用INS数据进行状态更新（预测阶段修正）
void CKalmanFilter::UpdateINS(double insX, double insY, double dt)
{
    // 基于INS数据调整状态向量（主要修正位置和速度）
    Eigen::VectorXd insState(6);
    insState << insX, (insX - m_x[0]) / dt, 0,  // 简化加速度计算
        insY, (insY - m_x[3]) / dt, 0;

    // 融合INS数据到预测状态（带信任权重）
    m_x = 0.7 * m_x + 0.3 * insState;
}

Eigen::VectorXd CKalmanFilter::GetState() const
{
    return m_x;
}