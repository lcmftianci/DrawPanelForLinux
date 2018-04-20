#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolBar>
#include <QFileDialog>
#include <QIcon>
#include <QDebug>
#include <QDir>
#include <QPoint>
#include <QCursor>
#include <QClipboard>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QColorDialog>
#include <QStyle>
#include <QRadioButton>
#include <Qt>
#include <QTextCodec>
#include <QStandardPaths>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow),m_wigth(10),m_style(1),m_color(Qt::red),
    m_xPropertion(1),m_yPropertion(1),m_nRealWidth(1),m_nRealHeight(1),
    m_AreaNum(0),m_strFilePath(""),m_xOldPropertion(1),m_yOldPropertion(1),
    m_nCurType(0),m_nPreType(0),m_bBegin(true),m_bCanMoveView(false),m_nCurDetector(0),
    m_bCanMovePoint(false),m_nPointIndex(0)
{
    ui->setupUi(this);

    InitView();
    InitToolBar();
    InitRedioGroup();
    UpdatePen();
    m_pWidthSpinBox->setRange(1, 200);  // 范围
    m_pWidthSpinBox->setSingleStep(1); // 步长
    m_pWidthSpinBox->setValue(m_wigth);
    qDebug() << m_wigth;
   // QPoint position = ui->label->rect().topLeft();
   // m_LabelPos.setX(position.x() + ui->listWidget->geometry().width() + 175);
   // m_LabelPos.setY(position.y() + ui->mainToolBar->height() + ui->menuBar->height() + 105);
   // qDebug() << "起点坐标:" << m_LabelPos.x() << "---" << m_LabelPos.y();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_painter = new QPainter;
    m_pTimer = new QTimer();
    m_pTimer->setInterval(1);

    m_pXmlManager = new XMLManager;

    //time signal
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimerSlot()));
    m_arrDetector.clear();
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitView()
{
    m_icon = QIcon("/stylesheet/info.png");
    m_pix = QPixmap("/stylesheet/info.png");
    m_pPix = new QPixmap("/stylesheet/info.png");
    QObject::connect(ui->actionOpenImage, SIGNAL(triggered()), this, SLOT(LoadImage()));
    QObject::connect(ui->actionResetConfig, SIGNAL(triggered()), this, SLOT(ResetView()));
    QObject::connect(ui->actionLoadXml, SIGNAL(triggered()), this, SLOT(OnLoadData()));
    QObject::connect(ui->actionSaveXml, SIGNAL(triggered()), this, SLOT(OnSaveData()));
    //动态记载一些功能选项
}

void MainWindow::InitToolBar()
{
   QTextCodec *pCoder = QTextCodec::codecForName("gb2312");
   m_pStyleLabel = new QLabel(pCoder->toUnicode("  线型风格: "));  //线型风格
   m_pStyleComboBox = new QComboBox;
   m_pStyleComboBox->addItem(tr("SolodLine") , static_cast<int>(Qt::SolidLine));
   m_pStyleComboBox->addItem(tr("DashLine") , static_cast<int>(Qt::DashLine));
   m_pStyleComboBox->addItem(tr("DotLine") , static_cast<int>(Qt::DotLine));
   m_pStyleComboBox->addItem(tr("DashDotLine") , static_cast<int>(Qt::DashDotLine));
   m_pStyleComboBox->addItem(tr("DashDotDotLine") , static_cast<int>(Qt::DashDotDotLine));
   m_pWidthLabel = new QLabel(pCoder->toUnicode("  线宽: "));
   m_pWidthSpinBox = new QSpinBox(this);
   connect(m_pWidthSpinBox,SIGNAL(valueChanged(int)), this,SLOT(OnSetWidth(int)));
   connect(m_pStyleComboBox,SIGNAL(activated(int)),this,SLOT(OnShowStyle()));
   m_pStyleComboBox->setCurrentIndex(1);

   m_pColorLabel = new QLabel(pCoder->toUnicode("  画笔颜色: "));
   m_pColorBtn = new QToolButton; //设置按钮的格式
   QPixmap pixmap(30,30);
   pixmap.fill(Qt::red);
   m_pColorBtn->setIcon(QIcon(pixmap));
   connect(m_pColorBtn,SIGNAL(clicked(bool)),this,SLOT(OnSetColor()));

//   m_pColorBtn = new QToolButton;
//   m_pColorBtn->setText(tr("清除"));
//   connect(m_pColorBtn,SIGNAL(clicked(bool)), this,SLOT(clear()));

   ui->mainToolBar->addWidget(m_pStyleLabel);
   ui->mainToolBar->addWidget(m_pStyleComboBox);
   ui->mainToolBar->addWidget(m_pWidthLabel);
   ui->mainToolBar->addWidget(m_pWidthSpinBox);
   ui->mainToolBar->addWidget(m_pColorLabel);
   ui->mainToolBar->addWidget(m_pColorBtn);
}

