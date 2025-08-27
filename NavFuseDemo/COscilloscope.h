#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include <afxwin.h> // MFC基础头文件（CStatic、CPoint、CRect等）

/**
 * @file Oscilloscope.h
 * @brief 示波器显示类，在Picture控件上动态绘制3类数据曲线
 * @note 对应PPT“基本要求-4. 模拟示波器显示数据”，支持GPS/INS/融合数据可视化
 * @依赖：无STL库，用自定义结构体+固定数组替代vector<tuple>
 */

 // 自定义数据点结构体（替代std::tuple，存储单条数据）
struct DataPoint {
    double time;   ///< 时间(s)（X轴）
    double gpsX;   ///< GPS X坐标(m)
    double gpsY;   ///< GPS Y坐标(m)
    double insX;   ///< INS X坐标(m)
    double insY;   ///< INS Y坐标(m)
    double fuseX;  ///< 融合 X坐标(m)
    double fuseY;  ///< 融合 Y坐标(m)
};

class COscilloscope {
public:
    // 固定参数：最大数据点数量（替代vector动态扩容，足够实验使用）
    static const int MAX_DATA_POINTS = 100;

    /**
     * @brief 构造函数：绑定绘图载体（MFC Picture控件）
     * @param pStatic：指向IDC_STATIC_OSC控件的指针（来自对话框类）
     * @return 无
     * @调用场景：对话框OnInitDialog中，创建显示实例时调用
     * @设计思路：与控件绑定确定绘图区域，用固定数组存储数据（最大100点）
     */
    COscilloscope(CStatic* pStatic);

    /**
     * @brief 析构函数：释放示波器资源（无动态资源）
     * @param 无
     * @return 无
     */
    ~COscilloscope();

    /**
     * @brief 添加数据点到缓存（用于绘制曲线）
     * @param t：当前时间(s)（X轴坐标）
     * @param x1/y1：GPS数据（红曲线）
     * @param x2/y2：INS数据（蓝曲线）
     * @param x3/y3：融合数据（绿曲线）
     * @return 无
     * @调用场景：对话框OnTimer中，每次FuseData后调用
     * @设计思路：用循环数组存储，超过最大点数时覆盖最早数据（FIFO）
     */
    void AddDataPoint(double t, double x1, double y1, double x2, double y2, double x3, double y3);

    /**
     * @brief 绘制示波器曲线（双缓冲避免闪烁）
     * @param 无
     * @return 无
     * @调用场景：对话框OnTimer中（AddDataPoint后）、OnPaint中（重绘）
     * @设计思路：
     *  1. 双缓冲绘图：先画到内存DC，再复制到屏幕DC，避免闪烁
     *  2. 绘制元素：坐标轴、网格线、3条数据曲线（GPS红/INS蓝/融合绿）
     */
    void Draw();

private:
    CStatic* m_pStatic;         ///< 绑定的MFC Picture控件（绘图载体）
    DataPoint m_data[MAX_DATA_POINTS]; ///< 固定数组存储数据点（替代vector）
    int m_dataCount;            ///< 当前数据点数量（0~MAX_DATA_POINTS）
    int m_dataIndex;            ///< 下一个数据点的存储索引（循环数组用）

    /**
     * @brief 数据坐标→像素坐标映射（私有工具接口）
     * @param t：数据时间（X轴数据）
     * @param val：数据位置（Y轴数据）
     * @param rect：控件客户区矩形（像素范围）
     * @param tMin/tMax：时间范围（X轴数据范围）
     * @param valMin/valMax：位置范围（Y轴数据范围）
     * @return CPoint：映射后的像素坐标（x,y）
     * @调用场景：Draw函数中，将数据点转为控件像素点
     * @设计思路：确保所有数据点适配控件大小，不超出显示区域
     */
    CPoint MapToPixel(double t, double val, CRect& rect, double tMin, double tMax, double valMin, double valMax);

    /**
     * @brief 获取数据缓存中的时间/位置范围（私有工具接口）
     * @param tMin/tMax：输出时间范围（X轴）
     * @param valMin/valMax：输出位置范围（Y轴，含GPS/INS/融合）
     * @return 无
     * @调用场景：Draw函数中，计算坐标映射的范围
     * @设计思路：遍历数组获取最大/最小值，确保所有数据在显示范围内
     */
    void GetDataRange(double& tMin, double& tMax, double& valMin, double& valMax);
};

#endif // OSCILLOSCOPE_H