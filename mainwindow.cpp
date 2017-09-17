#include "mainwindow.h"
#include "gallerywindow.h"
#include <QWidget>
#include <QtGui>
#include <ctime>
#include <string>
#include <iostream>
using namespace std;

const QString WIDGET_NAME = "ImageView";
const QString WINDOW_TITLE = "Matting System";
const int TOOLTIP_STRETCH = 5000;
const int VIEWER_WIDTH = 800;
const int VIEWER_HEIGHT = 600;

//将QImage转化为Image以供后面使用，用R,G,B形成的Color初始化Image中的每个元素
GrabCutNS::Image<GrabCutNS::Color>* create_image_array_from_QImage(const QImage &img)
{
	int w = img.width();
	int h = img.height();
	QRgb clr;
	GrabCutNS::Real R, G, B;
	GrabCutNS::Image<GrabCutNS::Color> *imgArr;
	imgArr = new GrabCutNS::Image<GrabCutNS::Color>(w, h);
	for (int y=0; y<h; ++y)
	{
		for (int x=0; x<w; ++x)
		{
			clr = img.pixel(x, y);
			R = static_cast<GrabCutNS::Real>(qRed(clr) / 255.0f);
			G = static_cast<GrabCutNS::Real>(qGreen(clr) / 255.0f);
			B = static_cast<GrabCutNS::Real>(qBlue(clr) / 255.0f);
			(*imgArr)(x,y) = GrabCutNS::Color(R, G, B);
		}
	}
	return imgArr;
}

void copy_image_array_to_QImage(const GrabCutNS::Image<GrabCutNS::Color>* imgArr, QImage &qImg)
{
	int w = imgArr->width();
	int h = imgArr->height();
	Q_ASSERT(qImg.width() == w && qImg.height() == h);

	int r, g, b;
	for (int y=0; y<h; ++y)
	{
		for (int x=0; x<w; ++x)
		{
			r = static_cast<int>((*imgArr)(x,y).r * 255.0f);
			g = static_cast<int>((*imgArr)(x,y).g * 255.0f);
			b = static_cast<int>((*imgArr)(x,y).b * 255.0f);
			qImg.setPixel(x,y, qRgb(r,g,b));
		}
	}
}

void copy_image_array_to_QImage(const GrabCutNS::Image<GrabCutNS::Real>* imgArr, QImage &qImg)
{
	int w = imgArr->width();
	int h = imgArr->height();
	Q_ASSERT(qImg.width() == w && qImg.height() == h);

	int luminance;
	for (int y=0; y<h; ++y)
	{
		for (int x=0; x<w; ++x)
		{
			luminance = static_cast<int>((*imgArr)(x,y) * 255.0f);
			qImg.setPixel(x,y, qRgb(luminance,luminance,luminance));
		}
	}
}
//用Rect去init图像的时候使用
void copy_alpha_array_to_QImage(const GrabCutNS::Image<GrabCutNS::Real>* alphaArr, QImage &qImg, bool mask =  true);
//最后保存图像的时候使用！
void copy_alpha_array_to_QImage(const GrabCutNS::Image<GrabCutNS::Real>* alphaArr, QImage &qImg, bool mask)
{
	int w = alphaArr->width();
	int h = alphaArr->height();
	Q_ASSERT(qImg.width() == w && qImg.height() == h);

	QRgb clr;
	int alpha;
	for (int y=0; y<h; ++y)
	{
		for (int x=0; x<w; ++x)
		{
			clr = qImg.pixel(x, y);
			alpha = static_cast<int>((*alphaArr)(x,y) * 255.0f);
			if (mask)
			{
				qImg.setPixel(x,y, qRgba(0,0,0, alpha));
			}
			else
			{
				if (alpha > 128)
					qImg.setPixel(x,y, qRgba(0,0,0,0));
			}
		}
	}
}


MainWindow::MainWindow()
{
	init();
}

MainWindow::~MainWindow()
{
}

//真正的初始化函数，由MainWindow调用！
void MainWindow::init()
{
	srand((unsigned int)time(0));

	setupUi();
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();	

	initParameters();
}

//初始化系统,打开文件后重新初始化
void MainWindow::initSystem()
{
	initParameters();

    //让其它几个图片的尺寸与初识图片一致，方便后续操作
	for (int i=1; i<VM_MAX; ++i)
	{
		mImages[i] = mImages[VM_IMAGE];
	}
    //初始化掩膜和图片一致
	mImgMask = QImage(mImages[VM_IMAGE].size(), QImage::Format_ARGB32);
	mImgMask.fill(0);
}