void MainWindow::UpdatePen()
{
//     QPen pen;
//     pen.setStyle((Qt::PenStyle)m_style);
//     pen.setWidth(m_wight);
//     pen.setColor(m_color);

     //m_painter->begin(m_pPix);
     //m_painter->setPen(pen);
     //m_painter->drawLine(startPos, e->pos());
     //m_painter->end();
     //startPos = e->pos();
    update();
}

//初始化RadioBtn
void MainWindow::InitRedioGroup()
{
    QVBoxLayout *pLayout = new QVBoxLayout();
    m_pBtnGroup = new QButtonGroup(this);
    m_pBtnGroup->setExclusive(true);
    m_pRadioArea = new QRadioButton(this);
    //m_pRadioArea->setLayoutDirection(Qt::LayoutDirection::Vertical);
    char chText[100];
    sprintf(chText, "%s", "检测区域");
    QTextCodec *pCoder = QTextCodec::codecForName("gb2312");
    m_pRadioArea->setText(pCoder->toUnicode(chText));
    pLayout->addWidget(m_pRadioArea);
    m_pBtnGroup->addButton(m_pRadioArea);
    m_pRadioTriLine = new QRadioButton(this);

    sprintf(chText, "%s", "报警线");
    m_pRadioTriLine->setText(pCoder->toUnicode(chText));
    pLayout->addWidget(m_pRadioTriLine);
    m_pBtnGroup->addButton(m_pRadioTriLine);

    m_pRadioComplete = new QRadioButton(this);
    sprintf(chText, "%s", "完成");
    m_pRadioComplete->setText(pCoder->toUnicode(chText));
    pLayout->addWidget(m_pRadioComplete);
    m_pBtnGroup->addButton(m_pRadioComplete);

    pLayout->setSpacing(10);
    pLayout->setContentsMargins(10, 10, 10, 10);
    ui->scrollArea->setLayout(pLayout);

    //m_pRadioArea->setFocus();
    m_pRadioArea->setChecked(true);

    // 连接信号槽
    connect(m_pBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(OnRadioChecked(QAbstractButton*)));
}

int MainWindow::DelPoint()
{
    qDebug() << "del point---";

    if(m_arrDetector.size() > 0)
    {
        std::vector<Detector*>::iterator iterDetector = m_arrDetector.end();
        iterDetector--;
        if((*iterDetector)->GetPointSize() == 1)
        {
            //detete *iterDetector;
            m_arrDetector.erase(m_arrDetector.end() - 1);
            //if(m_arrDetector.size()>0)
            //m_qCurPt = (*iterDetector--)->GetLastPoint();
        }
        else
        {
            (*iterDetector)->DeletePoint();
            //m_qCurPt = (*iterDetector)->GetLastPoint();
        }
    }

    if(m_arrDetector.size() == 0)
    {
        m_bBegin = true;
        m_arrDetector.clear();
        qDebug() << "zero";
    }

    PaintBitmap();
    return 0;
}

