#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <Eigen/Dense> // 依赖Eigen库（需配置工程链接Eigen）

/**
 * @file KalmanFilter.h
 * @brief 卡尔曼滤波工具类，实现PPT要求的“基于卡尔曼滤波器的数据融合”
 * @note 用于CDataFusion类内部，处理线性系统（如直线运动）的状态估计
 */
class CKalmanFilter {
public:
    /**
     * @brief 构造函数：初始化卡尔曼滤波默认状态
     * @param 无
     * @return 无
     * @调用场景：CDataFusion类构造时创建实例
     * @设计思路：初始状态向量全0，协方差矩阵为单位矩阵
     */
    CKalmanFilter();

    /**
     * @brief 析构函数：释放卡尔曼滤波资源（当前无动态资源）
     * @param 无
     * @return 无
     */
    ~CKalmanFilter();

    /**
     * @brief 设置卡尔曼滤波核心矩阵（PPT公式参数）
     * @param F：状态转移矩阵（6x6，2D匀加速模型：位置/速度/加速度）
     * @param H：观测矩阵（2x6，仅提取位置x[0],x[3]作为观测值）
     * @param Q：过程噪声协方差（6x6，INS加速度噪声，如0.01*I）
     * @param R：观测噪声协方差（2x2，GPS精度，如1.0*I）
     * @return 无
     * @调用场景：CDataFusion构造函数中初始化
     * @设计思路：矩阵参数化，便于根据传感器特性调整信任度（Q大=信任观测更多）
     */
    void SetParam(Eigen::MatrixXd F, Eigen::MatrixXd H, Eigen::MatrixXd Q, Eigen::MatrixXd R);

    /**
     * @brief 卡尔曼预测步（PPT公式：x_t = Fx_{t-1} + Bu + w_t）
     * @param 无
     * @return 无
     * @调用场景：CDataFusion::KalmanFusion中，先执行预测
     * @设计思路：
     *  - 状态预测：m_x = F * m_x（简化Bu=0，无外部控制输入）
     *  - 协方差预测：m_P = F * m_P * F^T + Q（叠加过程噪声）
     */
    void Predict();

    /**
     * @brief 卡尔曼更新步（PPT公式：z_t = Hx_t + v_t；x = x + Ky）
     * @param zX：GPS观测X坐标（z_t的X分量）
     * @param zY：GPS观测Y坐标（z_t的Y分量）
     * @return 无
     * @调用场景：CDataFusion::KalmanFusion中，预测后执行更新
     * @设计思路：用GPS观测值修正预测值，平衡预测与观测的信任度
     */
    void Update(double zX, double zY);

    /**
     * @brief 获取当前状态向量（给CDataFusion提取位置）
     * @param 无
     * @return Eigen::VectorXd：状态向量x（6维：x,vx,ax,y,vy,ay）
     * @调用场景：CDataFusion::KalmanFusion中，更新后提取融合位置
     * @设计思路：隐藏内部状态，仅通过接口输出必要信息（位置）
     */
    Eigen::VectorXd GetState() const;

private:
    Eigen::MatrixXd m_F; ///< 状态转移矩阵（6x6）
    Eigen::MatrixXd m_H; ///< 观测矩阵（2x6）
    Eigen::MatrixXd m_Q; ///< 过程噪声协方差矩阵（6x6）
    Eigen::MatrixXd m_R; ///< 观测噪声协方差矩阵（2x2）
    Eigen::VectorXd m_x; ///< 状态向量（6维：x,vx,ax,y,vy,ay）
    Eigen::MatrixXd m_P; ///< 状态协方差矩阵（6x6）
};

#endif // KALMANFILTER_H
