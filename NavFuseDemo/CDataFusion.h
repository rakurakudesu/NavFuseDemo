#ifndef DATAFUSION_H
#define DATAFUSION_H

#include "CSensor.h"
#include "CKalmanFilter.h"

/**
 * @file DataFusion.h
 * @brief 数据融合类，实现PPT要求的4种融合算法（加权+卡尔曼）
 * @note 目前包含加权平均、卡尔曼滤波
 */
class CDataFusion {
public:
    /**
     * @brief 融合算法枚举，包含PPT要求的加权+卡尔曼算法
     * @enum WEIGHTED：加权平均，根据误差倒数分配权重
     * @enum KALMAN：卡尔曼滤波，基于动态系统模型的最优估计
     */
    enum AlgType { WEIGHTED, KALMAN, GPS_ONLY};

    /**
     * @brief 构造函数：初始化融合的默认状态
     * @param 无
     * @return 无
     * @调用场景：导航融合演示对话框初始化时实例化
     * @设计思路：默认算法为加权平均，MSE初始为0，初始化卡尔曼滤波实例
     */
    CDataFusion();

    /**
     * @brief 析构函数：释放融合器资源（无动态资源）
     * @param 无
     * @return 无
     */
    ~CDataFusion();

    /**
     * @brief 计算GPS/INS的均方误差（MSE），用于加权融合的权重
     * @param trueX/trueY：真实X/Y坐标(m)（来自CMotionModel）
     * @param gpsX/gpsY：GPS模拟X/Y坐标(m)（来自CGPS）
     * @param insX/insY：INS模拟X/Y坐标(m)（来自CINS）
     * @return 无
     * @调用场景：对话框OnTimer中，FuseData之前调用
     */
    void CalcSensorMSE(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY,double &gpsmse,double &insmse);

    /**
     * @brief 执行数据融合，核心接口，调用对应算法
     * @param trueX/trueY：真实坐标(用于计算融合MSE)
     * @param gpsX/gpsY：GPS数据(传感器1)
     * @param insX/insY：INS数据(传感器2)
     * @param fuseX/fuseY：输出融合结果
     * @return 无
     * @调用场景：对话框OnTimer中，CalcSensorMSE之后调用
     */
    void CDataFusion::FuseData(double trueX, double trueY,
        double gpsX, double gpsY,
        double insX, double insY,
        double& fuseX, double& fuseY,double & fusemse);

    /**
     * @brief 获取各数据的MSE，用于界面显示
     * @param gpsMSE/insMSE/fuseMSE：输出参数，各数据的MSE
     * @return 无
     * @调用场景：对话框OnTimer中，FuseData之后调用
     */
    void GetMSE(double& gpsMSE, double& insMSE, double& fuseMSE);

    /**
     * @brief 设置融合算法
     * @param alg：算法类型（WEIGHTED/KALMAN等）
     * @return 无
     */
    void SetAlgorithm(AlgType alg);

    /**
     * @brief 获取当前融合算法
     * @return 当前算法类型
     */
    AlgType GetAlgorithm() const;

    /**
     * @brief 重置卡尔曼滤波器
     * @param initialState：初始状态向量
     * @param initialP：初始协方差矩阵
     * @return 无
     */
    void ResetKalman(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP)
    {
        m_kalman.Init(initialState, initialP);
    }

private:
    AlgType m_alg;              ///< 当前融合算法类型
    double m_gpsMSE;            ///< GPS的均方误差（MSE）
    double m_insMSE;            ///< INS的均方误差（MSE）
    double m_fuseMSE;           ///< 融合数据的均方误差（MSE）
    CKalmanFilter m_kalman;     ///< 卡尔曼滤波器实例（用于KALMAN算法）

    /**
     * @brief 加权融合算法实现（PPT要求）
     * @param gpsX/gpsY：GPS数据
     * @param insX/insY：INS数据
     * @param fuseX/fuseY：输出融合结果
     * @return 无
     * @调用场景：FuseData中，m_alg=WEIGHTED时调用
     */
    void WeightedFusion(double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

    /**
     * @brief 卡尔曼滤波算法实现（PPT公式）
     * @param gpsX/gpsY：GPS数据(观测值)
     * @param insX/insY：INS数据(状态预测源)
     * @param dt：时间间隔
     * @param fuseX/fuseY：输出融合结果(状态中的位置)
     * @return 无
     * @调用场景：FuseData中，m_alg=KALMAN时调用
     */
    void CDataFusion::KalmanFusion(double gpsX, double gpsY,
        double insX, double insY,
        double dt,
        double& fuseX, double& fuseY);
};

#endif // DATAFUSION_H