int MainWindow::DrawBitmap(int x, int y)
{
    //qDebug() << "鼠标点击:" << event->x() << "---" << event->y();
    //qDebug() << "label区域:" << ui->label->x() << "---" << ui->label->y();
    //if(event->y() < (ui->label->y() + ui->mainToolBar->height() + ui->menuBar->height())
    //        || event->y() > (ui->label->y() + ui->mainToolBar->height() + ui->menuBar->height() + ui->label->height()))
    //   return 1;
    if(x < (ui->label->y() + ui->mainToolBar->height() + ui->menuBar->height())
            || y > (ui->label->y() + ui->mainToolBar->height() + ui->menuBar->height() + ui->label->height()))
       return 1;

    if(m_nCurType == 2)
        return 2;

    if(m_strFilePath != "")
    {
        m_realPix = m_pix.scaled(ui->label->width(), ui->label->height());
        Detector* pDetector;
        if(m_AreaNum == 0)
        {
            pDetector = new Detector;
            //m_oldPt = event->globalPos();
            m_oldPt = m_qCurGlobal;
            m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
            pDetector->PushPoint(m_GlobalOldPos);
            pDetector->SetColor(m_color);
            pDetector->SetWidth(m_wigth);
            if(m_nCurType == 0)
                pDetector->SetType(nDetArea);
            else if(m_nCurType == 1)
            {
                pDetector->SetType(nTriLine);
                m_bBegin = false;
            }
            m_arrDetector.push_back(pDetector);
            m_nPreType = m_nCurType;
        }
        else
        {
            if(m_nCurType == m_nPreType)
            {
                std::vector<Detector*>::iterator iterDetector;
                qDebug() << "Draw Pixmap";
                //m_oldPt = event->globalPos();
                m_oldPt = m_qCurGlobal;
                m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);

                if(m_bBegin == true && m_nCurType == 1)
                {
                    pDetector = new Detector;
                    //m_oldPt = event->globalPos();
                    m_oldPt = m_qCurGlobal;
                    m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
                    pDetector->PushPoint(m_GlobalOldPos);
                    pDetector->SetColor(m_color);
                    pDetector->SetWidth(m_wigth);
                    if(m_nCurType == 0)
                        pDetector->SetType(nDetArea);
                    else if(m_nCurType == 1)
                    {
                        pDetector->SetType(nTriLine);
                        m_bBegin = false;
                    }
                    m_arrDetector.push_back(pDetector);
                    iterDetector = m_arrDetector.begin();
                    for(; iterDetector != m_arrDetector.end() - 1; iterDetector++)
                    {
                        QPen pen;
                        pen.setStyle((Qt::PenStyle)m_style);
                        pen.setWidth((*iterDetector)->GetWidth());
                        pen.setColor((*iterDetector)->GetColor());
                        QPainter qp(&m_realPix);
                        qp.setPen(pen);
                        std::vector<QPoint> arrPoints;
                        (*iterDetector)->GetPoints(arrPoints);
                        if((*iterDetector)->GetType() == nDetArea)
                        {
                            for(int i = 0; i < arrPoints.size()-1; i++)
                            {
                                qp.drawLine(arrPoints[i], arrPoints[i+1]);
                            }
                            qp.drawLine(arrPoints[0], arrPoints[arrPoints.size()-1]);
                        }
                        else if((*iterDetector)->GetType() == nTriLine)
                        {
                            qp.drawLine(arrPoints[0], arrPoints[1]);
                        }
                        qp.end();
                    }
                }
                else
                {
                    if(m_nCurType == 1 && m_bBegin == false)
                        m_bBegin = true;
                    iterDetector = m_arrDetector.end();
                    iterDetector--;
                    (*iterDetector)->PushPoint(m_GlobalOldPos);
                    iterDetector = m_arrDetector.begin();
                    for(; iterDetector != m_arrDetector.end(); iterDetector++)
                    {
                        QPen pen;
                        pen.setStyle((Qt::PenStyle)m_style);
                        pen.setWidth((*iterDetector)->GetWidth());
                        pen.setColor((*iterDetector)->GetColor());
                        QPainter qp(&m_realPix);
                        qp.setPen(pen);
                        std::vector<QPoint> arrPoints;
                        (*iterDetector)->GetPoints(arrPoints);
                        if((*iterDetector)->GetType() == nDetArea)
                        {
                            for(int i = 0; i < arrPoints.size()-1; i++)
                            {
                                qp.drawLine(arrPoints[i], arrPoints[i+1]);
                            }
                            qp.drawLine(arrPoints[0], arrPoints[arrPoints.size()-1]);
                        }
                        else if((*iterDetector)->GetType() == nTriLine)
                        {
                            qp.drawLine(arrPoints[0], arrPoints[1]);
                        }
                        qp.end();
                    }
                }
            }
            else
            {
                pDetector = new Detector;
                //m_oldPt = event->globalPos();
                m_oldPt = m_qCurGlobal;
                m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
                pDetector->PushPoint(m_GlobalOldPos);
                pDetector->SetColor(m_color);
                pDetector->SetWidth(m_wigth);
                if(m_nCurType == 0)
                    pDetector->SetType(nDetArea);
                else if(m_nCurType == 1)
                {
                    pDetector->SetType(nTriLine);
                    m_bBegin = false;
                }
                m_arrDetector.push_back(pDetector);
                m_nPreType = m_nCurType;

                qDebug() << "Draw Pixmap";
                std::vector<Detector*>::iterator iterDetector = m_arrDetector.begin();
                for(; iterDetector != m_arrDetector.end() - 1; iterDetector++)
                {
                    QPen pen;
                    pen.setStyle((Qt::PenStyle)m_style);
                    pen.setWidth((*iterDetector)->GetWidth());
                    pen.setColor((*iterDetector)->GetColor());
                    QPainter qp(&m_realPix);
                    qp.setPen(pen);
                    std::vector<QPoint> arrPoints;
                    (*iterDetector)->GetPoints(arrPoints);
                    if((*iterDetector)->GetType() == nDetArea)
                    {
                        for(int i = 0; i < arrPoints.size()-1; i++)
                        {
                            qp.drawLine(arrPoints[i], arrPoints[i+1]);
                        }
                        qp.drawLine(arrPoints[0], arrPoints[arrPoints.size()-1]);
                    }
                    else if((*iterDetector)->GetType() == nTriLine)
                    {
                        qp.drawLine(arrPoints[0], arrPoints[1]);
                    }
                    qp.end();
                }
            }
        }
        qDebug() << "detector:" << m_arrDetector.size();
        m_AreaNum++;
        update();
    }
}

