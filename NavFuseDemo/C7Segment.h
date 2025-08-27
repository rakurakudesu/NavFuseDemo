#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <afxwin.h> // MFC基础头文件（CStatic、CDC、CPen等）

/**
 * @file SevenSegment.h
 * @brief 数码管显示类，在Picture控件上模拟7段数码管，显示实时位置
 * @note 对应PPT“基本要求-4. 数码管显示数据”，支持GPS/INS/融合数据的数值显示
 * @依赖：无STL库，仅MFC基础控件
 */
class C7Segment {
public:
    /**
     * @brief 构造函数：绑定绘图载体（MFC Picture控件）
     * @param pStatic：指向IDC_STATIC_SEG控件的指针（来自对话框类）
     * @return 无
     * @调用场景：对话框OnInitDialog中，创建显示实例时调用
     * @设计思路：设置单个数码管尺寸（宽30px，高50px），适配控件区域
     */
    C7Segment(CStatic* pStatic);

    /**
     * @brief 析构函数：释放数码管资源（无动态资源）
     * @param 无
     * @return 无
     */
    ~C7Segment();

    /**
     * @brief 设置要显示的数值（6个数值：GPSx/y、INSx/y、融合x/y）
     * @param gpsX：GPS X坐标（保留2位小数）
     * @param gpsY：GPS Y坐标（保留2位小数）
     * @param insX：INS X坐标（保留2位小数）
     * @param insY：INS Y坐标（保留2位小数）
     * @param fuseX：融合 X坐标（保留2位小数）
     * @param fuseY：融合 Y坐标（保留2位小数）
     * @return 无
     * @调用场景：对话框OnTimer中，每次FuseData后调用
     * @设计思路：将数值格式化为带标签的CString（MFC类），供Draw函数使用
     */
    void SetValues(double gpsX, double gpsY, double insX, double insY, double fuseX, double fuseY);

    /**
     * @brief 绘制数码管（模拟7段显示）
     * @param 无
     * @return 无
     * @调用场景：对话框OnTimer中（SetValues后）、OnPaint中（重绘）
     * @设计思路：
     *  1. 分割控件区域为6个显示块（对应6个数值）
     *  2. 按7段规则绘制每个数字（点亮对应段）
     *  3. 绘制数值标签（如“GPS X:”），用颜色区分不同数据
     */
    void Draw();

private:
    CStatic* m_pStatic;       ///< 绑定的MFC Picture控件（绘图载体）
    int m_digitWidth;         ///< 单个数码管宽度（像素，默认30）
    int m_digitHeight;        ///< 单个数码管高度（像素，默认50）
    CString m_gpsXStr;        ///< GPS X坐标显示字符串（带标签）
    CString m_gpsYStr;        ///< GPS Y坐标显示字符串（带标签）
    CString m_insXStr;        ///< INS X坐标显示字符串（带标签）
    CString m_insYStr;        ///< INS Y坐标显示字符串（带标签）
    CString m_fuseXStr;       ///< 融合 X坐标显示字符串（带标签）
    CString m_fuseYStr;       ///< 融合 Y坐标显示字符串（带标签）

    /**
     * @brief 绘制单个数码管（私有工具接口）
     * @param pDC：设备上下文指针（MFC CDC）
     * @param x：数码管左上角X坐标（像素）
     * @param y：数码管左上角Y坐标（像素）
     * @param d：要显示的数字（0-9）
     * @param showDot：是否显示小数点（true=显示）
     * @return 无
     * @调用场景：Draw函数中，绘制每个数字时调用
     * @设计思路：按7段点亮规则绘制对应段，绿色高亮显示
     */
    void DrawDigit(CDC* pDC, int x, int y, int d, bool showDot);

    /**
     * @brief 提取数字的整数/小数部分（私有工具接口）
     * @param val：要解析的数值（如2.34）
     * @param intPart：输出整数部分（2）
     * @param decPart：输出小数部分（34，保留2位）
     * @return 无
     * @调用场景：SetValues中，格式化数值为数码管可显示的数字
     * @设计思路：确保小数部分保留2位，适配数码管显示
     */
    void SplitNumber(double val, int& intPart, int& decPart) {
        intPart = (int)val;                  // 整数部分（如2.34→2）
        decPart = (int)((val - intPart) * 100 + 0.5); // 小数部分（四舍五入到2位）
    }
};

#endif // SEVENSEGMENT_H
