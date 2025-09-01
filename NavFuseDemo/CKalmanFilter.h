#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <Eigen/Dense> // 包含Eigen库（用于矩阵运算）

/**
 * @file KalmanFilter.h
 * @brief 卡尔曼滤波器类，实现PPT要求的、用于数据融合的卡尔曼滤波算法。
 * @note 被CDataFusion类内部使用，处理运动系统的状态估计
 */
class CKalmanFilter {
public:
    /**
     * @brief 构造函数：初始化卡尔曼滤波的默认状态
     * @param 无
     * @return 无
     * @调用场景：CDataFusion构造时实例化
     * @设计思路：初始状态向量全为0，协方差矩阵为单位矩阵
     */
    CKalmanFilter();

    /**
     * @brief 析构函数：释放卡尔曼滤波器资源（无动态资源）
     * @param 无
     * @return 无
     */
    ~CKalmanFilter();

    void Init(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP);

    /**
     * @brief 设置卡尔曼滤波器的参数（PPT公式中的矩阵）
     * @param F：状态转移矩阵6x6（2D运动模型：位置/速度/加速度）
     * @param H：观测矩阵2x6（只取位置x[0],x[3]作为观测值）
     * @param Q：过程噪声协方差（6x6，INS噪声，0.01*I）
     * @param R：观测噪声协方差（2x2，GPS噪声，1.0*I）
     * @return 无
     * @调用场景：CDataFusion构造函数中初始化
     */
    void SetParam(Eigen::MatrixXd F, Eigen::MatrixXd H, Eigen::MatrixXd Q, Eigen::MatrixXd R);

    /**
     * @brief 执行预测步骤（PPT公式：x_t = Fx_{t-1} + Bu + w_t）
     * @param 无
     * @return 无
     * @调用场景：CDataFusion::KalmanFusion中，先执行预测
     */
    void Predict();

    /**
     * @brief 执行更新步骤（PPT公式：z_t = Hx_t + v_t；x = x + Ky）
     * @param zX：GPS观测X坐标（z_t的X分量）
     * @param zY：GPS观测Y坐标（z_t的Y分量）
     * @return 无
     * @调用场景：CDataFusion::KalmanFusion中，预测后执行更新
     */
    void Update(double zX, double zY);

    /**
     * @brief 使用INS数据更新状态（补充预测阶段的修正）
     * @param insX：INS测量的X坐标
     * @param insY：INS测量的Y坐标
     * @param dt：时间间隔
     * @return 无
     */
    void UpdateINS(double insX, double insY, double dt);

    /**
     * @brief 获取当前状态（供CDataFusion获取位置）
     * @param 无
     * @return Eigen::VectorXd：状态向量6维（x,vx,ax,y,vy,ay）
     * @调用场景：CDataFusion::KalmanFusion中，获取融合位置
     */
    Eigen::VectorXd GetState() const;
    Eigen::MatrixXd GetStateCovariance() const { return m_P; }
    void InitState(double initX, double initY);  // 初始化位置状态

private:
    Eigen::MatrixXd m_F; ///< 状态转移矩阵6x6
    Eigen::MatrixXd m_H; ///< 观测矩阵2x6
    Eigen::MatrixXd m_Q; ///< 过程噪声协方差矩阵6x6
    Eigen::MatrixXd m_R; ///< 观测噪声协方差矩阵2x2
    Eigen::VectorXd m_x; ///< 状态向量6维（x,vx,ax,y,vy,ay）
    Eigen::MatrixXd m_P; ///< 状态协方差矩阵6x6
};

#endif // KALMANFILTER_H