//遍历成员变量绘制点
int MainWindow::PaintBitmap()
{
    m_realPix = m_pix.scaled(ui->label->width(), ui->label->height());
    std::vector<Detector*>::iterator iterDetector = m_arrDetector.begin();
    for(; iterDetector != m_arrDetector.end(); iterDetector++)
    {
        QPen pen;
        pen.setStyle((Qt::PenStyle)m_style);
#ifdef COLORDEF
        pen.setWidth((*iterDetector)->GetWidth());
        pen.setColor((*iterDetector)->GetColor());
#else
        pen.setWidth(10);
        pen.setColor(m_color);
#endif
        QPainter qp(&m_realPix);
        qp.setPen(pen);
        std::vector<QPoint> arrPoints;
        (*iterDetector)->GetPoints(arrPoints);
        for(int i = 0; i < arrPoints.size() - 1; i++)
        {
            qp.drawLine(arrPoints[i], arrPoints[i+1]);
            pen.setColor(QColor::fromRgb(255,125,125));
            qp.setPen(pen);
            qp.drawEllipse(arrPoints[i].x() - 5,arrPoints[i].y() - 5, 10, 10);
#ifdef COLORDEF
            pen.setColor((*iterDetector)->GetColor());
#else
           pen.setColor(m_color);
#endif
            qp.setPen(pen);
        }
        qp.drawLine(arrPoints[0], arrPoints[arrPoints.size() - 1]);
        pen.setColor(QColor::fromRgb(255,125,125));
        qp.setPen(pen);
        qp.drawEllipse(arrPoints[arrPoints.size() - 1].x() - 5,arrPoints[arrPoints.size() - 1].y() - 5, 10, 10);
#ifdef COLORDEF
            pen.setColor((*iterDetector)->GetColor());
#else
           pen.setColor(m_color);
#endif
        qp.setPen(pen);
        qp.end();
    }
    return 0;
}

