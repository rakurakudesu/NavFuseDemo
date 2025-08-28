/**
 * @file CMotionModel.h
 * @brief 机器人运动模拟类，生成无噪声的真实位置数据（直线/圆周/S型曲线）
 * @note 对应PPT“基本要求-1. 3种运动方式选择”，为传感器模拟提供基准位置
 */
#ifndef CMOTIONMODEL_H
#define CMOTIONMODEL_H

class CMotionModel {
public:
    /**
     * @brief 运动类型枚举（覆盖PPT要求的3种运动方式）
     * @enum LINE：直线运动（需速度参数）
     * @enum ARC：圆周运动（需半径、角速度参数）
     * @enum S_CURVE：S型曲线运动（需振幅参数）
     */
    enum MotionType { LINE, ARC, S_CURVE };

    /**
     * @brief 构造函数：初始化运动默认状态
     * @param 无
     * @return 无
     * @调用场景：对话框类CNavFuseDemoDlg初始化时创建实例
     * @设计思路：默认运动为直线，初始位置(0,0)，累计时间从0s开始
     */
    CMotionModel();

    /**
     * @brief 析构函数：释放运动模型资源（当前无动态资源，仅作规范）
     * @param 无
     * @return 无
     * @调用场景：实例销毁时（如对话框关闭）
     */
    ~CMotionModel();

    /**
     * @brief 获取当前实例的指针（新增函数）
     * @return CMotionModel*：当前对象的指针
     * @调用场景：外部类需要直接访问当前实例时使用
     * @注意：指针有效期与当前实例一致，实例销毁后指针失效
     */
    CMotionModel* GetInstancePtr() { return this; }

    /**
     * @brief 设置运动类型及对应参数（核心配置接口）
     * @param type：运动类型（LINE/ARC/S_CURVE，来自界面单选按钮）
     * @param param1：主参数（LINE=速度(m/s)；ARC=半径(m)；S_CURVE=振幅(m)）
     * @param param2：副参数（仅ARC需角速度(rad/s)，其他运动传0即可）
     * @return 无
     * @调用场景：对话框“开始”按钮点击时，读取界面参数后调用
     * @设计思路：统一参数入口，避免为不同运动单独设计配置函数
     */
    void SetMotionParam(MotionType type, double param1, double param2 = 0.0);

    /**
     * @brief 按时间步长更新真实位置（核心计算接口）
     * @param dt：时间步长(s)，等于定时器间隔（默认0.1s，即10Hz刷新）
     * @return 无
     * @调用场景：对话框OnTimer事件中，每次定时器触发调用
     * @设计思路：
     *  - 直线运动：x = 速度 * 累计时间，y=0（匀速）
     *  - 圆周运动：x=半径*cos(角速度*累计时间)，y=半径*sin(角速度*累计时间)
     *  - S型曲线：x=累计时间，y=振幅*sin(累计时间)（正弦轨迹）
     */
    void UpdateTruePos(double dt);

    /**
     * @brief 获取当前真实位置（数据输出接口）
     * @param x：输出参数，真实X坐标(m)（引用传递，给传感器类传基准值）
     * @param y：输出参数，真实Y坐标(m)（引用传递，给传感器类传基准值）
     * @return 无
     * @调用场景：对话框OnTimer中，更新位置后调用，给CGPS/CINS传真实值
     * @设计思路：隐藏内部位置变量，通过接口安全输出，符合封装原则
     */
    void GetTruePos(double& x, double& y);

    MotionType m_type;    ///< 当前运动类型（LINE/ARC/S_CURVE）
    double m_time;        ///< 运动累计时间(s)，用于计算位置
    double m_x;           ///< 真实X坐标(m)
    double m_y;           ///< 真实Y坐标(m)
    double m_radius;      ///< 圆周运动半径(m)（仅ARC类型使用）
    double m_omega;       ///< 圆周运动角速度(rad/s)（仅ARC类型使用）
    double m_sAmp;        ///< S型曲线振幅(m)（仅S_CURVE类型使用）
    double m_lineSpeed;   ///< 直线运动速度(m/s)（仅LINE类型使用）
};

#endif // CMOTIONMODEL_H