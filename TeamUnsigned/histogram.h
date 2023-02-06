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


class Histogram : public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = nullptr);

private:
    QImage image;

    const uchar* inimg;
    unsigned char  *mask, *outimg, *sharpenImg;
    int histo[65536];
    int width = 0, height = 0, imageSize = 0;
    int min = 65536, max = 0;
    int hstMin = 65536, hstMax = 0;
    unsigned char gray;

    void setHistoChart();
private slots:
    void receiveHisto(QPixmap&);
signals:

};

#endif // HISTOGRAM_H