//生成数据
int MainWindow::DrawPoint()
{
    Detector* pDetector;
    if(m_nCurType == 0)
    {
        if(m_bBegin)
        {
            pDetector = new Detector;
            m_bBegin = false;
            m_oldPt = m_qCurGlobal;
            m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
            pDetector->PushPoint(m_GlobalOldPos);
            pDetector->SetColor(m_color);
            pDetector->SetWidth(m_wigth);
            pDetector->SetType(nDetArea);
            m_arrDetector.push_back(pDetector);
        }
        else
        {
            m_oldPt = m_qCurGlobal;
            m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
            std::vector<Detector*>::iterator iterDetector = m_arrDetector.end();
            iterDetector--;
            (*iterDetector)->PushPoint(m_GlobalOldPos);
        }
    }
    else if(m_nCurType == 1)
    {
        if(m_bBegin)
        {
            pDetector = new Detector;
            m_bBegin = false;
            m_oldPt = m_qCurGlobal;
            m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
            pDetector->PushPoint(m_GlobalOldPos);
            pDetector->SetColor(m_color);
            pDetector->SetWidth(m_wigth);
            pDetector->SetType(nTriLine);
            m_arrDetector.push_back(pDetector);
        }
        else
        {
            m_oldPt = m_qCurGlobal;
            m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
            std::vector<Detector*>::iterator iterDetector = m_arrDetector.end();
            iterDetector--;
            (*iterDetector)->PushPoint(m_GlobalOldPos);
            m_bBegin = true;
        }
    }
    return 0;
}

void MainWindow::LoadImage()
{
     m_strFilePath = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpeg *.jpg *.bmp)"));
     if(m_strFilePath == "")
         return;
     m_pix = QPixmap(m_strFilePath);
     m_nPixWidth = m_pix.width();
     m_nPixHeight = m_pix.height();
     m_realPix = m_pix.scaled(ui->label->width(), ui->label->height());
     m_nRealHeight = m_realPix.height();
     m_nRealWidth = m_realPix.width();

     m_yPropertion = m_nPixHeight/m_nRealHeight;
     m_xPropertion = m_nPixWidth/m_nRealWidth;
     m_yOldPropertion = m_yPropertion;
     m_xOldPropertion = m_xPropertion;

     if(m_strFilePath != "")
         m_arrDetector.clear();

#if 0
     if(m_nRealHeight != 0 && m_nRealWidth != 0)
     {
        m_yPropertion = m_nPixHeight/m_nRealHeight;
        m_xPropertion = m_nPixWidth/m_nRealWidth;
        if(m_yOldPropertion == 1 || m_xOldPropertion == 1)
        {
            m_yOldPropertion = m_yPropertion;
            m_xOldPropertion = m_xPropertion;
        }
        else
        {
            std::vector<Detector*>::iterator iterDetector = m_arrDetector.end();
            for(; iterDetector != m_arrDetector.end(); iterDetector++)
            {
                QPen pen;
                pen.setStyle((Qt::PenStyle)m_style);
                pen.setWidth(m_wigth);
                pen.setColor(m_color);
                QPainter qp(&m_realPix);
                qp.setPen(pen);
                std::vector<QPoint> arrPoints;
                (*iterDetector)->GetPoints(arrPoints);
                for(int i = 0; i < arrPoints.size()-1; i++)
                {
                    arrPoints[i] = QPoint(arrPoints[i].x() * m_xOldPropertion/m_xPropertion, arrPoints[i].y() * m_xOldPropertion/m_xPropertion);
                }
                for(int i = 0; i < arrPoints.size()-1; i++)
                {
                    qp.drawLine(arrPoints[i], arrPoints[i+1]);
                }
                qp.end();
            }
            m_yOldPropertion = m_yPropertion;
            m_xOldPropertion = m_xPropertion;
        }
     }
