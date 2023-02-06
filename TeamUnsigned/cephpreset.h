#ifndef CEPHPRESET_H
#define CEPHPRESET_H

#include <QImage>
#include <QPixmap>

class CephPreset : public QObject
{
    Q_OBJECT
public:
    explicit CephPreset(QObject *parent = nullptr);

private slots:
    void receievePreset(int);
    void receiveFile(QPixmap&);

signals:
    void panoPresetSend(QPixmap&);
    void panoPresetAdj(QPixmap&);

private:
    QImage defaultImg, image, prevImg;
    QImage presetImg;

    QPixmap pixmap;
    unsigned short * inimg, *mask, *outimg ,*sharpenImg, *copyImg;


    int width, height, imageSize;
    int cephViewWidth = 800;
    int cephViewHeight = 600;
    double avg = 0;

    void setPreset_1();
    void setPreset_2();
    void setPreset_3();
    void setPreset_4();
    void setPreset_5();
    void setPreset_6();

    void gaussian(float);
    void set3x3MaskValue();
    void highBoost(int);
    void ADFilter(unsigned short* ,int);

};

#endif // CEPHPRESET_H
