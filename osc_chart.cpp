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

#include "osc_chart.h"
#include "QPixmap"
#include "QPainter"
#include "QtMath"

//int chart_x[10000];
//int chart_y[10000];


#include "QDebug"



OSC_chart::OSC_chart( QWidget * parent )
    :QLabel(parent)
{

    set_chart(0, 0, parent->size().width(),parent->size().height());

    connect(this, SIGNAL(clicked( QMouseEvent* )), this, SLOT(slotClicked(QMouseEvent*)));
    connect(this, SIGNAL(moveing( QMouseEvent* )), this, SLOT(slotmoveing(QMouseEvent*)));
    connect(this, SIGNAL(doubleClicked( QMouseEvent* )), this, SLOT(slotdoubleClicked(QMouseEvent*)));
    this->setMouseTracking(true);
    // 1.QLabel是放在布局里面的，所以无法直接调节QLabel大小，只能设置窗口大小。
    // 2.主窗口调节也受限也子窗口，假如子窗口无法进行调节的话那么主窗口也无法调节。
    // 3.实际加载图片的时候发现对窗口调用resize只能放大窗口，缩小无效。 把QLabel的sizePolicy设置为Ignored就可以自由放大缩小了。
    this->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

    OSC_multiple=50;
}

void OSC_chart::resizeEvent(QResizeEvent *event)
{
    painter.end();
    pixmap=QPixmap(this->width()-this->lineWidth()*2,this->height()-this->lineWidth()*2);//画布
    pixmap.fill(Qt::transparent);
    painter.begin(&pixmap);//绘图
    Draw_Chart();
    setPixmap(pixmap);
}

//设置图标大小 初始化绘图设备 (首次初始化使用 ，可设为私有函数并去除else部分 因为在resizeEvent已经实现)
void OSC_chart::set_chart(int x, int y, int w, int h)
{

    if(pixmap.isNull()==true)
    {
        this->setGeometry(x,y,w,h);
        pixmap=QPixmap(this->width(),this->height());//画布
        pixmap.fill(Qt::transparent);
        painter.begin(&pixmap);//绘图
        //painter.setRenderHint(QPainter::Antialiasing, true); //反走样//效果差
        Draw_Chart();
        setPixmap(pixmap);
    }
    else
    {
        this->setGeometry(x,y,w,h);
        painter.end();
        pixmap=QPixmap(w,h);//画布
        pixmap.fill(Qt::transparent);
        painter.begin(&pixmap);//绘图
        Draw_Chart();
        setPixmap(pixmap);
    }


}



//查看图表，相当于激活一次鼠标移动槽
void OSC_chart::View_Chart()
{
    //emit moveing(externevent);
    pixmap.fill(Qt::transparent);
    Draw_Chart();
    setPixmap(pixmap);

}

//设置坐标指针隐藏 参数：true为隐藏 false不隐藏
void OSC_chart::sethide(bool hide)
{
    hide_pixmap=hide;
}

//求范围公式 舍去输入值除最高位以外的值
int arange(int x)
{
    int i=0;
    while(x>10)
    {
        x=x/10;
        i++;
    }
    while(i>0)
    {
        x=x*10;
        i--;
    }
    return x;
}