#endif
     qDebug() << m_strFilePath;
     qDebug() << QDir::currentPath();
}

void MainWindow::ResetView()
{
    m_arrDetector.clear();
    PaintBitmap();
}

void MainWindow::OnLoadData()
{
    QStringList strFileList = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    m_strXmlPath = QFileDialog::getOpenFileName(this, tr("Open XML"), strFileList[0], tr("Xml Files (*.xml)"));
    qDebug() << m_strXmlPath;
    m_arrDetector.clear();
    if(m_strXmlPath != "")
    {
        m_pXmlManager->LoadXML(m_strXmlPath);
        m_strFilePath = m_pXmlManager->GetImageData()->strImagePath;
        qDebug() << "------------------------" << m_strFilePath;
        m_pix = QPixmap(m_strFilePath);
        m_realPix = m_pix.scaled(ui->label->geometry().width(), ui->label->geometry().height());
        m_pXmlManager->GetDetectorData(m_arrDetector);
        qDebug() << m_arrDetector.size() << "size";
        PaintBitmap();
    }
}

void MainWindow::OnSaveData()
{
    if(m_strFilePath != "")
    {
        QFileInfo fi = QFileInfo(m_strFilePath);
        QString file_name = fi.fileName();  //获取文件名
        QString file_path = fi.absolutePath(); //获取文件路径(不包含文件名)
        file_path += "/ImageData.xml";
        qDebug() << file_path;
        file_path = QFileDialog::getSaveFileName(this, tr("Save File"),  file_path, tr("XML (*.xml)"));
        ImageData imgData;
        imgData.nHeight = m_realPix.height();
        imgData.nWidth = m_realPix.width();
        imgData.strImagePath = m_strFilePath;
        m_pXmlManager->SetImageData(&imgData);
        m_pXmlManager->ClearData();
        m_pXmlManager->SetDetectorData(m_arrDetector);
        m_pXmlManager->SaveXML(file_path);
    }
}

void MainWindow::OnShowStyle()
{
    m_style = m_pStyleComboBox->itemData(m_pStyleComboBox->currentIndex(),Qt::UserRole).toInt();
    UpdatePen();
}

void MainWindow::OnSetWidth(int nWidth)
{
    m_wigth = nWidth;
    UpdatePen();
}


void MainWindow::OnSetColor()
{
    m_color = QColorDialog::getColor(static_cast<int>(Qt::black) ,this);
    if(m_color.isValid())
    {
        QPixmap p(20,20);
        p.fill(m_color);
        m_pColorBtn->setIcon(QIcon(p));
    }
    UpdatePen();
}

void MainWindow::OnRadioChecked(QAbstractButton *aBtn)
{
    // 当前点击的按钮
        qDebug() << QString("Clicked Button : %1").arg(aBtn->text());
        m_nPreType = m_nCurType;

        // 遍历按钮，获取选中状态
        QList<QAbstractButton*> list = m_pBtnGroup->buttons();
        int i = 0;
        foreach (QAbstractButton *pButton, list)
        {
            if(pButton->isChecked())
            {
                QString strStatus = pButton->isChecked() ? "Checked" : "Unchecked";
                qDebug() << QString("Button : %1 is %2").arg(aBtn->text()).arg(strStatus);
                break;
            }
            i++;
        }

        if(i == 0)
        {
            qDebug() << QString("Button : %1 is %2").arg(aBtn->text()).arg(aBtn->isChecked());
            m_nCurType = 0;
            m_bBegin = true;
        }
        else if(i == 1)
        {
            qDebug() << QString("Button : %1 is %2").arg(aBtn->text()).arg(aBtn->isChecked());
            m_nCurType = 1;
            m_bBegin = true;
        }
        else if(i == 2)
        {
            qDebug() << QString("Button : %1 is %2").arg(aBtn->text()).arg(aBtn->isChecked());
            m_nCurType = 2;
            m_bBegin = true;
        }
}

