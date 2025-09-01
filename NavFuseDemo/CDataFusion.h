#ifndef DATAFUSION_H
#define DATAFUSION_H

#include "CSensor.h"
#include "CKalmanFilter.h"

/**
 * @file DataFusion.h
 * @brief 数据融合类，实现PPT要求的4种融合算法（基础+进阶）
 * @note 基础：加权求和、卡尔曼滤波；进阶：无迹卡尔曼、粒子滤波
 * @依赖：无STL库，仅依赖自定义传感器类和卡尔曼类
 */
    class CDataFusion {
    public:
        /**
         * @brief 融合算法枚举（覆盖PPT要求的基础+进阶算法）
         * @enum WEIGHTED：加权求和（基础，权重与传感器精度相关）
         * @enum KALMAN：卡尔曼滤波（基础，线性系统最优估计）
         * @enum UKF：无迹卡尔曼滤波（进阶，处理非线性系统如圆周/S型运动）
         * @enum PARTICLE：粒子滤波（进阶，处理非高斯噪声）
         */
        enum AlgType { WEIGHTED, KALMAN, UKF, PARTICLE };

        /**
         * @brief 构造函数：初始化融合默认状态
         * @param 无
         * @return 无
         * @调用场景：对话框类CNavFuseDemoDlg初始化时创建实例
         * @设计思路：默认算法为加权求和，MSE初始为0，初始化卡尔曼滤波实例
         */
        CDataFusion();

        /**
         * @brief 析构函数：释放融合资源（无动态资源）
         * @param 无
         * @return 无
         */
        ~CDataFusion();

        /**
         * @brief 设置融合算法（核心配置接口）
         * @param alg：算法类型（WEIGHTED/KALMAN/UKF/PARTICLE，来自界面组合框）
         * @return 无
         * @调用场景：对话框“开始”按钮点击时，读取界面算法选择后调用
         * @设计思路：统一算法入口，后续FuseData自动匹配对应逻辑
         */


         /**
          * @brief 计算GPS/INS的均方误差（MSE），用于加权求和的权重
          * @param trueX：真实X坐标(m)（来自CMotionModel）
          * @param trueY：真实Y坐标(m)（来自CMotionModel）
          * @param gpsX：GPS模拟X坐标(m)（来自CGPS）
          * @param gpsY：GPS模拟Y坐标(m)（来自CGPS）
          * @param insX：INS模拟X坐标(m)（来自CINS）
          * @param insY：INS模拟Y坐标(m)（来自CINS）
          * @return 无
          * @调用场景：对话框OnTimer中，FuseData之前调用
          * @设计思路：MSE=(模拟值-真实值)?（X+Y合计），MSE越小权重越大
          */
        void CalcSensorMSE(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY);

        /**
         * @brief 执行数据融合（核心计算接口，调用对应算法）
         * @param trueX：真实X坐标(m)（用于计算融合MSE）
         * @param trueY：真实Y坐标(m)（用于计算融合MSE）
         * @param gpsX：GPS模拟X坐标(m)（输入数据1）
         * @param gpsY：GPS模拟Y坐标(m)（输入数据1）
         * @param insX：INS模拟X坐标(m)（输入数据2）
         * @param insY：INS模拟Y坐标(m)（输入数据2）
         * @param fuseX：输出融合X坐标(m)（给显示类）
         * @param fuseY：输出融合Y坐标(m)（给显示类）
         * @return 无
         * @调用场景：对话框OnTimer中，CalcSensorMSE之后调用
         * @设计思路：根据m_alg调用对应算法，输出融合结果并计算融合MSE
         */
        void FuseData(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief 获取三类数据的MSE（给界面显示对比）
         * @param gpsMSE：输出GPS的MSE（给界面列表控件）
         * @param insMSE：输出INS的MSE（给界面列表控件）
         * @param fuseMSE：输出融合数据的MSE（给界面列表控件）
         * @return 无
         * @调用场景：对话框OnTimer中，FuseData之后调用
         * @设计思路：隐藏内部MSE变量，通过接口安全输出，便于界面对比算法效果
         */
         // 设置融合算法
        void SetAlgorithm(AlgType alg);

        // 获取当前融合算法
        AlgType GetAlgorithm() const;  // 添加这一行



        void GetMSE(double& gpsMSE, double& insMSE, double& fuseMSE);
    private:
        AlgType m_alg;              ///< 当前融合算法类型
        double m_gpsMSE;            ///< GPS的均方误差（MSE）
        double m_insMSE;            ///< INS的均方误差（MSE）
        double m_fuseMSE;           ///< 融合数据的均方误差（MSE）
        CKalmanFilter m_kalman;     ///< 卡尔曼滤波实例（组合关系，用于KALMAN算法）

        /**
         * @brief 加权求和算法（基础算法，PPT要求）
         * @param gpsX/gpsY：GPS数据
         * @param insX/insY：INS数据
         * @param fuseX/fuseY：输出融合结果
         * @return 无
         * @调用场景：FuseData中，m_alg=WEIGHTED时调用
         * @设计思路：权重=1/MSE，融合值=GPS*w_gps + INS*w_ins
         */
        void WeightedFusion(double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief 卡尔曼滤波算法（基础算法，PPT公式落地）
         * @param trueX/trueY：真实值（用于计算融合MSE）
         * @param gpsX/gpsY：GPS数据（观测值）
         * @param insX/insY：INS数据（状态预测输入）
         * @param fuseX/fuseY：输出融合结果（卡尔曼估计的位置）
         * @return 无
         * @调用场景：FuseData中，m_alg=KALMAN时调用
         * @设计思路：用INS加速度预测，GPS观测修正，提取状态向量中的位置作为结果
         */
        void KalmanFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief 无迹卡尔曼滤波（进阶算法，处理非线性系统）
         * @param trueX/trueY：真实值（用于计算融合MSE）
         * @param gpsX/gpsY：GPS数据（观测值）
         * @param insX/insY：INS数据（状态预测输入）
         * @param fuseX/fuseY：输出融合结果
         * @return 无
         * @调用场景：FuseData中，m_alg=UKF时调用
         * @设计思路：用UT变换近似高斯分布，解决PPT“非线性运动处理”问题
         */
        void UKFFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief 粒子滤波（进阶算法，处理非高斯噪声）
         * @param trueX/trueY：真实值（用于计算融合MSE）
         * @param gpsX/gpsY：GPS数据（观测值）
         * @param insX/insY：INS数据（状态预测输入）
         * @param fuseX/fuseY：输出融合结果
         * @return 无
         * @调用场景：FuseData中，m_alg=PARTICLE时调用
         * @设计思路：用粒子集近似状态分布，适合非高斯噪声场景
         */
        void ParticleFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);
};

#endif // DATAFUSION_H