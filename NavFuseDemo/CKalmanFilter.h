#pragma once
#include <Eigen/Dense>

class CKalmanFilter {
public:
    CKalmanFilter();
    ~CKalmanFilter();

    void SetParam(Eigen::MatrixXd F, Eigen::MatrixXd H, Eigen::MatrixXd Q, Eigen::MatrixXd R);
    void Init(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP);
    void Predict();
    // 新增残差阈值参数
    void Update(double zX, double zY, double maxResidual = 10.0);
    void UpdateINS(double insX, double insY, double dt);
    Eigen::VectorXd GetState() const;
    Eigen::MatrixXd GetStateCovariance() const { return m_P; }
    void InitState(double initX, double initY);
    // 新增动态调整噪声的接口
    void SetR(const Eigen::MatrixXd& R) { m_R = R; }
    void SetQ(const Eigen::MatrixXd& Q) { m_Q = Q; }

private:
    Eigen::VectorXd m_x; // 状态向量
    Eigen::MatrixXd m_P; // 状态协方差矩阵
    Eigen::MatrixXd m_F; // 状态转移矩阵
    Eigen::MatrixXd m_H; // 观测矩阵
    Eigen::MatrixXd m_Q; // 过程噪声协方差
    Eigen::MatrixXd m_R; // 观测噪声协方差
};