#include "dentalimageview.h"
#include "histogram.h"

#include <QFile>
#include <QFileDialog>

DentalImageView::DentalImageView()
{
    /* ui 관련 선언 */
    m_layout.addWidget(&m_area, 0, 0, 1, 5);
    m_layout.addWidget(&m_zoomOut, 1, 0);
    m_layout.addWidget(&m_zoomIn, 1, 1);
    m_layout.addWidget(&m_scaleLabel, 1, 2);
    m_layout.addWidget(&m_zoomReset, 1, 3);
    m_layout.addWidget(&m_histo, 1, 4);
    m_imageLabel = new QLabel(this);
    m_area.setWidget(m_imageLabel);
    m_imageLabel->setScaledContents(true);

    updateStyleSheet();

    m_zoomIn.setAutoRepeat(true);
    m_zoomIn.autoRepeat();
    m_zoomIn.setAutoDefault(200);
    m_zoomOut.setAutoRepeat(true);
    m_zoomOut.autoRepeat();
    m_zoomOut.setAutoDefault(200);

    /* zoomIn 버튼 클릭 시, 배율 증가 */
    connect(&m_zoomIn, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="194.9%" && m_scaleLabel.text() !="")
            scaleImage(1.1);
    });

    /* zoomOut 버튼 클릭 시, 배율 감소 */
    connect(&m_zoomOut, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="100.0%" && m_scaleLabel.text() !="")
            scaleImage(1.0/1.1);
    });

    /* zoomReset 버튼 클릭 시, 배율 Reset */
    connect(&m_zoomReset, &QPushButton::clicked, [this]{
        if(m_scaleFactor != 1){
            m_imageLabel->resize(dentalViewWidth,dentalViewHeight);
            m_scaleFactor=1;
            m_scaleLabel.setText("100.0%");
        }
    });

    /* histo 버튼 클릭 시, 히스토그램 객체 생성 */
    connect(&m_histo, &QPushButton::clicked, [this]{
        if(viewPixmap.isNull()) return; //View에 이미지가 없는 경우 예외 처리

        histogram = new Histogram();
        connect(this, SIGNAL(sendHisto(QPixmap&)),
                histogram, SLOT(receiveHisto(QPixmap&)));
        emit sendHisto(viewPixmap);     // Histogram 클래스로 View의 이미지 전송
        delete histogram;
    });
}
void DentalImageView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        lastDragPos = event->pos();
    }
}
void DentalImageView::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = this->mapFromParent(event->pos());

    QPoint limit;
    if(m_scaleLabel.text() == "110.0%") limit=QPoint(-112,-71);
    else if(m_scaleLabel.text() == "121.0%") limit=QPoint(-222,-137);
    else if(m_scaleLabel.text() == "133.1%") limit=QPoint(-344,-211);
    else if(m_scaleLabel.text() == "146.4%") limit=QPoint(-477,-289);
    else if(m_scaleLabel.text() == "161.1%") limit=QPoint(-624,-377);
    else if(m_scaleLabel.text() == "177.2%") limit=QPoint(-785,-476);
    else if(m_scaleLabel.text() == "194.9%") limit=QPoint(-962,-581);

    if(m_scaleLabel.text() != "100.0%" && (pos-lastDragPos).x() <0 && (pos-lastDragPos).y() <0 &&
            (pos-lastDragPos).x() > limit.x() && (pos-lastDragPos).y() > limit.y()) {
        m_imageLabel->move(pos-lastDragPos);

    }
}


/* StyleSheet 설정 함수 */
void DentalImageView::updateStyleSheet()
{
    m_imageLabel->setStyleSheet("border: none;");
    m_scaleLabel.setStyleSheet("border: none;");

    m_zoomIn.setStyleSheet("QPushButton:hover\n"
                           "{\n"
                           "border: 2px solid red;\n"
                           "background-color: rgb(103, 104, 114);\n"
                           "}\n"
                           "QPushButton:pressed\n"
                           "{\n"
                           "background-color: rgb(35,190,212);\n"
                           "color: rgb(255, 255, 255);\n"
                           "}\n");
    m_zoomOut.setStyleSheet("QPushButton:hover\n"
                            "{\n"
                            "border: 2px solid red;\n"
                            "background-color: rgb(103, 104, 114);\n"
                            "}\n"
                            "QPushButton:pressed\n"
                            "{\n"
                            "background-color: rgb(35,190,212);\n"
                            "color: rgb(255, 255, 255);\n"
                            "}\n");
    m_histo.setStyleSheet("QPushButton:hover\n"
                          "{\n"
                          "border: 2px solid red;\n"
                          "background-color: rgb(103, 104, 114);\n"
                          "}\n"
                          "QPushButton:pressed\n"
                          "{\n"
                          "background-color: rgb(35,190,212);\n"
                          "color: rgb(255, 255, 255);\n"
                          "}\n");
    m_zoomReset.setStyleSheet("QPushButton:hover\n"
                              "{\n"
                              "border: 2px solid red;\n"
                              "background-color: rgb(103, 104, 114);\n"
                              "}\n"
                              "QPushButton:pressed\n"
                              "{\n"
                              "background-color: rgb(35,190,212);\n"
                              "color: rgb(255, 255, 255);\n"
                              "}\n");
}
/* panorama 이미지 배율에 맞게 Label크기를 조정하는 함수
 * @param Label의 배율
 */
void DentalImageView::scaleImage(double factor)
{
    m_scaleFactor *= factor;
    m_scaleLabel.setText(QStringLiteral("%1%").arg(m_scaleFactor*100, 0, 'f', 1));
    QSize size = m_imageLabel->pixmap().size() * m_scaleFactor;
    m_imageLabel->resize(size);
}
/* panoramaForm 에서 View로 픽스맵 데이터 받아오는 슬롯
 * @param panoramaForm에 load 한 이미지
 */
void DentalImageView::receiveLoadImg(QPixmap pixmap)
{
    viewPixmap = pixmap.scaled(dentalViewWidth, dentalViewHeight);
    m_imageLabel->setPixmap(pixmap.scaled(dentalViewWidth, dentalViewHeight));
    scaleImage(1.0);

    prevImg = pixmap.scaled(panoWidth, panoHeight).toImage();

    emit sendPanoPrev(viewPixmap);  //히스토 연산을 위한 시그널
}
/* panoramaForm에서 리셋 신호 전송 시, View 리셋 슬롯 */
void DentalImageView::receiveResetPano(QPixmap& pixmap)
{
    m_imageLabel->setPixmap(pixmap.scaled(dentalViewWidth, dentalViewHeight));
    scaleImage(1.0);

    viewPixmap = pixmap.scaled(dentalViewWidth, dentalViewHeight);
}
/* panoramaForm에서 저장 신호 전송 시, 현재 View의 Img 전송 슬롯 */
void DentalImageView::receiveSavePano()
{
    emit sendSave(prevImg);
}

