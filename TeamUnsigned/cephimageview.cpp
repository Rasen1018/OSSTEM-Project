#include "cephimageview.h"

#include <QMimeData>
#include <QFile>
#include <QFileDialog>

CephImageView::CephImageView(QWidget *parent)
    : QWidget{parent}
{
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

    connect(&m_zoomIn, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="")
            scaleImage(1.1);
    });

    connect(&m_zoomOut, &QPushButton::clicked, [this]{
        if(m_scaleLabel.text() !="100.0%" && m_scaleLabel.text() !="")
            scaleImage(1.0/1.1);
    });

    connect(&m_zoomReset, &QPushButton::clicked, [this]{
        if(m_scaleFactor != 1){
            m_imageLabel->resize(cephViewWidth,cephViewHeight);
            m_scaleFactor=1;
            m_scaleLabel.setText("100.0%");
        }
    });

    connect(&m_histo, &QPushButton::clicked, [this]{
        if(viewPixmap.isNull()) return;

        histogram = new Histogram();
        connect(this, SIGNAL(sendHisto(QPixmap&)),
                histogram, SLOT(receiveHisto(QPixmap&)));
        emit sendHisto(viewPixmap);
        delete histogram;

    });

}

void CephImageView::scaleImage(double factor)
{
    m_scaleFactor *= factor;
    m_scaleLabel.setText(QStringLiteral("%1%").arg(m_scaleFactor*100, 0, 'f', 1));
    QSize size = m_imageLabel->pixmap().size() * m_scaleFactor;
    m_imageLabel->resize(size);
}

/* panoramaForm 에서 로드 버튼 클릭 시 or 연산 후,  뷰로 픽스맵 데이터 전송하는 함수. */
void CephImageView::receiveLoadImg(QPixmap pixmap)
{
    viewPixmap = pixmap.scaled(cephViewWidth, cephViewHeight);
        m_imageLabel->setPixmap(pixmap.scaled(cephViewWidth, cephViewHeight));
        scaleImage(1.0);

        prevImg = pixmap.scaled(cephWidth, cephHeight).toImage();

        emit sendCephPrev(viewPixmap);

}

void CephImageView::receiveResetCeph(QPixmap& pixmap)
{
    m_imageLabel->setPixmap(pixmap.scaled(cephViewWidth, cephViewHeight));
    scaleImage(1.0);

    viewPixmap = pixmap.scaled(cephViewWidth, cephViewHeight);
}

void CephImageView::receiveSaveCeph()
{
    emit sendSave(prevImg);
}

void CephImageView::updateStyleSheet()
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

