/*
    MIT License

    Copyright (c) 2019 gengyuchao https://github.com/gengyuchao

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

 */

#ifndef OSC_CHART_H
#define OSC_CHART_H

//#include <QWidget>
#include "QLabel"
#include "QPainter"
#include "QMouseEvent"
#include "QWheelEvent"
#include "QList"
//重要提示：使用ui设计时一定要设置最小值，否则无法随窗口改变大小
class OSC_chart : public QLabel
{
    Q_OBJECT

private:
    //是否隐藏指针label
    bool hide_pixmap=false;
    //坐标点显示
    bool viewPos=true;
    //坐标点分度值
    int Pots_multiple=10;
    int Pots_multiple_x=10;
    int Pots_multiple_y=10;
    //示波器放大倍数
    double OSC_multiple=50;
    double OSC_multiple_x=10;
    double OSC_multiple_y=10;
    //x，y轴虚线显示的条数（虚线间隔相等）
    int Step_x=10;
    int Step_y=5;

    QPixmap pixmap;//画布
    QPainter painter;//绘图

    QColor Color_Box[10]={Qt::black,Qt::red,Qt::green,Qt::blue,Qt::cyan,
                          Qt::magenta,Qt::yellow,Qt::gray,Qt::darkRed,Qt::darkBlue};


    void Draw_Wave(QList<int> chart_x,QList<int> chart_y);
    void Draw_Chart();
    void Draw_Cross_pointer(int x,int y);



public:
    //explicit OSC_chart(QWidget *parent = nullptr);
    OSC_chart( QWidget * parent = 0);
    ~OSC_chart(){}
    //外联函数 是否隐藏指针label
    void sethide(bool hide);
    void set_chart(int x, int y, int w, int h);
    void View_Chart();

    void Set_Line_name(int Line_num,QString name);
    void Add_Line_Data(int Line_num,int pos_y);
    void Add_Line_Data_XY(int Line_num,int pos_x,int pos_y);
    //鼠标事件
    QMouseEvent *externevent;

    QString line_name[16];
    QList<int> data_x[16];
    QList<int> data_y[16];
    double Set_Chart_Auto(void);



    int start_x=0;  // 鼠标左键按下时，鼠标指针的点距离原点的位置x
    int start_y=0;  // 鼠标左键按下时，鼠标指针的点距离原点的位置y
    int offset_x=0; // 鼠标左键拖拽移动距离原点的位置x
    int offset_y=0; // 鼠标左键拖拽移动距离原点的位置y
    int start_flag=0;
    int MAX_INT_VALUE = INT32_MAX;
signals:
    void clicked(QMouseEvent *event);
    void moveing(QMouseEvent *event);
    void doubleClicked(QMouseEvent *event);

public slots:
    void slotClicked(QMouseEvent *event);
    void slotmoveing(QMouseEvent *event);
    void slotdoubleClicked(QMouseEvent *event);

protected:
    public:
        void mousePressEvent ( QMouseEvent * event ) ;
        void mouseMoveEvent( QMouseEvent * event ) ;
        void mouseDoubleClickEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent  *event);
        void mouseReleaseEvent(QMouseEvent  *event);
        void resizeEvent(QResizeEvent *event);
};

#endif // OSC_CHART_H
