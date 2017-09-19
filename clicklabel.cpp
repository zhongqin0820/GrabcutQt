#include "clicklabel.h"
#include <QMouseEvent>
#include <iostream>

using namespace std;

ClickLabel::ClickLabel(QWidget *parent) :
    QLabel(parent)
{
    this->m_x = this->pos().x();
    this->m_y = this->pos().y();
    cout<<"click: "<<m_x<<","<<m_y<<endl;
}



// mark: protected:------------------------------------------------
void ClickLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    cout<<"before: "<<this->pos().x()<<","<<this->pos().y()<<endl;
    cout<<"mouse: "<<ev->x()<<","<<ev->y()<<endl;
    int m__x = this->pos().x() + ev->x() - this->m_x;
    int m__y = this->pos().y() + ev->y() - this->m_y;
    cout<<"after: "<<m__x<<","<< m__y<<endl;
    cout<<endl;
    if (ev->button() == Qt::LeftButton)
    {
        this->setGeometry(m__x,m__y,this->width(),this->height());
    }
}

void ClickLabel::mousePressEvent(QMouseEvent *e)
{
    cout<<"press: "<<e->x()<<","<<e->y()<<endl;
    this->m_x = e->x();
    this->m_y = e->y();
}

void ClickLabel::mouseMoveEvent(QMouseEvent *ev)
{
//    cout<<"move : "<<ev->x()<<","<<ev->y()<<endl;
//    this->setGeometry(ev->x()-this->m_x,ev->y()-this->m_y,this->width(),this->height());
//    if (ev->button() == Qt::LeftButton)
//    {
//        this->setGeometry(ev->x()-this->m_x,ev->y()-this->m_y,this->width(),this->height());
//    }
}

//双击保存合成的图片
void ClickLabel::mouseDoubleClickEvent(QMouseEvent *e)
{

}

void ClickLabel::enterEvent(QEvent * e)
{

}

void ClickLabel::leaveEvent(QEvent *)
{

}

ClickLabel::~ClickLabel()
{

}