void MainWindow::OnTimeSlot()
{
    //m_qPreGlobal = QCursor::pos();
    //m_qPreGlobal = ui->label->mapFromGlobal(QCursor::pos());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //ui->label->setPixmap(m_pix.scaled(ui->label->geometry().width(), ui->label->geometry().height()));
    //QPoint position = ui->label->rect().topLeft();
    //m_LabelPos.setX(position.x());
    //m_LabelPos.setY(position.y() + ui->mainToolBar->height() + ui->menuBar->height() + 10);
    //qDebug() << "起点坐标:" << m_LabelPos.x() << "---" << m_LabelPos.y();
    //ui->label->updateGeometry();
#if 0
    ui->label->setAlignment(Qt::AlignCenter);		//居中显示
    m_realPix = m_pix.scaled(ui->label->width(), ui->label->height());
    m_nRealHeight = m_realPix.height();
    m_nRealWidth = m_realPix.width();
    m_yOldPropertion = m_yPropertion;
    m_xOldPropertion = m_xPropertion;

    if(m_nRealHeight != 0 && m_nRealWidth != 0)
    {
       m_yPropertion = m_nPixHeight/m_nRealHeight;
       m_xPropertion = m_nPixWidth/m_nRealWidth;
       {
           std::vector<Detector*>::iterator iterDetector = m_arrDetector.end();
           for(; iterDetector != m_arrDetector.end(); iterDetector++)
           {
               QPen pen;
               pen.setStyle((Qt::PenStyle)m_style);
               pen.setWidth(m_wigth);
               pen.setColor(m_color);
               QPainter qp(&m_realPix);
               qp.setPen(pen);
               std::vector<QPoint> arrPoints;
               (*iterDetector)->GetPoints(arrPoints);
               for(int i = 0; i < arrPoints.size()-1; i++)
               {
                   arrPoints[i] = QPoint(arrPoints[i].x() * m_xPropertion/m_xOldPropertion, arrPoints[i].y() * m_xPropertion/m_xOldPropertion);
               }
               for(int i = 0; i < arrPoints.size()-1; i++)
               {
                   qp.drawLine(arrPoints[i], arrPoints[i+1]);
               }
               qp.end();
               (*iterDetector)->PushPoint(arrPoints);
           }
           m_yOldPropertion = m_yPropertion;
           m_xOldPropertion = m_xPropertion;
       }
    }
#endif
    PaintBitmap();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "PressEvent";
    if(event->y() < (ui->label->y() + ui->mainToolBar->height() + ui->menuBar->height())
            || event->y() > (ui->label->y() + ui->mainToolBar->height() + ui->menuBar->height() + ui->label->height()))
       return;
    if(m_nCurType == 2)
    {
        //处理用户选择区域是否符合要求
        //m_qCurPt = QPoint(event->x(), event->y());
        m_qCurGlobal = event->globalPos();
        m_GlobalOldPos = ui->label->mapFromGlobal(m_qCurGlobal);
         m_qPreGlobal = m_GlobalOldPos;
        std::vector<Detector*>::iterator iterDetector = m_arrDetector.begin();
        int i = 0;
        for(; iterDetector != m_arrDetector.end(); iterDetector++)
        {
            m_nPointIndex = (*iterDetector)->IsSamePoint(m_GlobalOldPos);
            if(m_nPointIndex >= 0)
            {
                m_nCurDetector = i;
                m_bCanMovePoint = true;
                qDebug() << "CanMove";
                return;
            }
            else if((*iterDetector)->IsInArea(m_GlobalOldPos))
            {
                m_nCurDetector = i;
                m_bCanMoveView = true;
            }
            i++;
        }
        return;
    }
#if 0
    m_oldPt = event->globalPos();
    m_GlobalOldPos = ui->label->mapFromGlobal(m_oldPt);
#endif
    m_qCurPt = QPoint(event->x(), event->y());
    m_qCurGlobal = event->globalPos();
    //DrawBitmap(m_qCurPt.x(), m_qCurPt.y());

    DrawPoint();
    PaintBitmap();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "ReleaseEvent";
    m_bCanMovePoint = false;
    m_bCanMoveView = false;
#if 0
    m_pt = event->globalPos();
    m_GlobalPos = ui->label->mapFromGlobal(m_pt);
    m_xOffSet = ui->label->pixmap()->rect().width();
    QPen pen;
    pen.setStyle((Qt::PenStyle)m_style);
    pen.setWidth(m_wight);
    pen.setColor(m_color);
    QPainter qp(&m_realPix);
    qp.setPen(pen);
    qp.drawLine(m_GlobalOldPos, m_GlobalPos);
    qp.end();
#endif
    update();
    //qDebug() << m_GlobalPos.x() << "---" << m_GlobalPos.y();
    //qDebug() << m_GlobalOldPos.x() << "---" << m_GlobalOldPos.y();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "moveEvent";
    //只有选择了完成才可以平移图像
    if(m_nCurType == 2 && m_bCanMovePoint)
    {
        qDebug() << "Is move";
        //处理用户选择区域是否符合要求
        //m_qCurPt = QPoint(event->x(), event->y());
        m_qCurGlobal = event->globalPos();
        m_GlobalOldPos = ui->label->mapFromGlobal(m_qCurGlobal);
        //if(m_arrDetector[m_nCurDetector]->IsSamePoint(m_GlobalOldPos) > 0)
        {
            m_arrDetector[m_nCurDetector]->MovePoint(m_GlobalOldPos, m_nPointIndex);
        }
#if 0
        std::vector<Detector*>::iterator iterDetector = m_arrDetector.begin();
        for(; iterDetector != m_arrDetector.end(); iterDetector++)
        {
            if((*iterDetector)->IsSamePoint(m_GlobalOldPos) > 0)
            {
                (*iterDetector)->MovePoint(m_GlobalOldPos);
            }
            else if((*iterDetector)->IsInArea(m_GlobalOldPos))
            {
                (*iterDetector)->MoveView(m_GlobalOldPos.x() - m_qPreGlobal.x(), m_GlobalOldPos.y() - m_qPreGlobal.y());
            }
        }
#endif
    }
    else if(m_nCurType == 2 && m_bCanMoveView)
    {
        m_qCurGlobal = event->globalPos();
        m_GlobalOldPos = ui->label->mapFromGlobal(m_qCurGlobal);
        //if(m_arrDetector[m_nCurDetector]->IsInArea(m_GlobalOldPos))
        {
            m_arrDetector[m_nCurDetector]->MoveView(m_GlobalOldPos.x() - m_qPreGlobal.x(), m_GlobalOldPos.y() - m_qPreGlobal.y());
        }
    }
    DrawPoint();
    PaintBitmap();

    m_qPreGlobal = m_GlobalOldPos;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //QPainter paiter(this);
    ui->label->setPixmap(m_realPix.scaled(ui->label->geometry().width(), ui->label->geometry().height()));
    //paiter.drawPixmap(m_LabelPos.x(), m_LabelPos.y(), ui->label->geometry().width(), ui->label->geometry().height(), m_realPix);
//    QPen pen;
//    pen.setStyle((Qt::PenStyle)m_style);
//    pen.setWidth(m_wight);
//    pen.setColor(m_color);
    //QPainter paiter(this);
    //paiter.setPen(pen);
    //paiter.drawLine(m_GlobalOldPos, m_GlobalPos);
}

//撤销
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "abc";
    if (event->modifiers() ==  Qt::ControlModifier && event->key() == Qt::Key_Z)
    {
           DelPoint();
    }
    else if(event->key() == Qt::Key_Right)
    {
           qDebug() << "del two";
    }
}
