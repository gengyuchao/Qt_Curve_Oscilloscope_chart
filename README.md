# Qt_Curve_Oscilloscope_chart
 用于Qt的曲线显示器、示波器图表模块 

# 使用方法

    鼠标滚轮操作：**向上**放大、**向下**缩小

    鼠标指针位于**左边缘**时控制**横向**缩放

    鼠标指针位于**下边缘**时控制**纵向**缩放

(```)
    //设置图表大小
    
    void set_chart(int x, int y, int w, int h);

    //手动刷新图表数据
    void View_Chart();

    //按线的标号，连续画线，新坐标直接添加在上一坐标后面
    void Add_Line_Data(int Line_num,int pos_y);

    //按线的标号，指定x，y坐标，添加数据
    void Add_Line_Data_XY(int Line_num,int pos_x,int pos_y);

    //坐标数据链表存储
    QList<int> data_x[16];
    QList<int> data_y[16];

    //自动设置最新100个数据的纵坐标范围，让数据完全显示在屏幕上。
    double Set_Chart_Auto(void);

    //隐藏鼠标指针的操作
    void sethide(bool hide);
(```)

