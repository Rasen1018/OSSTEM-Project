#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

/*
 * Histogram 분포를 구하고, 그리는 클래스
 */
class Histogram : public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);

private:
    void setHistoChart();           //Histogram 분포를 그리는 함수

    const uchar* inimg;             //Histogram 분포를 구하기 위한 영상의 메모리
    unsigned char value;

    int histo[256];                 //Histogram 분포 배열
    int width = 0, height = 0, imageSize = 0;

    int min = 999, max = 0;
    int hstMin = 999, hstMax = 0;


private slots:
    void receiveHisto(QPixmap&);    //Histogram 분포를 구하기 위한 슬롯

};

#endif // HISTOGRAM_H