//鼠标滚轮事件
void OSC_chart::wheelEvent(QWheelEvent  *event)
{

    static int last_Pots_multiple_x,last_Pots_multiple_y;
    last_Pots_multiple_x=Pots_multiple_x;
    last_Pots_multiple_y=Pots_multiple_y;
#if (QT_VERSION <= QT_VERSION_CHECK(6,0,0))
    QPointF position = event->pos();
    int wheel_delta = event->delta();
#else
    QPointF position = event->position();
    int wheel_delta = event->angleDelta().y();
#endif
    if(position.x()<30)
    {
        double OSC_multiple_x_last=OSC_multiple_x;
        if(wheel_delta>0)
        {

            //以10为基础开始 自动计算每次缩放的大小
            OSC_multiple_x=OSC_multiple_x*11/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int weight=(int)(this->width()/OSC_multiple_x)/Step_x;
            if(last_Pots_multiple_x/2>=arange(weight))//如果坐标步长小于当前步长的一半，则重新自动计算步长
                if(arange(weight)>0)//防止计算出来的步长为0
                    Pots_multiple_x=arange(weight);
            offset_x=(position.x()-30)-((position.x()-30)-offset_x)/OSC_multiple_x_last*OSC_multiple_x;
        }
        else
        {
            //以10为基础开始 自动计算每次缩放的大小
            if(OSC_multiple_x*9/10!=0)//防止结果为0 因为这个变量会作为除数
                OSC_multiple_x=OSC_multiple_x*9/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int weight=(int)(this->width()/OSC_multiple_x)/Step_x;
            if(last_Pots_multiple_x*2<=arange(weight))//如果坐标步长大于当前步长的一倍，则重新自动计算步长
                Pots_multiple_x=arange(weight);
        }
        offset_x=(position.x()-30)-((position.x()-30)-offset_x)/OSC_multiple_x_last*OSC_multiple_x;
    }

    if(position.y()>this->height()-30)
    {
        double OSC_multiple_y_last=OSC_multiple_y;
        if(wheel_delta>0)
        {
            //以10为基础开始 自动计算每次缩放的大小
            OSC_multiple_y=OSC_multiple_y*11/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int weight=(int)(this->height()/OSC_multiple_y)/Step_y;
            if(last_Pots_multiple_y/2>=arange(weight))
                if(arange(weight)>0)
            Pots_multiple_y=arange(weight);
        }
        else
        {
            //以10为基础开始 自动计算每次缩放的大小
            if(OSC_multiple_y*11/10!=0)//防止结果为0 因为这个变量会作为除数
            OSC_multiple_y=OSC_multiple_y*9/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int weight=(int)(this->height()/OSC_multiple_y)/Step_y;
            if(last_Pots_multiple_y*2<=arange(weight))
            Pots_multiple_y=arange(weight);
        }
        offset_y=-(this->height()-30-position.y())+((this->height()-30-position.y())+offset_y)/OSC_multiple_y_last*OSC_multiple_y;
    }


    //对于大部分区域 同时更新offset
    if(position.x()>30 && position.y()<this->height()-30)
    {
        if(wheel_delta>0)
        {
            double OSC_multiple_x_last=OSC_multiple_x;
            //以10为基础开始 自动计算每次缩放的大小
            OSC_multiple_x=OSC_multiple_x*11/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int weight=(int)(this->width()/OSC_multiple_x)/Step_x;
            if(last_Pots_multiple_x/2>=arange(weight))//如果坐标步长小于当前步长的一半，则重新自动计算步长
                if(arange(weight)>0)//防止计算出来的步长为0
                    Pots_multiple_x=arange(weight);

            offset_x=(position.x()-30)-((position.x()-30)-offset_x)/OSC_multiple_x_last*OSC_multiple_x;

            double OSC_multiple_y_last=OSC_multiple_y;
            //以10为基础开始 自动计算每次缩放的大小
            OSC_multiple_y=OSC_multiple_y*11/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int height=(int)(this->height()/OSC_multiple_y)/Step_y;
            if(last_Pots_multiple_y/2>=arange(height))
                if(arange(height)>0)
            Pots_multiple_y=arange(height);
            offset_y=-(this->height()-30-position.y())+((this->height()-30-position.y())+offset_y)/OSC_multiple_y_last*OSC_multiple_y;
        }
        else
        {
            double OSC_multiple_x_last=OSC_multiple_x;
            //以10为基础开始 自动计算每次缩放的大小
            if(OSC_multiple_x*9/10!=0)//防止结果为0 因为这个变量会作为除数
                OSC_multiple_x=OSC_multiple_x*9/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int weight=(int)(this->width()/OSC_multiple_x)/Step_x;
            if(last_Pots_multiple_x*2<=arange(weight))//如果坐标步长大于当前步长的一倍，则重新自动计算步长
                Pots_multiple_x=arange(weight);
            offset_x=(position.x()-30)-((position.x()-30)-offset_x)/OSC_multiple_x_last*OSC_multiple_x;

            double OSC_multiple_y_last=OSC_multiple_y;
            //以10为基础开始 自动计算每次缩放的大小
            if(OSC_multiple_y*11/10!=0)//防止结果为0 因为这个变量会作为除数
            OSC_multiple_y=OSC_multiple_y*9/10;
            //根据窗口大小和缩放比例计算实际对应的的坐标范围
            int height=(int)(this->height()/OSC_multiple_y)/Step_y;
            if(last_Pots_multiple_y*2<=arange(height))
            Pots_multiple_y=arange(height);
            offset_y=-(this->height()-30-position.y())+((this->height()-30-position.y())+offset_y)/OSC_multiple_y_last*OSC_multiple_y;
        }
    }


    viewPos=false;

    //滚轮操作等不涉及到鼠标点击操作时，重新绘制图表后显示

        pixmap.fill(Qt::transparent);
        Draw_Chart();
        //下面这句话无用 使用时在滚轮滑动的时候会显示十字指针
        Draw_Cross_pointer(position.x(),position.y());

        setPixmap(pixmap);

    //emit slotmoveing(externevent);
}



