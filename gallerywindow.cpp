#include "gallerywindow.h"
#include <QListWidget>
#include <QDebug>
#include <iostream>
#include <cstdio>
using namespace std;
const int W_ICONSIZE = 96; //图片宽度
const int H_ICONSIZE = 96; //图片高度
const int H_ITEMSIZE = 110; //单元项高度(因为单元项既包括图片又包括文本)

//构造函数
GalleryWindow::GalleryWindow(QWidget *parent) : QMainWindow(parent)
{
    initSystem();
}

//初始化gallery系统
void GalleryWindow::initSystem(){
    //创建QListWidget部件
    m_pListWidget = new QListWidget(this);
    //设置QListWidget中的单元项的图片大小
    m_pListWidget->setIconSize(QSize(W_ICONSIZE, H_ICONSIZE));
    m_pListWidget->setResizeMode(QListView::Adjust);
    //设置QListWidget的显示模式
    m_pListWidget->setViewMode(QListView::IconMode);
    //设置QListWidget中的单元项不可被拖动
    m_pListWidget->setMovement(QListView::Static);
    //设置QListWidget中的单元项的间距
    m_pListWidget->setSpacing(10);
    //依次创建7个单元项
    for(int nIndex = 0;nIndex<7;++nIndex)
    {
        //获得图片路径
        QString strPath=QString(":/gallery/%1.jpeg").arg(nIndex+1);
        char imgName[40]={'\0'};
        sprintf(imgName,"%d",nIndex+1);
        cout<<imgName<<endl;
        qDebug("%s\n",strPath.toStdString().data());
        //生成图像objPixmap
        QPixmap objPixmap(strPath);
        //生成QListWidgetItem对象(注意：其Icon图像进行了伸缩[96*96])---scaled函数
        QListWidgetItem *pItem = new QListWidgetItem(QIcon(objPixmap.scaled(QSize(W_ICONSIZE,H_ICONSIZE))),imgName);
        //设置单元项的宽度和高度
        pItem->setSizeHint(QSize(W_ICONSIZE,H_ITEMSIZE));
        m_pListWidget->insertItem(nIndex, pItem);

    }
     setCentralWidget(m_pListWidget);
     //设置信号槽
     connect(m_pListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(slotitemClicked(QListWidgetItem*)));

     m_strPath = "";

     setWindowTitle("FBgSynthesising");
}

//点击响应
void GalleryWindow::slotitemClicked(QListWidgetItem * item)
{
    cout<<"slotitemClicked"<<endl;
    if(!item)
    {
        return;
    }
    int nRowIndex = m_pListWidget->row(item);

    m_strPath=QString(":/gallery/%1.jpeg").arg(nRowIndex+1);
    qDebug("slotitemClicked strPath: %s\n",m_strPath.toStdString().data());
    paintEvent();
    this->update();
}

//调用setBgImage函数
void GalleryWindow::paintEvent ()
{
//    cout<<"paintEvent"<<endl;
    qDebug("paintEvent() strPath:%s\n",m_strPath.toStdString().data());
    setBgImage(m_strPath);

}

//设置主窗口背景
void GalleryWindow::setBgImage(const QString &strPath)
{
    cout<<"SetBgImage:"<<strPath.toStdString().data()<<endl;
    QPixmap objPixmap(strPath);
    QPalette palette = this->palette();
    if(strPath.isEmpty())
    {
        palette.setBrush(QPalette::Base, QBrush(QColor(0,0,255)));
    }
    else
    {
        palette.setBrush(QPalette::Base, QBrush(objPixmap.scaled(width(),height())));
    }
    setPalette(palette);
    this->update();
}

//析构函数
GalleryWindow::~GalleryWindow()
{

}
