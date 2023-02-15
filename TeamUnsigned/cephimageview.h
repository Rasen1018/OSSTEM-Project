#ifndef CEPHIMAGEVIEW_H
#define CEPHIMAGEVIEW_H

#include "histogram.h"

#include <QWidget>

#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
/*
 * cephalo 이미지의 보여지는 부분을 담당하는
 * CephImageView 클래스
 * 프로그램에서 영상이 보이는 부분은 View 클래스의 동작.
 */
class CephImageView : public QWidget
{
    Q_OBJECT

public:
    explicit CephImageView(QWidget *parent = nullptr);
private:

    void updateStyleSheet();                //StyleSheet 설정 함수
    void scaleImage(double);                //이미지 배율에 맞게 Label크기를 조정하는 함수

    Histogram* histogram;                   //Histogram 클래스의 객체

    QGridLayout m_layout{this};
    QScrollArea m_area;
    QLabel *m_imageLabel, m_scaleLabel;     //이미지, 배율이 표시되는 Label
    QPushButton m_zoomOut{"Zoom Out"}, m_zoomIn{"Zoom In"},
                m_zoomReset{"Zoom Reset"}, m_histo{"Histogram"};
    double m_scaleFactor = 1.0; //배율

    QImage prevImg;     //뷰에 보여지는 QImage 변수
    QPixmap viewPixmap; //뷰에 보여지는 QPixmap 변수

    int cephViewWidth = 800;        //뷰의 가로
    int cephViewHeight = 600;       //뷰의 세로

    int cephWidth = 3000;           //cephalo 원본영상의 가로
    int cephHeight = 2400;          //cephalo 원본영상의 세로

signals:
    void sendSave(QImage&);         //이미지 전송 시그널
    void sendHisto(QPixmap&);       //이미지 전송 시그널
    void sendCephPrev(QPixmap&);    //히스토 연산을 위한 시그널

private slots:
    void receiveLoadImg(QPixmap);           //cephaloForm 에서 View로 픽스맵 데이터 받아오는 슬롯
    void receiveResetCeph(QPixmap&);        //cephaloForm에서 리셋 신호 전송 시, View 리셋 슬롯
    void receiveSaveCeph();                 //cephaloForm에서 저장 신호 전송 시, 현재 View의 Img 전송 슬롯
};

#endif // CEPHIMAGEVIEW_H
