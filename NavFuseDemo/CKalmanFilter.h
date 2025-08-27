#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <Eigen/Dense> // ����Eigen�⣨�����ù�������Eigen��

/**
 * @file KalmanFilter.h
 * @brief �������˲������࣬ʵ��PPTҪ��ġ����ڿ������˲����������ںϡ�
 * @note ����CDataFusion���ڲ�����������ϵͳ����ֱ���˶�����״̬����
 */
class CKalmanFilter {
public:
    /**
     * @brief ���캯������ʼ���������˲�Ĭ��״̬
     * @param ��
     * @return ��
     * @���ó�����CDataFusion�๹��ʱ����ʵ��
     * @���˼·����ʼ״̬����ȫ0��Э�������Ϊ��λ����
     */
    CKalmanFilter();

    /**
     * @brief �����������ͷſ������˲���Դ����ǰ�޶�̬��Դ��
     * @param ��
     * @return ��
     */
    ~CKalmanFilter();

    /**
     * @brief ���ÿ������˲����ľ���PPT��ʽ������
     * @param F��״̬ת�ƾ���6x6��2D�ȼ���ģ�ͣ�λ��/�ٶ�/���ٶȣ�
     * @param H���۲����2x6������ȡλ��x[0],x[3]��Ϊ�۲�ֵ��
     * @param Q����������Э���6x6��INS���ٶ���������0.01*I��
     * @param R���۲�����Э���2x2��GPS���ȣ���1.0*I��
     * @return ��
     * @���ó�����CDataFusion���캯���г�ʼ��
     * @���˼·����������������ڸ��ݴ��������Ե������ζȣ�Q��=���ι۲���ࣩ
     */
    void SetParam(Eigen::MatrixXd F, Eigen::MatrixXd H, Eigen::MatrixXd Q, Eigen::MatrixXd R);

    /**
     * @brief ������Ԥ�ⲽ��PPT��ʽ��x_t = Fx_{t-1} + Bu + w_t��
     * @param ��
     * @return ��
     * @���ó�����CDataFusion::KalmanFusion�У���ִ��Ԥ��
     * @���˼·��
     *  - ״̬Ԥ�⣺m_x = F * m_x����Bu=0�����ⲿ�������룩
     *  - Э����Ԥ�⣺m_P = F * m_P * F^T + Q�����ӹ���������
     */
    void Predict();

    /**
     * @brief ���������²���PPT��ʽ��z_t = Hx_t + v_t��x = x + Ky��
     * @param zX��GPS�۲�X���꣨z_t��X������
     * @param zY��GPS�۲�Y���꣨z_t��Y������
     * @return ��
     * @���ó�����CDataFusion::KalmanFusion�У�Ԥ���ִ�и���
     * @���˼·����GPS�۲�ֵ����Ԥ��ֵ��ƽ��Ԥ����۲�����ζ�
     */
    void Update(double zX, double zY);

    /**
     * @brief ��ȡ��ǰ״̬��������CDataFusion��ȡλ�ã�
     * @param ��
     * @return Eigen::VectorXd��״̬����x��6ά��x,vx,ax,y,vy,ay��
     * @���ó�����CDataFusion::KalmanFusion�У����º���ȡ�ں�λ��
     * @���˼·�������ڲ�״̬����ͨ���ӿ������Ҫ��Ϣ��λ�ã�
     */
    Eigen::VectorXd GetState() const;

private:
    Eigen::MatrixXd m_F; ///< ״̬ת�ƾ���6x6��
    Eigen::MatrixXd m_H; ///< �۲����2x6��
    Eigen::MatrixXd m_Q; ///< ��������Э�������6x6��
    Eigen::MatrixXd m_R; ///< �۲�����Э�������2x2��
    Eigen::VectorXd m_x; ///< ״̬������6ά��x,vx,ax,y,vy,ay��
    Eigen::MatrixXd m_P; ///< ״̬Э�������6x6��
};

#endif // KALMANFILTER_H
