#include "gallerywindow.h"
//#include "qselectwidget.h"
#include "clicklabel.h"
#include <QListWidget>
#include <QString>
#include <QPainter>
#include <QDebug>
#include <QSplitter>
#include <QRect>
#include <QImage>
#include <QLabel>
#include <QtGui>
#include <iostream>
#include <cstdio>
using namespace std;
const int W_ICONSIZE = 96; //图片宽度
const int H_ICONSIZE = 96; //图片高度
const int H_ITEMSIZE = 110; //单元项高度(因为单元项既包括图片又包括文本)

//构造函数，初始化系统参数，并且将前景图片初始化到QPushbutton
GalleryWindow::GalleryWindow(QString alphaImg, QWidget *parent) : QMainWindow(parent)
{
//    cout<<"GalleryWindow"<<endl;
    m_alphaPath = alphaImg;
    m_maskPixmapl.load(m_alphaPath);
    m_maskClickLabel.setPixmap(m_maskPixmapl);
    initSystem();
}

//初始化gallery系统：相册列表
void GalleryWindow::initSystem(){
//    cout<<"initSystem"<<endl;
    m_flag = false;
    m_pListWidget = new QListWidget(this);
    m_pListWidget->setIconSize(QSize(W_ICONSIZE, H_ICONSIZE));
    m_pListWidget->setAutoFillBackground(false);
    m_pListWidget->setResizeMode(QListView::Adjust);
    m_pListWidget->setViewMode(QListView::IconMode);
    m_pListWidget->setMovement(QListView::Static);
    m_pListWidget->setWrapping(false);
    m_pListWidget->setFlow(QListView::LeftToRight);
    m_pListWidget->setSpacing(10);
    m_pListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_pListWidget->setFixedHeight(H_ICONSIZE+36);
    //依次创建7个单元项
    for(int nIndex = 0;nIndex<12;++nIndex)
    {
        //获得图片路径
        QString strPath=QString(":/gallery/%1.jpeg").arg(nIndex+1);
        char imgName[40]={'\0'};
        sprintf(imgName,"%d",nIndex+1);
//        cout<<imgName<<endl;
//        qDebug("%s\n",strPath.toStdString().data());
        //生成图像objPixmap
        QPixmap objPixmap(strPath);
        //生成QListWidgetItem对象(注意：其Icon图像进行了伸缩[96*96])---scaled函数
        QListWidgetItem *pItem = new QListWidgetItem(QIcon(objPixmap.scaled(QSize(W_ICONSIZE,H_ICONSIZE))),imgName);
        //设置单元项的宽度和高度
        pItem->setSizeHint(QSize(W_ICONSIZE,H_ITEMSIZE));
        m_pListWidget->insertItem(nIndex, pItem);

    }
    setBgImage(m_strPath);
    m_drawLabel = new QLabel(this);
    m_drawLabel->setScaledContents(true);

    QSplitter *pRightSpliter = new QSplitter(Qt::Vertical);
    pRightSpliter->addWidget(m_pListWidget);
    pRightSpliter->addWidget(m_drawLabel);
    this->setCentralWidget(pRightSpliter);

     //设置信号槽
     connect(m_pListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(slotitemClicked(QListWidgetItem*)));
     m_strPath = "";
     this->setWindowTitle("FBgSynthesising");
}

//点击响应
void GalleryWindow::slotitemClicked(QListWidgetItem * item)
{
//    cout<<"slotitemClicked"<<endl;
    if(!item)
    {
        return;
    }
    int nRowIndex = m_pListWidget->row(item);
    m_flag = true;
    m_strPath=QString(":/gallery/%1.jpeg").arg(nRowIndex+1);
//    qDebug("slotitemClicked strPath: %s\n",m_strPath.toStdString().data());
    paintEvent(nullptr);
}

//绘制图像函数
void GalleryWindow::paintEvent (QPaintEvent *e)
{
//    cout<<"paintEvent"<<endl;
    if(m_strPath.isEmpty())
    {
        m_maskClickLabel.setParent(m_drawLabel);
        m_maskClickLabel.show();
        return;
    }
    if(m_flag)
    {
        m_flag = false;
        m_bgPixmap.load(m_strPath);
        m_drawLabel->setPixmap(m_bgPixmap);
        m_bgPixmap.scaled(m_drawLabel->x(),m_drawLabel->y());
        m_drawLabel->resize(m_bgPixmap.width(),m_bgPixmap.height());
        m_drawLabel->show();
    }
}

//设置相册列表窗口背景为白色
void GalleryWindow::setBgImage(const QString &strPath)
{
//    cout<<"setBgImage"<<endl;
    if(strPath.isEmpty())
    {
        QPalette palette = m_pListWidget->palette();
        palette.setBrush(QPalette::Base, QBrush(QColor(255,255,255)));
        m_pListWidget->setPalette(palette);
    }
    this->update();
}

//析构函数
GalleryWindow::~GalleryWindow()
{
    delete m_pListWidget;
    delete m_drawLabel;
    if(m_pListWidget != nullptr)
    {
        m_pListWidget = nullptr;
    }
    if(m_drawLabel != nullptr)
    {
        m_drawLabel = nullptr;
    }

}
