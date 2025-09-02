#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <Eigen/Dense> // ����Eigen�⣨���ھ������㣩

/**
 * @file KalmanFilter.h
 * @brief �������˲����࣬ʵ��PPTҪ��ġ����������ںϵĿ������˲��㷨��
 * @note ��CDataFusion���ڲ�ʹ�ã������˶�ϵͳ��״̬����
 */
class CKalmanFilter {
public:
    /**
     * @brief ���캯������ʼ���������˲���Ĭ��״̬
     * @param ��
     * @return ��
     * @���ó�����CDataFusion����ʱʵ����
     * @���˼·����ʼ״̬����ȫΪ0��Э�������Ϊ��λ����
     */
    CKalmanFilter();

    /**
     * @brief �����������ͷſ������˲�����Դ���޶�̬��Դ��
     * @param ��
     * @return ��
     */
    ~CKalmanFilter();

    void Init(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP);

    /**
     * @brief ���ÿ������˲����Ĳ�����PPT��ʽ�еľ���
     * @param F��״̬ת�ƾ���6x6��2D�˶�ģ�ͣ�λ��/�ٶ�/���ٶȣ�
     * @param H���۲����2x6��ֻȡλ��x[0],x[3]��Ϊ�۲�ֵ��
     * @param Q����������Э���6x6��INS������0.01*I��
     * @param R���۲�����Э���2x2��GPS������1.0*I��
     * @return ��
     * @���ó�����CDataFusion���캯���г�ʼ��
     */
    void SetParam(Eigen::MatrixXd F, Eigen::MatrixXd H, Eigen::MatrixXd Q, Eigen::MatrixXd R);

    /**
     * @brief ִ��Ԥ�ⲽ�裨PPT��ʽ��x_t = Fx_{t-1} + Bu + w_t��
     * @param ��
     * @return ��
     * @���ó�����CDataFusion::KalmanFusion�У���ִ��Ԥ��
     */
    void Predict();

    /**
     * @brief ִ�и��²��裨PPT��ʽ��z_t = Hx_t + v_t��x = x + Ky��
     * @param zX��GPS�۲�X���꣨z_t��X������
     * @param zY��GPS�۲�Y���꣨z_t��Y������
     * @return ��
     * @���ó�����CDataFusion::KalmanFusion�У�Ԥ���ִ�и���
     */
    void Update(double zX, double zY);

    /**
     * @brief ʹ��INS���ݸ���״̬������Ԥ��׶ε�������
     * @param insX��INS������X����
     * @param insY��INS������Y����
     * @param dt��ʱ����
     * @return ��
     */
    void UpdateINS(double insX, double insY, double dt);

    /**
     * @brief ��ȡ��ǰ״̬����CDataFusion��ȡλ�ã�
     * @param ��
     * @return Eigen::VectorXd��״̬����6ά��x,vx,ax,y,vy,ay��
     * @���ó�����CDataFusion::KalmanFusion�У���ȡ�ں�λ��
     */
    Eigen::VectorXd GetState() const;
    Eigen::MatrixXd GetStateCovariance() const { return m_P; }
    void InitState(double initX, double initY);  // ��ʼ��λ��״̬

private:
    Eigen::MatrixXd m_F; ///< ״̬ת�ƾ���6x6
    Eigen::MatrixXd m_H; ///< �۲����2x6
    Eigen::MatrixXd m_Q; ///< ��������Э�������6x6
    Eigen::MatrixXd m_R; ///< �۲�����Э�������2x2
    Eigen::VectorXd m_x; ///< ״̬����6ά��x,vx,ax,y,vy,ay��
    Eigen::MatrixXd m_P; ///< ״̬Э�������6x6
};

#endif // KALMANFILTER_H
