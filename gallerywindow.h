#ifndef GALLERYWINDOW_H
#define GALLERYWINDOW_H
/*
 * class:gallerywindow
 * goal:实现相册浏览、选择图像添加到背景、根据字符串得到的前景，得到合成前景背景之后的图像，保存合成后的图像
 * author:钟琴
 * time:2017.09.17 11:24:55
*/
#include <QMainWindow>
#include <QLabel>
#include "clicklabel.h"
#include <QtGui>

class QListWidget;
class QScrollArea;
class QListWidgetItem;
class QPaintEvent;
class QLable;
class ClickLabel;
class GalleryWindow : public QMainWindow
{
    Q_OBJECT
public:
    //    传入抠好后的图片的文件名进行读取
    explicit GalleryWindow(QString alphaImg, QWidget *parent = nullptr);
    //    析构函数
    ~GalleryWindow();
protected:
    //    绘图必须要使用的函数
    void paintEvent (QPaintEvent *);
private:
    //    初始化系统变量
    void initSystem();
    //    绘制背景图片
    void setBgImage(const QString &strPath);
private slots:
    //    点击图片响应函数
    void slotitemClicked(QListWidgetItem * item);
private:
    QListWidget *m_pListWidget;//    模拟相册的list
    QPixmap m_maskPixmapl;
    QPixmap m_bgPixmap;
    QLabel *m_drawLabel;
    ClickLabel m_maskClickLabel;
    QString m_strPath;//    背景图像文件路径
    QString m_alphaPath;//    前景图像文件路径
    bool m_flag;
};

#endif // GALLERYWINDOW_H
