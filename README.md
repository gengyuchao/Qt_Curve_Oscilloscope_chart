# Qt_Curve_Oscilloscope_chart
 用于Qt的曲线显示器、示波器图表模块 

# 使用方法

    鼠标滚轮操作：向上放大、向下缩小

    鼠标指针位于左边缘时控制横向缩放

    鼠标指针位于下边缘时控制纵向缩放

```C++

    //设置图表大小
    
    void set_chart(int x, int y, int w, int h);

    //主动刷新图表数据
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
```

# Example

## 使用代码方法
```C++
#include "osc_chart.h"
int number;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    OSC_chart *label_OSC = new OSC_chart(this);
    label_OSC->set_chart(10,20,this->width()-20,this->height()-20);
    label_OSC->Add_Line_Data(0,number);
    label_OSC->View_Chart();
}

```

## 使用UI方法

 在UI界面拉出一个Label控件，设置好大小（需要设置一个最小值），右击控件 -点击-> 提升为 添加 OSC_chart 类型，将 QLabel 提升为 OSC_chart 类型，就可以直接使用了

 ```C++
    ui->label_OSC->Add_Line_Data(0,number0);
    ui->label_OSC->Add_Line_Data(1,number1);
    ui->label_OSC->View_Chart();
 ```

### Frame 
![DemoFrame1](https://github.com/gengyuchao/Qt_Curve_Oscilloscope_chart/blob/master/Image/Osc_Chart_ui.png)

[其他效果图](https://blog.csdn.net/gengyuchao/article/details/125532799)