void OSC_chart::Draw_Chart(/*QPainter *painter*/)
{

    //画背景
    //实线
    painter.setPen(Qt::SolidLine);
    painter.drawLine(30,this->height()-30,this->width(),this->height()-30);
    painter.drawLine(30,0,30,this->height()-30);

    //坐标
    painter.setPen(Qt::black);
    painter.setFont(QFont( "Arial" , 6 ,  QFont::Light) );

    if(OSC_multiple_x<1)
        painter.drawText(this->width()-50,15,"0."+QString::number((int)(OSC_multiple*100),10)+ "P:"+QString::number((int)(Pots_multiple),10));
    else
        painter.drawText(this->width()-50,15,"  "+QString::number((int)(OSC_multiple),10)+ "P:"+QString::number((int)(Pots_multiple),10));


    painter.drawText(this->width()/2,this->height()/2,"  "+QString::number((int)(offset_x),10)+ "Y:"+QString::number((int)(offset_y),10));


    if(1)
    {
        //虚线
        painter.setPen(Qt::DashLine);
        int Step_data_x = Pots_multiple_x;
        //初始位置是起点+偏移量的相对坐标 范围是相对范围+偏移量的相对坐标+多一次的显示（保证生成的最后一根线是从最外面开始的）
        for(int i=0;i<=Step_x*2;i++)
        {
            // 实际数据对应的x轴的坐标
            int data_width=i*Step_data_x - (int)(offset_x / OSC_multiple_x /Step_data_x) * (int)Step_data_x ;
            // 作图时x轴的坐标
            int paint_width=data_width*OSC_multiple_x + offset_x + 30;

            if(paint_width < 30)
            {
                continue;
            }
            if (paint_width > this->width())
            {
                break;
            }

            painter.drawText(paint_width,this->height()-20,QString::number(data_width,10));
            painter.drawLine(paint_width,this->height()-20,paint_width,0);
        }

    }

    if(1)
    {
        //虚线
        painter.setPen(Qt::DashLine);
        int Step_data_y = Pots_multiple_y;
        //初始位置是起点+偏移量的相对坐标 范围是相对范围+偏移量的相对坐标+多一次的显示（保证生成的最后一根线是从最外面开始的）
        for(int i=0;i<=Step_y*2;i++)
        {
            // 实际数据对应的y轴的坐标
            int data_height=i*Step_data_y +  (int)(offset_y / OSC_multiple_y /Step_data_y) * (int)Step_data_y ;
            // 作图时y轴的坐标
            int paint_height=this->height() - 30 - (data_height*OSC_multiple_y  - offset_y);
            if (paint_height > this->height() - 30)
            {
                continue;
            }
            if (paint_height < 0)
            {
                break;
            }

            painter.drawText(10,paint_height,QString::number(data_height,10));
            painter.drawLine(10,paint_height,this->width(),paint_height);
        }

    }

//    if(data_x[0].isEmpty()==false)
//    {
//        painter.setPen(Qt::SolidLine);
//        painter.setPen(Qt::black);
//        Draw_Wave(data_x[0],data_y[0]);
//        //qDebug()<<QString::number((int)&data_x[0],10);
//    }
    for(int wave_num=0;wave_num<9;wave_num++)
    {
        if(data_x[wave_num].isEmpty()==false)
        {
            painter.setPen(Qt::SolidLine);
            painter.setPen(QPen(Color_Box[wave_num],2));
            Draw_Wave(data_x[wave_num],data_y[wave_num]);
            //qDebug()<<QString::number((int)&data_x[0],10);
        }
    }
//    if(data_x[1].isEmpty()==false)
//    {
//        painter.setPen(Qt::SolidLine);
//        painter.setPen(Qt::black);
//        Draw_Wave(data_x[1],data_y[1]);
//    }
}

