#ifndef SENSOR_H
#define SENSOR_H

#include <random>
#include "CMotionModel.h" // 依赖运动类的真实位置

/**
 * @file Sensor.h
 * @brief 传感器模拟基类+子类（GPS/INS），生成带噪声的模拟数据
 * @note 符合PPT“基本要求-2. GPS/INS数据模拟”，GPS无漂移/INS有漂移
 */
class CSensor {
public:
    double m_freq;       ///< 传感器刷新频率(Hz)
    double m_acc;        ///< 传感器精度(m)
    double m_noiseSigma; ///< 噪声标准差（σ=sqrt(acc)，符合高斯分布N(0,σ²)）
    double m_lastTime;   ///< 上一次生成数据的时间(s)，控制刷新间隔
    std::random_device m_rd; ///< 随机数种子（用于生成高斯噪声）
    std::mt19937 m_gen;  ///< 随机数生成器（C++11，确保噪声随机性）
    double m_driftX;     ///< X方向累积漂移量(m)
    double m_driftY;     ///< Y方向累积漂移量(m)
    double m_driftRate;  ///< 漂移率(m/s)（可调整，默认0.01m/s）

public:
    /**
     * @brief 构造函数：初始化传感器默认参数
     * @param 无
     * @return 无
     * @调用场景：CGPS/CINS子类构造时隐式调用
     * @设计思路：默认频率1Hz、精度1m（噪声σ=1），上次生成时间0s（未启动）
     */
    CSensor() {}

    /**
     * @brief 析构函数：确保子类析构时正确释放（多态基类必加）
     * @param 无
     * @return 无
     * @调用场景：传感器实例销毁时（如对话框关闭）
     * @设计思路：避免子类资源泄漏，符合C++多态规范
     */
    virtual ~CSensor() {}

    /**
     * @brief 设置传感器核心参数（配置接口）
     * @param freq：刷新频率(Hz)（GPS=1-10Hz，INS=10-100Hz，来自界面编辑框）
     * @param acc：精度(m)（GPS=0.5-5m，INS短期=0.1-1m，来自界面编辑框）
     * @return 无
     * @调用场景：对话框“开始”按钮点击时，读取界面参数后调用
     * @设计思路：精度acc对应噪声标准差σ=sqrt(acc)，统一参数入口
     */
    void SetParam(double freq, double acc);

    /**
     * @brief 生成传感器模拟数据（纯虚函数，子类实现差异化逻辑）
     * @param currentTime：当前累计时间(s)（来自对话框m_currentTime，控制刷新频率）
     * @param trueX：真实X坐标(m)（来自CMotionModel，基准值）
     * @param trueY：真实Y坐标(m)（来自CMotionModel，基准值）
     * @param simX：输出参数，模拟X坐标(m)（带噪声/漂移，给融合类）
     * @param simY：输出参数，模拟Y坐标(m)（带噪声/漂移，给融合类）
     * @return bool：数据是否有效（true=新数据；false=未到刷新时间，沿用旧数据）
     * @调用场景：对话框OnTimer事件中，每次定时器触发调用
     * @设计思路：按刷新频率控制数据生成，子类实现GPS/INS的差异化噪声逻辑
     */
    virtual bool GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) = 0;

    /**
     * @brief 生成高斯噪声（保护接口，仅子类调用）
     * @param 无
     * @return double：高斯噪声值（服从N(0, σ²)分布，σ=m_noiseSigma）
     * @调用场景：子类GenerateData中，给真实位置叠加噪声时调用
     * @设计思路：符合PPT“估计误差/观测误差高斯分布”的要求
     */
    double GenerateGaussNoise();
};

/**
 * @brief GPS传感器子类：无累积漂移，仅叠加高斯噪声
 * @note 特性：长期稳定，刷新频率低（如1Hz），符合PPT对GPS的调研要求
 */
class CGPS : public CSensor {
public:
    /**
     * @brief 构造函数：初始化GPS特有参数（当前无特有参数，仅继承基类）
     * @param 无
     * @return 无
     * @调用场景：对话框类CNavFuseDemoDlg初始化时创建实例
     */
    CGPS() {}

    /**
     * @brief 析构函数：释放GPS资源（当前无动态资源）
     * @param 无
     * @return 无
     */
    ~CGPS() {}

    /**
     * @brief 重写生成GPS模拟数据（无漂移，仅加高斯噪声）
     * @param currentTime：当前累计时间(s)（控制刷新频率）
     * @param trueX：真实X坐标(m)（基准值）
     * @param trueY：真实Y坐标(m)（基准值）
     * @param simX：输出GPS模拟X坐标(m)（真实值+高斯噪声）
     * @param simY：输出GPS模拟Y坐标(m)（真实值+高斯噪声）
     * @return bool：数据是否有效（true=新数据；false=沿用旧数据）
     * @调用场景：对话框OnTimer事件中，与CINS轮流调用
     * @设计思路：GPS无漂移，仅按频率生成带高斯噪声的位置数据
     */
    bool GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) override;
};

/**
 * @brief INS传感器子类：短期精确（小噪声）+ 长期累积漂移
 * @note 特性：刷新频率高（如10Hz），符合PPT对INS的调研要求
 */
class CINS : public CSensor {
public:
    /**
     * @brief 构造函数：初始化INS漂移参数
     * @param 无
     * @return 无
     * @调用场景：对话框类CNavFuseDemoDlg初始化时创建实例
     * @设计思路：默认漂移率0.01m/s（每100ms漂移0.001m），初始漂移量0
     */
    CINS() {}

    /**
     * @brief 析构函数：释放INS资源（当前无动态资源）
     * @param 无
     * @return 无
     */
    ~CINS() {}

    /**
     * @brief 重写生成INS模拟数据（高斯噪声+累积漂移）
     * @param currentTime：当前累计时间(s)（控制刷新频率+计算漂移）
     * @param trueX：真实X坐标(m)（基准值）
     * @param trueY：真实Y坐标(m)（基准值）
     * @param simX：输出INS模拟X坐标(m)（真实值+噪声+X方向漂移）
     * @param simY：输出INS模拟Y坐标(m)（真实值+噪声+Y方向漂移）
     * @return bool：数据是否有效（true=新数据；false=沿用旧数据）
     * @调用场景：对话框OnTimer事件中，与CGPS轮流调用
     * @设计思路：漂移随时间累积，刷新频率高于GPS，短期数据更精确
     */
    bool GenerateData(double currentTime, double trueX, double trueY, double& simX, double& simY) override;

    /**
   * @brief 设置INS的漂移率
   * @param driftRate 漂移率（单位：m/s）
   */
    void SetDriftRate(double driftRate);

    void ResetDrift() {
        m_driftX = 0.0;
        m_driftY = 0.0;
    }

};

#endif // SENSOR_H
