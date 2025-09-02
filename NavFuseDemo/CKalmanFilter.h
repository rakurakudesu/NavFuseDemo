#pragma once
#include <Eigen/Dense>

class CKalmanFilter {
public:
    CKalmanFilter();
    ~CKalmanFilter();

    void SetParam(Eigen::MatrixXd F, Eigen::MatrixXd H, Eigen::MatrixXd Q, Eigen::MatrixXd R);
    void Init(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP);
    void Predict();
    // �����в���ֵ����
    void Update(double zX, double zY, double maxResidual = 10.0);
    void UpdateINS(double insX, double insY, double dt);
    Eigen::VectorXd GetState() const;
    Eigen::MatrixXd GetStateCovariance() const { return m_P; }
    void InitState(double initX, double initY);
    // ������̬���������Ľӿ�
    void SetR(const Eigen::MatrixXd& R) { m_R = R; }
    void SetQ(const Eigen::MatrixXd& Q) { m_Q = Q; }

private:
    Eigen::VectorXd m_x; // ״̬����
    Eigen::MatrixXd m_P; // ״̬Э�������
    Eigen::MatrixXd m_F; // ״̬ת�ƾ���
    Eigen::MatrixXd m_H; // �۲����
    Eigen::MatrixXd m_Q; // ��������Э����
    Eigen::MatrixXd m_R; // �۲�����Э����
};