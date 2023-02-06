#ifndef DENTALIMAGEVIEW_H
#define DENTALIMAGEVIEW_H

#include "histogram.h"

#include <QWidget>

#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

class DentalImageView  : public QWidget
{
    Q_OBJECT;

public:
    DentalImageView();

    void scaleImage(double);

private:
    void updateStyleSheet();
    QGridLayout m_layout{this};
    QScrollArea m_area;
    QLabel *m_imageLabel, m_scaleLabel;
    QPushButton m_zoomOut{"Zoom Out"}, m_zoomIn{"Zoom In"}, m_zoomReset{"Zoom Reset"},
                m_histo{"Histogram"};
    double m_scaleFactor = 1.0;
    QImage prevImg;
    QPixmap viewPixmap;
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;

    int panoWidth = 3000;
    int panoHeight = 1628;

    Histogram* histogram;
private slots:
    void receiveLoadImg(QPixmap);
    void receiveResetPano(QPixmap&);
    void receiveSavePano();

signals:
    void send(QPixmap , QString);
    void sendSave(QImage&);
    void sendHisto(QPixmap&);
    void sendPanoPrev(QPixmap&);
};

#endif // DENTALIMAGEVIEW_H
