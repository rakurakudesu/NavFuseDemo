#include "pch.h"
#include "CKalmanFilter.h"

CKalmanFilter::CKalmanFilter() {
    m_x = Eigen::VectorXd(6);
    m_x.setZero();
    m_P = Eigen::MatrixXd(6, 6);
    m_P.setIdentity();
    m_P *= 10.0;
}

CKalmanFilter::~CKalmanFilter() {}

void CKalmanFilter::SetParam(Eigen::MatrixXd F, Eigen::MatrixXd H, Eigen::MatrixXd Q, Eigen::MatrixXd R) {
    m_F = F;
    m_H = H;
    m_Q = Q;
    m_R = R;
}

void CKalmanFilter::Init(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP) {
    m_x = initialState;
    m_P = initialP;
}

void CKalmanFilter::Predict() {
    m_x = m_F * m_x;
    m_P = m_F * m_P * m_F.transpose() + m_Q;
}

// 核心修改：添加残差阈值判断
void CKalmanFilter::Update(double zX, double zY, double maxResidual) {
    Eigen::VectorXd z(2);
    z << zX, zY;

    Eigen::MatrixXd Ht = m_H.transpose();
    Eigen::MatrixXd S = m_H * m_P * Ht + m_R;
    Eigen::MatrixXd K = m_P * Ht * S.ldlt().solve(Eigen::MatrixXd::Identity(S.rows(), S.cols()));

    Eigen::VectorXd y = z - m_H * m_x;
    double residualNorm = y.norm(); // 计算残差模长

    // 若残差超过阈值，减小更新权重（0.1倍）
    if (residualNorm > maxResidual) {
        m_x += 0.1 * K * y; // 弱更新
        // 同时增大观测噪声，降低后续信任度
        Eigen::MatrixXd newR = m_R * (1 + residualNorm / maxResidual);
        SetR(newR);
    }
    else {
        m_x += K * y; // 正常更新
        // 残差正常时逐渐恢复R
        m_R = m_R * 0.95;
    }

    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(m_x.size(), m_x.size());
    m_P = (I - K * m_H) * m_P;
}

void CKalmanFilter::UpdateINS(double insX, double insY, double dt) {
    Eigen::VectorXd insState(6);
    insState << insX, (insX - m_x[0]) / dt, 0,
        insY, (insY - m_x[3]) / dt, 0;
    m_x = 0.9 * m_x + 0.1 * insState;
}

Eigen::VectorXd CKalmanFilter::GetState() const {
    return m_x;
}

void CKalmanFilter::InitState(double initX, double initY) {
    m_x.setZero();
    m_x[0] = initX;
    m_x[3] = initY;
    m_P.setIdentity();
    m_P *= 10.0;
}