//初始化一些参量
void MainWindow::initParameters()
{
	mViewMode = VM_IMAGE;
	mSelectionMode = SM_NONE;
	mRefining = false;
	mInitialized = false;
    mShowMask = false;//

    if (mImages->isNull()){
		mImgView->setFixedSize(VIEWER_WIDTH, VIEWER_HEIGHT);
    }else{
//        mImgView->setFixedSize(VIEWER_WIDTH, VIEWER_HEIGHT);
        mImgView->setFixedSize(mImages[VM_IMAGE].size());
//        mImages[VM_IMAGE] = new_Image;
    }
}

//初始化系统的UI
void MainWindow::setupUi()
{
	mImgView = new QWidget;
	mImgView->setObjectName(WIDGET_NAME);
	this->setCentralWidget(mImgView);
	mImgView->installEventFilter(this);
//    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->layout()->setSizeConstraint(QLayout::SetMaximumSize);
	mProgressBar = new QProgressBar(this);

	setWindowTitle(WINDOW_TITLE);
}

//返回图片的尺寸
QSize MainWindow::sizeHint() const
{
	return mImages[VM_IMAGE].size();
}

//添加UI
void MainWindow::createActions()
{
	// file menu
	mOpenAct = new QAction(QIcon(":/images/open.png"), tr("&Open Obj..."), this);
	mOpenAct->setShortcut(QKeySequence::Open);
	mOpenAct->setToolTip(tr("Open an image file"));
	connect(mOpenAct, SIGNAL(triggered()), this, SLOT(open()));

	mSaveAsImageAct = new QAction(QIcon(":/images/save.png"), tr("&Save As Image..."), this);
	mSaveAsImageAct->setShortcut(QKeySequence::Save);
	mSaveAsImageAct->setToolTip(tr("Save the segmented image"));
	connect(mSaveAsImageAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	mQuitAct = new QAction(tr("&Quit"), this);
    mQuitAct->setShortcut(QKeySequence::Quit);
	mQuitAct->setToolTip(tr("Exit the application"));
	connect(mQuitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    // plus edit menu
    mNewAct = new QAction(QIcon(":/images/new.png"), tr("&New Obj..."), this);
//    mNewAct->setShortcut(QKeySequence::Open);
    mNewAct->setToolTip(tr("New an image file"));
    connect(mNewAct, SIGNAL(triggered()), this, SLOT(newWin()));

    mMagicAct = new QAction(QIcon(":/images/cut.png"), tr("&Magic Refine..."), this);
    mMagicAct->setShortcut(tr("Ctrl+R"));
    mMagicAct->setToolTip(tr("Magic an image file"));
    connect(mMagicAct, SIGNAL(triggered()), this, SLOT(triggerrefineOnce()));

    mMagicActplus = new QAction(QIcon(":/images/magic.png"), tr("&Magic Refine plus..."), this);
    mMagicActplus->setShortcut(tr("Ctrl+R"));
    mMagicActplus->setToolTip(tr("Magic an image file plus"));
    connect(mMagicActplus, SIGNAL(triggered()), this, SLOT(triggerrefineOnce()));

    mGalleyAct = new QAction(QIcon(":/images/paste.png"), tr("&New Window Gallery..."), this);
    mGalleyAct->setShortcut(tr("Ctrl+N"));
    mGalleyAct->setToolTip(tr("Create Gallery Window"));
    connect(mGalleyAct, SIGNAL(triggered()), this, SLOT(openGallery()));

    // edit menu
	mViewModeGroup = new QActionGroup(this);
	mViewModeGroup->setExclusive(true);
	for (int i=0; i<VM_MAX; ++i)
	{
		QString name = "";
		switch (i)
		{
		case VM_IMAGE:
            name += "Image";
			break;
		case VM_GMM_MASK:
			name += "GMM mask";
			break;
		case VM_NLINK_MASK:
			name += "NLink mask";
			break;
		case VM_TLINK_MASK:
			name += "TLink mask";
			break;
		}
        mViewModeActs[i] = new QAction(tr("%1").arg(name), this);
		mViewModeActs[i]->setCheckable(true);
		mViewModeActs[i]->setToolTip(tr("Toggle view of %1").arg(name));
		mViewModeGroup->addAction(mViewModeActs[i]);
	}
    mViewModeActs[VM_IMAGE]->setChecked(true);//默认是进入Image菜单栏(未编辑时)
    for(int i=0;i<VM_MAX;++i){
        mViewModeActs[i]->setVisible(false);
    }
	connect(mViewModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeViewModeAct(QAction*)));

    //其他操作，不同的编辑操作,快捷键形式
	mRefineAct = new QAction(tr("Refine"), this);
	mRefineAct->setToolTip(tr("Run GrabCut refinement"));
    //运行细化一次
    mRefineOnceAct = new QAction(tr("Refine Once"), this);
	mRefineOnceAct->setToolTip(tr("Run one step of GrabCut refinement"));
	mRefineOnceAct->setShortcut(tr("Ctrl+R"));

	mFitGMMsAct = new QAction(tr("Fit GMMs"), this);
	mFitGMMsAct->setToolTip(tr("Run the Orchard-Bowman GMM clustering"));
    //取消细化
	mAbortRefiningAct = new QAction(tr("Abort"), this);
	mAbortRefiningAct->setToolTip(tr("Stop refining that is running"));
	mAbortRefiningAct->setShortcut(tr("Ctrl+C"));
    //是否显示掩膜
	mShowMaskAct = new QAction(tr("Show Mask"), this);
	mShowMaskAct->setToolTip(tr("Show alpha mask of the segmentation"));
	mShowMaskAct->setCheckable(true);
	mShowMaskAct->setChecked(mShowMask);
	mShowMaskAct->setShortcut(tr("Ctrl+M"));
    //每一次画笔修改,都会更新图片
	mEditActGroup = new QActionGroup(this);
	mEditActGroup->setExclusive(false);
	mEditActGroup->addAction(mRefineAct);
	mEditActGroup->addAction(mRefineOnceAct);
	mEditActGroup->addAction(mFitGMMsAct);
	mEditActGroup->addAction(mAbortRefiningAct);
	mEditActGroup->addAction(mShowMaskAct);
    connect(mEditActGroup, SIGNAL(triggered(QAction*)), this, SLOT(triggerEditAct(QAction*)));

	// view menu
	mViewToolBarAct = new QAction(tr("&Toolbar"), this);
	mViewToolBarAct->setToolTip(tr("Toggle toolbar"));
	mViewToolBarAct->setCheckable(true);
    mViewToolBarAct->setChecked(true);
	mViewToolBarAct->setShortcut(tr("Ctrl+T"));

	mInfoToolBarAct = new QAction(tr("&Information"), this);
	mInfoToolBarAct->setToolTip(tr("Toggle information toolbar"));
	mInfoToolBarAct->setCheckable(true);
    mInfoToolBarAct->setChecked(true);
	mInfoToolBarAct->setShortcut(tr("Ctrl+I"));

	mViewActGroup = new QActionGroup(this);
	mViewActGroup->addAction(mViewToolBarAct);
	mViewActGroup->addAction(mInfoToolBarAct);
	mViewActGroup->setExclusive(false);
	connect(mViewActGroup, SIGNAL(triggered(QAction*)), this, SLOT(triggerViewAct(QAction*)));

	// help menu
	mAboutAct = new QAction(tr("&About"), this);
	mAboutAct->setToolTip(tr("Show the application's About box"));
	connect(mAboutAct, SIGNAL(triggered()), this, SLOT(about()));
	mAboutQtAct = new QAction(tr("About &Qt"), this);
	mAboutQtAct->setToolTip(tr("Show the Qt library's About box"));
	connect(mAboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

//编辑信号槽，根据用户输入动作执行对应操作
void MainWindow::triggerEditAct(QAction* act)
{
	if (mImages[VM_IMAGE].isNull())
		return;

	if (act == mRefineOnceAct)
	{
		mGrabCut->refineOnce();
	}
	else if (act == mRefineAct)
	{
		mRefining = true;
	}
	else if (act == mFitGMMsAct)
	{
		mGrabCut->fitGMMs();
	}
	else if (act == mRefineAct)
	{
		mRefining = false;
	}
	else if (act == mShowMaskAct)
	{
		mShowMask = mShowMaskAct->isChecked();
	}
	updateImages();
	mImgView->update();
	updateInformationBar();
}

//工具栏、信息栏等
void MainWindow::triggerViewAct(QAction *act)
{
	bool bShow = act->isChecked();
	if (act == mViewToolBarAct)
	{
		mFileToolBar->setVisible(bShow);
		mEditToolBar->setVisible(bShow);
		mCameraLightToolBar->setVisible(bShow);
	}
	else if (act == mInfoToolBarAct)
	{
		mInfoToolBar->setVisible(bShow);
	}
}

//更新操作后的各视图
void MainWindow::changeViewModeAct(QAction* act)
{
	for (int i=0; i<VM_MAX; ++i)
	{
		if (act == mViewModeActs[i])
		{
			mViewMode = static_cast<ViewMode>(i);
			mImgView->update();
			break;
		}
	}
}

//创建菜单
void MainWindow::createMenus()
{
	mFileMenu = menuBar()->addMenu(tr("&File"));
	mFileMenu->addAction(mOpenAct);
	mFileMenu->addAction(mSaveAsImageAct);
    mFileMenu->addAction(mNewAct);
//    mFileMenu->addAction(mMagicAct);
//    mFileMenu->addAction(mMagicActplus);
	mFileMenu->addSeparator();
	mFileMenu->addAction(mQuitAct);
    mFileMenu->addAction(mMagicAct);
    mFileMenu->addAction(mMagicActplus);
    mFileMenu->addAction(mGalleyAct);
	menuBar()->addSeparator();

	mViewMenu = menuBar()->addMenu(tr("&View"));
	mViewMenu->addAction(mViewToolBarAct);
	mViewMenu->addAction(mInfoToolBarAct);

    menuBar()->addSeparator();//分隔栏

	mEditMenu = menuBar()->addMenu(tr("&Edit"));
	for (int i=0; i<VM_MAX; ++i)
	{
        mEditMenu->addAction(mViewModeActs[i]);//
	}
	mEditMenu->addSeparator();
	mEditMenu->addAction(mRefineAct);
	mEditMenu->addAction(mRefineOnceAct);
	mEditMenu->addAction(mFitGMMsAct);
	mEditMenu->addAction(mAbortRefiningAct);
	mEditMenu->addSeparator();
	mEditMenu->addAction(mShowMaskAct);

	menuBar()->addSeparator();
	mHelpMenu = menuBar()->addMenu(tr("&Help"));
	mHelpMenu->addAction(mAboutAct);
	mHelpMenu->addAction(mAboutQtAct);
}

//创建工具拦：文件、编辑
void MainWindow::createToolBars()
{
	// file toolbar
	mFileToolBar = addToolBar(tr("File"));
	mFileToolBar->addAction(mOpenAct);
	mFileToolBar->addAction(mSaveAsImageAct);
    mFileToolBar->addAction(mNewAct);
	// edit toolbar
	mEditToolBar = addToolBar(tr("Edit"));
    mEditToolBar->addAction(mMagicAct);
    mEditToolBar->addAction(mMagicActplus);
    mEditToolBar->addAction(mGalleyAct);
	for (int i=0; i<VM_MAX; ++i)
	{
		mEditToolBar->addAction(mViewModeActs[i]);
	}

	// information toolbar
	mInfoToolBar = addToolBar(tr("Information"));
    mInfoToolBar->setVisible(false);
	addToolBar(Qt::RightToolBarArea, mInfoToolBar);

	QGroupBox *infoGroup = new QGroupBox(tr("Information"));
	//QProgressBar *progressBarCopy = new QProgressBar(mProgressBar);
	//connect(mProgressBar, SIGNAL(valueChanged(int)), progressBarCopy, SLOT(setValue(int)));
	QBoxLayout *infoLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	mInfoLabel = new QLabel("");
	infoLayout->addWidget(mInfoLabel);
	//infoLayout->addWidget(progressBarCopy);
	infoGroup->setLayout(infoLayout);
	mInfoToolBar->addWidget(infoGroup);

	updateInformationBar();
}

//加载进度条
void MainWindow::createStatusBar()
{
	statusBar()->addWidget(mProgressBar);
	mProgressBar->hide();
	
	statusBar()->showMessage(tr("Ready"));
	statusBar()->setSizeGripEnabled(false);
}

//每次操作之后都要更新mImages里的内容
void MainWindow::updateImages()
{
	if (mShowMask)
		copy_alpha_array_to_QImage(mGrabCut->getAlphaImage(), mImgMask);

	copy_image_array_to_QImage(mGrabCut->getNLinksImage(), mImages[VM_NLINK_MASK]);
	copy_image_array_to_QImage(mGrabCut->getTLinksImage(), mImages[VM_TLINK_MASK]);
	copy_image_array_to_QImage(mGrabCut->getGMMsImage(), mImages[VM_GMM_MASK]);
}

//打开文件
void MainWindow::open()
{
    string folerName = "/Users/Zoking/Documents/WorkSpace/QTworkspace/project1/autoMattingv1_0/test_set";
	QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Obj Model"), folerName.c_str(), tr("Image Files (*.bmp *.png *.jpg *.jpeg *.tiff)"));
    mfileName = fileName.mid(folerName.length(),fileName.indexOf(".")-folerName.length());
    if (!fileName.isEmpty())
	{
		openImage(fileName);
		mImageArr.reset();
		mGrabCut.reset();
		mImageArr = std::auto_ptr<GrabCutNS::Image<GrabCutNS::Color> >(create_image_array_from_QImage(mImages[VM_IMAGE]));
        mGrabCut = std::auto_ptr<GrabCutNS::GrabCut>(new GrabCutNS::GrabCut(mImageArr.get()));//针对mImageArr生成的mGrabCut
        //先load照片再利用照片initSystem
		initSystem();

		mImgView->update();
        //设置系统名字
		setWindowTitle( tr("%1 - %2")
            .arg(strippedName(fileName))//传入文件绝对路径得到“文件名.类型"
			.arg(WINDOW_TITLE) );
	}
}

//保存文件
void MainWindow::saveAs()
{
    string folerName = "/Users/Zoking/Documents/WorkSpace/QTworkspace/project1/autoMattingv1_0/res_set";
    QString fileName1 = QString(folerName.c_str()) + mfileName;

	QString fileName = QFileDialog::getSaveFileName(this, 
        tr("Save Result"), fileName1,tr("Images (*.png *.jpg *.bmp)"));
	saveAsImageFile(fileName);
}
//打开新的workshop window
void MainWindow::newWin()
{
    MainWindow *nw = new MainWindow();
    nw->show();
    nw->resize(800,600);
    this->update();
}

//触发refineOnce
void MainWindow::triggerrefineOnce(){
    if (mImages[VM_IMAGE].isNull()){
        cout<<"refine nothing"<<endl;
        return;
    }else if(mInitialized == false){
        //  此处添加自动抠图！！！
        cout<<"circle the rect init"<<endl;
        int s_x=39,s_y=30,e_x=579,e_y=618;
        mGrabCut->initialize(s_x, s_y, e_x, e_y);
        mGrabCut->fitGMMs();
        mSelectionMode = SM_NONE;
        mInitialized = true;
        mShowMask = true;
        mShowMaskAct->setChecked(mShowMask);
        updateImages();//更新鼠标操作后的图片
    }
    cout<<"magic once "<<endl;
    mGrabCut->refineOnce();
    updateImages();
    mImgView->update();
    updateInformationBar();
}

//打开相册
void MainWindow::openGallery()
{
    cout<<"openGallery()"<<endl;
    GalleryWindow *g = new GalleryWindow();
    g->show();
    g->resize(800,600);
    this->update();
}

//打开图像
void MainWindow::openImage(const QString& fileName)
{
    QImage img;
    img.load(fileName);
    int max_ = max(img.width(),img.height());
    QImage new_Image = img.scaled(VIEWER_HEIGHT*img.width()/max_,VIEWER_HEIGHT*img.height()/max_);
    mImages[VM_IMAGE] = new_Image;
}

//保存前景图像
void MainWindow::saveAsImageFile(const QString& fileName)
{
	QImage img;
	switch (mViewMode)
	{
	case VM_IMAGE:
		img  = mImages[VM_IMAGE].convertToFormat(QImage::Format_ARGB32);
		copy_alpha_array_to_QImage(mGrabCut->getAlphaImage(), img, false);
		break;
	default:
		img = mImages[mViewMode];
		break;
	}
	img.save(fileName);
}

//关于
void MainWindow::about()
{
	QMessageBox::about(this, tr("About %1").arg(WINDOW_TITLE),
		tr("<p>A <b>Graph Cut</b> project writed by maxint, in July, 2010.</p>"
		"<p>Email: <a href='mailto:lnychina@gmail.com'>lnychina@gmail.com</a></p>"
		"<p>Blog: <a href='http://hi.baidu.com/maxint'>http://hi.baidu.com/maxint</a></p>"));
}

//鼠标操作
bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
	if (obj->isWidgetType() &&
		obj->objectName() == WIDGET_NAME)
	{
		QWidget *wid = static_cast<QWidget*>(obj);
		if (e->type() == QEvent::Paint)
		{
            static QPen penDarkCyan(QBrush(Qt::darkCyan), 2);//掩膜区域
            static QPen penBlue(QBrush(Qt::blue), 4);//圈出前景、背景(Rect)
            static QPen penRed(QBrush(Qt::red), 4);//标注前景区域
			static QPainter painter;
			painter.begin(wid);
			QRect rect(QPoint(0,0), wid->size());
            painter.setBackground(QColor(200, 200, 200));
//            painter.setBackground(QColor(35, 35, 35));
            painter.drawImage(rect, mImages[mViewMode]);//圈出感兴趣区域
			if (mShowMask)
				painter.drawImage(rect, mImgMask);
			switch (mSelectionMode)
			{
			case SM_RECT:
				painter.setPen(penDarkCyan);
				painter.drawRect(QRect(mStartPos, mLastPos));
				break;
			case SM_PAINT_FG:
				painter.setPen(penRed);
				break;
			case SM_PAINT_BG:
				painter.setPen(penBlue);
				break;
			default:
				break;
			}
			if (mSelectionMode == SM_PAINT_FG || mSelectionMode == SM_PAINT_BG)
			{		
				painter.drawPoints(mPaintingPoses.data(), mPaintingPoses.size());
			}
			painter.end();
			e->accept();
			return true;
		}
		else if (mImages[VM_IMAGE].isNull())
		{
			// skip null image
		}
		else if (e->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent *me = static_cast<QMouseEvent*>(e);
            if (mInitialized)//初始化过
			{
				mPaintingPoses.clear();
                if (me->buttons() & Qt::LeftButton)//左键标注前景
				{
					mSelectionMode = SM_PAINT_FG;
				}
				else
				{
                    mSelectionMode = SM_PAINT_BG;//右键标注背景
				}
				mPaintingPoses.append(me->pos());
			}
            else//未初始化
			{
				if (me->buttons() & Qt::LeftButton)
				{
					mStartPos = mLastPos = me->pos();
					mSelectionMode = SM_RECT;
				}
			}
			mImgView->update();
		}
		else if (e->type() == QEvent::MouseMove)
		{
			QMouseEvent *me = static_cast<QMouseEvent*>(e);
			mLastPos = me->pos();
			if (mInitialized)
			{
				mPaintingPoses.append(mLastPos);
			}
			mImgView->update();
		}
		else if (e->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent *me = static_cast<QMouseEvent*>(e);
			mEndPos = me->pos();
			if (mInitialized)
			{
				mPaintingPoses.append(mEndPos);
				for (int i=0; i<mPaintingPoses.size(); ++i)
				{
					const QPoint &pt = mPaintingPoses.at(i);
					mGrabCut->setTrimap(pt.x()-2, pt.y()-2, pt.x()+2, pt.y()+2, 
						(mSelectionMode==SM_PAINT_FG) ? GrabCutNS::TrimapForeground : GrabCutNS::TrimapBackground);
				}
				mGrabCut->refineOnce();
				mGrabCut->buildImages();
				mSelectionMode = SM_NONE;
			}
			else
			{
				mGrabCut->initialize(mStartPos.x(), mStartPos.y(), mEndPos.x(), mEndPos.y());
                cout<<"位置："<<mStartPos.x() <<","<< mStartPos.y()<<";"<< mEndPos.x()<<","<< mEndPos.y()<<endl;
				mGrabCut->fitGMMs();
				mSelectionMode = SM_NONE;
				mInitialized = true;
				mShowMask = true;
				mShowMaskAct->setChecked(mShowMask);
			}
            updateImages();//更新鼠标操作后的图片
			mImgView->update();
		}
	}
	
	return QMainWindow::eventFilter(obj, e);
}

//获得文件名
QString MainWindow::strippedName(const QString& fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

//更新信息(暂时无用)
void MainWindow::updateInformationBar()
{
	QString info = tr("<style type='text/css'><!--"
		".info, { margin: 5px; }"
		"--></style>");
	info += tr("<table class='info'>");
	//info += tr("<tr><td>Primitives: </td><td>%1</td></tr>").arg(mEngine->getNumOfPrimitives());
	//info += tr("<tr><td>Resolution: </td><td>%1 x %2</td></tr>")
	//	.arg(mImage.width()).arg(mImage.height());
	//info += tr("<tr><td>Last cost time: </td><td>%1 ms</td></tr>").arg(mLastCostTime);
	//info += tr("<tr><td>Trace depth: </td><td>%1</td></tr>").arg(mEngine->getTraceDepth());
	//info += tr("<tr><td>Regular Samples: </td><td>%1</td></tr>").arg(mEngine->getRegularSampleSize());
	info += tr("</table>");
	mInfoLabel->setText(info);
}