void OSC_chart::Draw_Wave(QList<int> chart_x,QList<int> chart_y)
{

    //画曲线
    if(true)
    {

        int x=0,y=0,last_x=30,last_y=this->height()-30;
        int last_data_index = 0;  //为高频曲线记录上一个点的索引
        if(offset_x<0) // 向右移动
        {
            if(-offset_x/OSC_multiple_x>=chart_x.length()-1)//如果起点已经超出列表范围 那么直接退出
                return;
            if((-offset_x/OSC_multiple_x)-1>=0)
            {
                last_x=chart_x[(int)(-offset_x/OSC_multiple_x)-1]*OSC_multiple_x+30;
                last_y=this->height()-chart_y[(int)(-offset_x/OSC_multiple_x)-1]*OSC_multiple_y-30;
                last_data_index = (int)(-offset_x/OSC_multiple_x)-1;
            }//else
                //qDebug()<<"errot";
        }
        else {
            last_x=chart_x[0]*OSC_multiple_x+30;
            last_y=this->height()-chart_y[0]*OSC_multiple_y-30;
            last_data_index = 0;
        }
        // 考虑最后一个点可能会超出画布，所以把计算用的画布延 长一个数据单位
        int max_canvas_width = this->width()-offset_x + Pots_multiple_x*OSC_multiple_x;

        for(int i=0-offset_x;i<max_canvas_width;i++) // 遍历图像上的像素点，找到对应的数据点作图
        {
            // i 表示移动（offset）后，被遍历图像的相对位置，绘图遍历总长度还是图像的长度
            if(i<=0)
            {
                i=0;
            }

            double double_data_index=(double)(i/OSC_multiple_x);

            if (double_data_index > __INT32_MAX__)
            {
                qDebug() << "data_index is too big " << double_data_index;
                return;
            }

            int data_index=(int)double_data_index;

            if(data_index>=chart_x.length()-1)
            {
                break;
            }

            if(true)//公式已统一 不在分情况
            {
                if(chart_x[data_index]>=chart_x[last_data_index])
                    x=chart_x[data_index]*OSC_multiple_x+30;
                else if(i==0&&chart_x[data_index]==0)//如果显示的第一个点是0
                    x=chart_x[data_index]*OSC_multiple_x+30;//那么第一个画的点也是0点 相当于原地不动画了一个点
                else
                    break;
                y=this->height()-chart_y[(int)(data_index)]*OSC_multiple_y-30;

            }

            if(last_x+offset_x>=30) //把超出横坐标显示范围的内容屏蔽掉
            {
                painter.drawLine(last_x+offset_x,last_y+offset_y,x+offset_x,y+offset_y);
            }
            else { //First line
                int x0 = last_x+offset_x;
                int y0 = last_y+offset_y;
                int x1 = x+offset_x;
                int y1 = y+offset_y;
                int x_mid = 30;
                int y_mid = y0 + (x_mid - x0)*(double)(y1 - y0)/(double)(x1 - x0);
                if( x1 > x_mid)
                    painter.drawLine(x_mid,y_mid,x1,y1);
            }

            // 对于高频波形来说，这种作图算法可能会丢失信息
            // 但该算法已经精确到像素，所以在所在尺度显示上已经无法再精确，只需将丢失的信息补回即可
            // 具体做法是：
            // 在这里需要补充画一条线，用来显示这个区间点的y值范围，线的x轴都在这个座标点上，y轴是这个区间的最大值和最小值
            if (false && (( x - last_x ) <= 1) && x > 30 ) // 出于性能考虑，该算法可以默认关闭
            {
                int y_min = chart_y[last_data_index];
                int y_max = chart_y[last_data_index];
                for (int j = last_data_index; j <= data_index; j++)
                {
                    if (chart_y[j] < y_min)
                        y_min = chart_y[j];
                    if (chart_y[j] > y_max)
                        y_max = chart_y[j];
                }
                int range_y_min=this->height()-y_min*OSC_multiple_y-30;
                int range_y_max=this->height()-y_max*OSC_multiple_y-30;
//                qDebug() << "y min" << range_y_min << "y max" << range_y_max << "X" << x;
                painter.drawLine(x+offset_x,range_y_min+offset_y,x+offset_x,range_y_max+offset_y);

            }

            last_x=x;
            last_y=y;
            last_data_index = data_index;

        }


    }
}

