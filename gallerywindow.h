#ifndef GALLERYWINDOW_H
#define GALLERYWINDOW_H
/*
 * class:gallerywindow
 * goal:实现相册浏览、选择图像添加到背景、根据字符串得到的前景，得到合成前景背景之后的图像，保存合成后的图像
 * author:钟琴
 * time:2017.09.17 11:24:55
*/
#include <QMainWindow>
class QListWidget;
class QListWidgetItem;
class QPaintEvent;

class GalleryWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GalleryWindow(QWidget *parent = nullptr);
    ~GalleryWindow();
protected:
    void paintEvent ();
private:
    void initSystem();
    void setBgImage(const QString &strPath);
private slots:
    void slotitemClicked(QListWidgetItem * item);
private:
    QListWidget *m_pListWidget;
    QString m_strPath;
};

#endif // GALLERYWINDOW_H
