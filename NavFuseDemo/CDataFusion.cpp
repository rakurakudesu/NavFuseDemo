#include "pch.h"
#include "CDataFusion.h"
#include "CKalmanFilter.h"
#include <cmath>

CDataFusion::CDataFusion() : m_alg(WEIGHTED), m_gpsMSE(0), m_insMSE(0), m_fuseMSE(0) {
    Eigen::MatrixXd F(6, 6);
    Eigen::MatrixXd H(2, 6);
    Eigen::MatrixXd Q(6, 6);
    Eigen::MatrixXd R(2, 2);

    F << 1, 0.01, 0.00005, 0, 0, 0,
        0, 1, 0.01, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0.01, 0.00005,
        0, 0, 0, 0, 1, 0.01,
        0, 0, 0, 0, 0, 1;

    H << 1, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0;

    Q = Eigen::MatrixXd::Identity(6, 6) * 0.01;
    R = Eigen::MatrixXd::Identity(2, 2) * 1.0;

    m_kalman.SetParam(F, H, Q, R);
}

CDataFusion::~CDataFusion() {}

void CDataFusion::SetAlgorithm(AlgType alg) {
    m_alg = alg;
}

void CDataFusion::CalcSensorMSE(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& gpsmse, double &insmse) 
{
    double gpsErrX = gpsX - trueX;
    double gpsErrY = gpsY - trueY;
    gpsmse += (gpsErrX * gpsErrX + gpsErrY * gpsErrY) / 2.0;

    double insErrX = insX - trueX;
    double insErrY = insY - trueY;
    insmse += (insErrX * insErrX + insErrY * insErrY) / 2.0;
}

void CDataFusion::FuseData(double trueX, double trueY,
    double gpsX, double gpsY,
    double insX, double insY,
    double& fuseX, double& fuseY,double & fusemse) {
    switch (m_alg) {
    case KALMAN:
        KalmanFusion(gpsX, gpsY, insX, insY, 0.01, fuseX, fuseY);
        break;
    case WEIGHTED:
        WeightedFusion(gpsX, gpsY, insX, insY, fuseX, fuseY);
        break;
    case GPS_ONLY: 
        fuseX = gpsX;
        fuseY = gpsY;
        break;
    default:
        WeightedFusion(gpsX, gpsY, insX, insY, fuseX, fuseY);
    }

    double fuseErrX = fuseX - trueX;
    double fuseErrY = fuseY - trueY;
    fusemse += (fuseErrX * fuseErrX + fuseErrY * fuseErrY) / 2.0;
}

void CDataFusion::GetMSE(double& gpsMSE, double& insMSE, double& fuseMSE) {
    gpsMSE = m_gpsMSE;
    insMSE = m_insMSE;
    fuseMSE = m_fuseMSE;
}

void CDataFusion::WeightedFusion(double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY) {
    double wGps =( 1.0 / (m_gpsMSE + 1e-6))*20;
    double wIns = 1.0 / (m_insMSE + 1e-6);
    double total = wGps + wIns;

    fuseX = (wGps * gpsX + wIns * insX) / total;
    fuseY = (wGps * gpsY + wIns * insY) / total;
}

// 核心修改：优化状态初始化，避免直接覆盖
void CDataFusion::KalmanFusion(double gpsX, double gpsY,
    double insX, double insY,
    double dt,
    double& fuseX, double& fuseY) {

    Eigen::VectorXd currentState = m_kalman.GetState();
    // 计算GPS与INS的偏差
    double deltaX = gpsX - insX;
    double deltaY = gpsY - insY;
    double gpsInsDiff = sqrt(deltaX * deltaX + deltaY * deltaY);

    // 若GPS与INS偏差过大，采用INS平滑过渡（不直接用GPS覆盖）
    if (gpsInsDiff > 5.0)
    { // 阈值可根据场景调整
        currentState[0] = 0.5 * currentState[0] + 0.5 * gpsX; // 平滑更新X
        currentState[3] = 0.5 * currentState[3] + 0.5 * gpsY; // 平滑更新Y
        // 增大过程噪声，适应INS漂移
        Eigen::MatrixXd Q = Eigen::MatrixXd::Identity(6, 6) * (0.01 + gpsInsDiff * 0.001);
        m_kalman.SetQ(Q);
    }
    else {
        currentState[0] = insX;
        currentState[3] = insY;
        // 恢复正常过程噪声
        m_kalman.SetQ(Eigen::MatrixXd::Identity(6, 6) * 0.01);
    }

    m_kalman.Init(currentState, m_kalman.GetStateCovariance());
    m_kalman.Predict();
    // 传入残差阈值（根据GPS精度设置，这里设为10米）
    m_kalman.Update(gpsX, gpsY, 10.0);

    Eigen::VectorXd fusedState = m_kalman.GetState();
    fuseX = fusedState[0];
    fuseY = fusedState[3];
}

CDataFusion::AlgType CDataFusion::GetAlgorithm() const {
    return m_alg;
}