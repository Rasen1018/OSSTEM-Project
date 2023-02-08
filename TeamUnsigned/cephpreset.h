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
    void cephPresetSend(QPixmap&);
    void cephPresetAdj(QPixmap&);

private:
    QImage defaultImg, image;
    QImage presetImg;

    QPixmap pixmap;
    quint8 * inimg, *mask, *outimg ,*copyImg, *copyImg2;


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

    void set3x3MaskValue();

    quint8* gaussian(float);
    quint8* highBoost(int);
    quint8* highBoost(quint8*, int);
    quint8* ADFilter(quint8* ,int);

};

#endif // CEPHPRESET_H
