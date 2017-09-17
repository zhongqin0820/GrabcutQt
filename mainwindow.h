#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <memory>
#include "GrabCut.h"
#include "Image.h"
#include "Color.h"
#include <QtWidgets>

class QMenu;
class QLabel;
class QAction;
class QActionGroup;
class QDoubleSpinBox;
class QProgressBar;
class QLayout;


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

protected:
	 bool eventFilter(QObject *obj, QEvent *e);
	 QSize sizeHint() const;

private:
    //创建窗口
    void setupUi();
    //创建UI
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
    //  初始化
	void init();
    //	初始化
    void initSystem();
	void initParameters();

    //加载导入的图片
	void openImage(const QString& fileName);
	void saveAsImageFile(const QString& fileName);
	QString strippedName(const QString& fullFileName);
	void updateInformationBar();

	void updateImages();

private slots:
    //打开图片文件夹
	void open();
    //保存图片至文件夹
	void saveAs();
    //打开新的workshop window
    void newWin();
    //自动抠图操作refine
    void triggerrefineOnce();
    //打开相册
    void openGallery();

	void triggerEditAct(QAction* act);
	void triggerViewAct(QAction* act);
	void changeViewModeAct(QAction* act);
	void about();

private:
	enum ViewMode
	{
		VM_IMAGE = 0,
		VM_GMM_MASK,
		VM_NLINK_MASK,
		VM_TLINK_MASK,
		VM_MAX
	};

	enum SelectionMode
	{
		SM_NONE = 0,
		SM_RECT,
		SM_PAINT_BG,
		SM_PAINT_FG
	};

private:
    //视图UI
	QWidget *mImgView;
    //图片序列参照VireMode
	QImage mImages[VM_MAX];
    //图片掩膜
	QImage mImgMask;

	// file menu
	QMenu *mFileMenu;
	QAction *mOpenAct;
	QAction *mQuitAct;
	QAction *mSaveAsImageAct;
    // fileName;
    QString mfileName;
    //高级添加
    QAction *mNewAct;
    QAction *mMagicAct;
    QAction *mMagicActplus;
    QAction *mGalleyAct;
	// view menu
	QMenu *mViewMenu;
	QAction *mViewToolBarAct;
	QAction *mInfoToolBarAct;
	QActionGroup *mViewActGroup;

	// edit menu
	QMenu *mEditMenu;
	QAction *mViewModeActs[VM_MAX];
	QActionGroup *mViewModeGroup;
	QAction *mRefineAct;
	QAction *mRefineOnceAct;
	QAction *mFitGMMsAct;
	QAction *mAbortRefiningAct;
	QAction *mShowMaskAct;
	QActionGroup *mEditActGroup;

	// help menu
	QMenu *mHelpMenu;
	QAction *mAboutAct;
	QAction *mAboutQtAct;

	QToolBar *mFileToolBar;
	QToolBar *mEditToolBar;
	QToolBar *mCameraLightToolBar;

	// info toolbar
	QToolBar *mInfoToolBar;
	QLabel *mInfoLabel;
	long mLastCostTime;

	// status bar
	QLabel *mResLabel;
	QProgressBar *mProgressBar;

	// mouse operations
	QPoint mLastPos;
	QPoint mStartPos;
	QPoint mEndPos;

	// view mode
	ViewMode mViewMode;

	// grabcut
	std::auto_ptr<GrabCutNS::Image<GrabCutNS::Color> > mImageArr;
	std::auto_ptr<GrabCutNS::GrabCut> mGrabCut;
	QVector<QPoint> mPaintingPoses;
	SelectionMode mSelectionMode;
	bool mRefining;
	bool mInitialized;
    //显示掩膜
	bool mShowMask;
};

#endif // MAINWINDOW_H
