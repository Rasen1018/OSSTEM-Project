#include "histogram.h"

Histogram::Histogram(QWidget *parent)
    : QWidget{parent}
{
}

void Histogram::setHistoChart(){

    QString legendName = "value range : " + QString::number(min) + " ~ " +QString::number(max);

    QBarSet *set0 = new QBarSet(legendName);
    set0->setColor(Qt::black);

    for(int i = min; i < max+1; i++){   // value count 채우기
        set0->append(histo[i]);
    }

    QBarSeries *series = new QBarSeries();
    series->append(set0);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Image Histogram");
    chart->setAnimationOptions(QChart::AllAnimations);

    QStringList xValue;
    for(int i = min; i < max +1; i ++){  //x축 value 설정
        xValue <<  QString::number(i);
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(xValue);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%.0f");
    axisY->setRange(hstMin, hstMax);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Customize chart background
    QLinearGradient backgroundGradient;
    backgroundGradient.setColorAt(0.0, QRgb(0xd2d0d1));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chartView->chart()->setBackgroundBrush(backgroundGradient);
    chartView->chart()->setPlotAreaBackgroundVisible(true);

    chartView->show();
    chartView->resize(300,300);

}
void Histogram::receiveHisto(QPixmap& pixmap){

    min = 999, max = 0;
    hstMin = 999, hstMax = 0;

    image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_Grayscale8);

    inimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    for(int i = 0; i < 256; i ++) {
        histo[i] =0;
    }

    for (int i = 0; i < imageSize; i++) {   //histogram 분포
        gray = inimg[i];
        histo[gray] += 1;

        if(min >= inimg[i]) min = inimg[i];
        if(max <= inimg[i]) max = inimg[i];
    }

    for(int i = 0; i < 256; i ++){
        if(hstMin >= histo[i]) hstMin = histo[i];
        if(hstMax <= histo[i]) hstMax = histo[i];
    }

    setHistoChart();
}

