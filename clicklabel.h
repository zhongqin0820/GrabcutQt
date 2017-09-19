#ifndef CLICKLABEL_H
#define CLICKLABEL_H
#include <QLabel>

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickLabel(QWidget *parent = 0);
    ~ClickLabel();
signals:
    // 鼠标单击信号
//    void myclicked();
protected:
    // 鼠标单击事件
    void mousePressEvent(QMouseEvent *e);       //--鼠标按下事件
    void mouseMoveEvent(QMouseEvent *e);    //--鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *e); //--鼠标双击事件
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    int m_x;
    int m_y;
public slots:
};

#endif // CLICKLABEL_H