void OSC_chart::Set_Line_name(int Line_num,QString name)
{
    this->line_name[Line_num] = name;
}

//在表格中画线 添加线的数据 其中X正方向自加 不需要输入
void OSC_chart::Add_Line_Data(int Line_num,int pos_y)
{
    int pos_x=(this->data_x[Line_num].length());
    int last_x=pos_x*OSC_multiple_x+30;

    this->data_x[Line_num].append(pos_x);
    this->data_y[Line_num].append(pos_y);

    if((last_x+offset_x)>(this->width()-30))
    {
        if(OSC_multiple_x<0.5)//offset是int型 防止更新的偏移量小于1被当作0处理
            offset_x-=1;
        else
            offset_x-=OSC_multiple_x*2;
    }
}

//在表格中画线 添加线的数据 其中X正方向由参数输入
void OSC_chart::Add_Line_Data_XY(int Line_num,int pos_x,int pos_y)
{
    //int pos_x=(this->data_x[Line_num].length());
    int last_x=pos_x*OSC_multiple_x+30;
    if(this->data_x[Line_num].isEmpty())
    {
        if(pos_x!=0)
        {
            for(int i=1;i<pos_x;i++)
            {
                this->data_x[Line_num].append(i);
                this->data_y[Line_num].append(0);
            }
        }
        this->data_x[Line_num].append(pos_x);
        this->data_y[Line_num].append(pos_y);

    }
    else if(pos_x>this->data_x[Line_num][data_x[Line_num].length()-1])
    {
        for(int i=this->data_x[Line_num][data_x[Line_num].length()-1]+1;i<pos_x;i++)
        {
            this->data_x[Line_num].append(i);
            this->data_y[Line_num].append(pos_y);
        }
    }
    else
    {
        this->data_y[Line_num].replace(pos_x,pos_y);
    }

    if((last_x+offset_x)>(this->width()-30))
    {
        if(OSC_multiple_x<0.5)//offset是int型 防止更新的偏移量小于1被当作0处理
            offset_x-=1;
        else
            offset_x-=OSC_multiple_x*2;
    }
}
//表格自动设置（自动调整纵坐标显示范围）
double OSC_chart::Set_Chart_Auto()
{
    int Max_of_All=0,Min_of_All=0;
    bool first_set=1;
    for(int i=0;i<10;i++)
    {
        int Begin=this->data_y[i].length()-100,End=this->data_y[i].length()-1;
        if(Begin<0)
           Begin=0;
        if(End<=Begin)
            continue;
        if(first_set==1)
        {
            first_set=0;
            Max_of_All=this->data_y[i][Begin];
            Min_of_All=this->data_y[i][Begin];
        }


        for(int j=Begin+1;j<End;j++)
        {
            if(Max_of_All<this->data_y[i][j])
            {
                Max_of_All=this->data_y[i][j];
            }
            else
            {
                if(Min_of_All>this->data_y[i][j])
                {
                    Min_of_All=this->data_y[i][j];
                }
            }
        }

    }
    if(Max_of_All-Min_of_All>0)
    {
        OSC_multiple_y=(double)(this->height()-30)/(double)((Max_of_All-Min_of_All)*5/3);

        //根据窗口大小和缩放比例计算实际对应的的坐标范围
        int weight=(int)(this->height()/OSC_multiple_y)/Step_y;
        if(arange(weight)>0)
        Pots_multiple_y=arange(weight);

        offset_y=((Max_of_All+Min_of_All)/2.0)*OSC_multiple_y-this->height()/2;

        Draw_Chart();
        emit slotmoveing(externevent);
        return OSC_multiple_y;
    }
    else
        return -1;
}
//画指针线和指针坐标
void OSC_chart::Draw_Cross_pointer(/*QPainter *painter,*/int pos_x,int pos_y)
{

    if(hide_pixmap==false)
    {
        painter.drawLine(pos_x,0,pos_x,this->height());
        painter.drawLine(0,pos_y,this->width(),pos_y);
    }
    if(viewPos==true)
    {
        painter.setPen(Qt::red);
        painter.setFont(QFont( "Arial" , 11 ,  QFont::Bold) );
        QString PotsStr =QString::number((int)((pos_x-29-offset_x)/OSC_multiple_x),10)+","+QString::number((int)((this->height()-30-pos_y+offset_y)/OSC_multiple_y),10);
        if(pos_x<60&&pos_y<30)
            painter.drawText(this->width()-PotsStr.length()*10,15,PotsStr);
        else
            painter.drawText(1,15,PotsStr);

    }
    else
    {
        painter.setPen(Qt::cyan);
        painter.drawText(1,15,"");
    }
}

