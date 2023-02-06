#ifndef CEPHIMAGEVIEW_H
#define CEPHIMAGEVIEW_H

#include "histogram.h"

#include <QWidget>

#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

class CephImageView : public QWidget
{
    Q_OBJECT


public:
    explicit CephImageView(QWidget *parent = nullptr);

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

    int cephViewWidth = 800;
    int cephViewHeight = 600;

    int cephWidth = 3000;
    int cephHeight = 2400;

    Histogram* histogram;
private slots:
    void receiveLoadImg(QPixmap);
    void receiveResetCeph(QPixmap&);
    void receiveSaveCeph();

signals:
    void send(QPixmap , QString);
    void sendSave(QImage&);
    void sendHisto(QPixmap&);
    void sendCephPrev(QPixmap&);

};

#endif // CEPHIMAGEVIEW_H
