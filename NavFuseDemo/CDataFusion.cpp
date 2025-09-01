// CDataFusion.cpp 实现数据融合的核心算法
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

    // 初始化状态转移矩阵 (6x6) - 包含位置、速度、加速度模型
    F << 1, 0.01, 0.00005, 0, 0, 0,
        0, 1, 0.01, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0.01, 0.00005,
        0, 0, 0, 0, 1, 0.01,
        0, 0, 0, 0, 0, 1;

    // 初始化观测矩阵 (2x6) - 只观测位置
    H << 1, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0;

    // 初始化过程噪声协方差（INS噪声）
    Q = Eigen::MatrixXd::Identity(6, 6) * 0.01;

    // 初始化观测噪声协方差（GPS噪声）
    R = Eigen::MatrixXd::Identity(2, 2) * 1.0;

    m_kalman.SetParam(F, H, Q, R);
}

CDataFusion::~CDataFusion() {}

void CDataFusion::SetAlgorithm(AlgType alg)
{
    m_alg = alg;
}

void CDataFusion::CalcSensorMSE(double trueX, double trueY, 
    double gpsX, double gpsY, 
    double insX, double insY) 
{
    // 计算GPS的MSE
    double gpsErrX = gpsX - trueX;
    double gpsErrY = gpsY - trueY;
    m_gpsMSE = (gpsErrX * gpsErrX + gpsErrY * gpsErrY) / 2.0;

    // 计算INS的MSE
    double insErrX = insX - trueX;
    double insErrY = insY - trueY;
    m_insMSE = (insErrX * insErrX + insErrY * insErrY) / 2.0;
}

void CDataFusion::FuseData(double trueX, double trueY, 
    double gpsX, double gpsY, 
    double insX, double insY, 
    double& fuseX, double& fuseY)
{
    switch (m_alg) 
    {
    case KALMAN:
        KalmanFusion(gpsX, gpsY, insX, insY, 0.01, fuseX, fuseY);
        break;
    case WEIGHTED:
        WeightedFusion(gpsX, gpsY, insX, insY, fuseX, fuseY);
        break;
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
    // 加权融合实现
    double wGps = 1.0 / (m_gpsMSE + 1e-6); // 防止除零
    double wIns = 1.0 / (m_insMSE + 1e-6);
    double total = wGps + wIns;

    fuseX = (wGps * gpsX + wIns * insX) / total;
    fuseY = (wGps * gpsY + wIns * insY) / total;
}

void CDataFusion::KalmanFusion(double gpsX, double gpsY,
    double insX, double insY,
    double dt,
    double& fuseX, double& fuseY)
{
    // 1. 用INS高频数据修正当前状态（INS频率高于GPS，提供实时运动趋势）
        Eigen::VectorXd currentState = m_kalman.GetState();
    // 假设INS输出的位置更实时，用INS的x/y修正当前状态的位置
    currentState[0] = insX;  // 更新x位置
    currentState[3] = insY;  // 更新y位置
    // 保留当前协方差，仅更新状态向量
    m_kalman.Init(currentState, m_kalman.GetStateCovariance());

    // 2. 执行预测（基于修正后的状态和运动模型）
    m_kalman.Predict();

    // 3. 用GPS低频高精度数据更新（校准预测偏差）
    m_kalman.Update(gpsX, gpsY);

    // 4. 输出融合结果（取状态中的x和y位置）
    Eigen::VectorXd fusedState = m_kalman.GetState();
    fuseX = fusedState[0];
    fuseY = fusedState[3];
}


CDataFusion::AlgType CDataFusion::GetAlgorithm() const {
    return m_alg;  // 返回当前算法类型
}