void OSC_chart::slotmoveing(QMouseEvent *event)
{
//    painter.end();
//    pixmap=QPixmap(this->width(),this->height());//画布
//    pixmap.fill(Qt::transparent);
//    painter.begin(&pixmap);//绘图
#if (QT_VERSION <= QT_VERSION_CHECK(6,0,0))
    QPointF position = event->pos();
#else
    QPointF position = event->position();
#endif

    //画pixmap前清除原来的内容 pixmap和painter为私有变量
    pixmap.fill(Qt::transparent);


    externevent=event;
    Draw_Chart();

    painter.setPen(Qt::blue);//设置指针颜色
    Draw_Cross_pointer(position.x(),position.y());

    setPixmap(pixmap);


}
void OSC_chart::slotdoubleClicked(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        this->setMouseTracking(false);
    }
    if(event->button() == Qt::RightButton)
    {
        sethide(true);
        viewPos=false;
    }

}

void OSC_chart::slotClicked(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        this->setMouseTracking(false);
        if(hide_pixmap==true)
        {
            sethide(false);
            viewPos=true;
        }
    }
    if(event->button() == Qt::LeftButton)
    {
        this->setMouseTracking(true);
        sethide(false);
        viewPos=true;
    }

}
void OSC_chart::mousePressEvent(QMouseEvent *event)
{
#if (QT_VERSION <= QT_VERSION_CHECK(6,0,0))
    QPointF position = event->pos();
#else
    QPointF position = event->position();
#endif
    if(event->button() == Qt::LeftButton)
    {
        start_x=position.x()-offset_x;
        start_y=position.y()-offset_y;
        start_flag=1;
    }
    emit clicked(event);
    emit slotmoveing(event);
}

void OSC_chart::mouseMoveEvent(QMouseEvent *event)
{
#if (QT_VERSION <= QT_VERSION_CHECK(6,0,0))
    QPointF position = event->pos();
#else
    QPointF position = event->position();
#endif
    if(start_flag==1)
    {
        //防止超过最大处理数
        if(abs((position.y() - start_y)/OSC_multiple_y)<MAX_INT_VALUE)
            offset_y=position.y()-start_y;

        if(abs((position.x()-start_x)/OSC_multiple_x)<MAX_INT_VALUE)
            offset_x=position.x()-start_x;
    }
    emit slotmoveing(event);
}

void OSC_chart::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit doubleClicked(event);
    emit slotmoveing(event);

}

void OSC_chart::mouseReleaseEvent(QMouseEvent  *event)
{
    start_flag=0;
}
