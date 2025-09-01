// CDataFusion.cpp 中实现卡尔曼滤波融合逻辑
#include "pch.h"
#include "CDataFusion.h"
#include <cmath>

CDataFusion::CDataFusion()
    : m_alg(WEIGHTED), m_gpsMSE(0), m_insMSE(0), m_fuseMSE(0) 
{
    // 初始化卡尔曼滤波器参数
    Eigen::MatrixXd F(6, 6); // 状态转移矩阵
    Eigen::MatrixXd H(2, 6); // 观测矩阵
    Eigen::MatrixXd Q(6, 6); // 过程噪声协方差
    Eigen::MatrixXd R(2, 2); // 观测噪声协方差

    // 初始化状态转移矩阵 (6x6) - 针对位置、速度、加速度模型
    F << 1, 0.1, 0.005, 0, 0, 0,
        0, 1, 0.1, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0.1, 0.005,
        0, 0, 0, 0, 1, 0.1,
        0, 0, 0, 0, 0, 1;

    // 初始化观测矩阵 (2x6) - 只观测位置
    H << 1, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0;

    // 初始化过程噪声协方差 (INS误差)
    Q = Eigen::MatrixXd::Identity(6, 6) * 0.01;

    // 初始化观测噪声协方差 (GPS误差)
    R = Eigen::MatrixXd::Identity(2, 2) * 1.0;

    m_kalman.SetParam(F, H, Q, R);
}

CDataFusion::~CDataFusion() {}

void CDataFusion::SetAlgorithm(AlgType alg) 
{
    m_alg = alg;
}

void CDataFusion::CalcSensorMSE(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY) {
    // 计算GPS的MSE
    double gpsErrX = gpsX - trueX;
    double gpsErrY = gpsY - trueY;
    m_gpsMSE = (gpsErrX * gpsErrX + gpsErrY * gpsErrY) / 2.0;

    // 计算INS的MSE
    double insErrX = insX - trueX;
    double insErrY = insY - trueY;
    m_insMSE = (insErrX * insErrX + insErrY * insErrY) / 2.0;
}

void CDataFusion::FuseData(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY) {
    switch (m_alg) {
    case KALMAN:
        KalmanFusion(trueX, trueY, gpsX, gpsY, insX, insY, fuseX, fuseY);
        break;
    case WEIGHTED:
        WeightedFusion(gpsX, gpsY, insX, insY, fuseX, fuseY);
        break;
        // 其他算法实现...
    default:
        WeightedFusion(gpsX, gpsY, insX, insY, fuseX, fuseY);
    }

    // 计算融合结果的MSE
    double fuseErrX = fuseX - trueX;
    double fuseErrY = fuseY - trueY;
    m_fuseMSE = (fuseErrX * fuseErrX + fuseErrY * fuseErrY) / 2.0;
}

void CDataFusion::GetMSE(double& gpsMSE, double& insMSE, double& fuseMSE) {
    gpsMSE = m_gpsMSE;
    insMSE = m_insMSE;
    fuseMSE = m_fuseMSE;
}

void CDataFusion::WeightedFusion(double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY) {
    // 简单加权融合实现
    double wGps = 1.0 / (m_gpsMSE + 1e-6); // 防止除零
    double wIns = 1.0 / (m_insMSE + 1e-6);
    double total = wGps + wIns;

    fuseX = (wGps * gpsX + wIns * insX) / total;
    fuseY = (wGps * gpsY + wIns * insY) / total;
}

void CDataFusion::KalmanFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY) {
    // 使用INS数据进行预测
    Eigen::VectorXd currentState = m_kalman.GetState();

    // 预测步骤
    m_kalman.Predict();

    // 使用GPS数据进行更新
    m_kalman.Update(gpsX, gpsY);

    // 获取融合后的状态
    Eigen::VectorXd fusedState = m_kalman.GetState();
    fuseX = fusedState[0];  // X位置
    fuseY = fusedState[3];  // Y位置
}

// 其他滤波算法的实现可以在这里添加...
void CDataFusion::UKFFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY) {
    // 此处实现UKF融合算法
    fuseX = (gpsX + insX) / 2; // 临时实现，需要替换为实际UKF算法
    fuseY = (gpsY + insY) / 2;
}

void CDataFusion::ParticleFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY) {
    // 此处实现粒子滤波融合算法
    fuseX = (gpsX + insX) / 2; // 临时实现，需要替换为实际粒子滤波算法
    fuseY = (gpsY + insY) / 2;
}
CDataFusion::AlgType CDataFusion::GetAlgorithm() const {
    return m_alg;  // 返回当前算法类型
}