#ifndef CEPHALOFORM_H
#define CEPHALOFORM_H

#include "cephimageview.h"
#include "filterbuttonform.h"

#include <QWidget>

class QFile;
class CephImageView;

namespace Ui {
class CephaloForm;
}
/*
 * cephalo 영상 후처리를 담당하는 cephalo Form
 * @exception cephalo 영상의 사이즈 : 3000x2400
 * @exception 저장 시, 16bit unsigned raw 파일로 저장.
 */
class CephaloForm : public QWidget
{
    Q_OBJECT
public:
    explicit CephaloForm(QWidget *parent = nullptr);
    ~CephaloForm();

private:
    Ui::CephaloForm *ui;
    QFile* file;
    QImage defaultImg;                    //원본이미지를 저장하기 위한 변수
    QPixmap defaultPixmap, prevPixmap;    //원본이미지와 연산 결과 저장하는 변수

    CephImageView *cephImageView;         //CephImageView 객체 (cephalo View)
    FilterButtonForm *filterWidget;       //filterButtonForm 객체 선언

    int imageWidth;     //이미지 가로
    int imageHeight;    //이미지 세로
    int panoImgLabelWidth = 360;
    int panoImgLabelHeight = 287;

    /* 밝기, 대조, 선예도, 감마, deNoise 값 저장 변수 */
    int brightValue=0;
    int contrastValue=0;
    int sbValue=0;
    int deNoiseValue=0;
    int gammaValue=0;

private slots:
    void loadDB_Data(QString);          //type이 cephalo 인 DB load하는 슬롯

    void on_filePushButton_clicked();   //cephalo Form으로 이미지를 load하는 슬롯

    /* cephaloForm ui의 밝기 값을 처리하는 슬롯 */
    void on_brightSlider_valueChanged(int value);
    void on_brightMinusButton_clicked();
    void on_brightPlusButton_clicked();

    /* cephaloForm ui의 대조 값을 처리하는 슬롯 */
    void on_contrastSlider_valueChanged(int value);
    void on_contrastMinusButton_clicked();
    void on_contrastPlusButton_clicked();

    /* cephaloForm ui의 선예도를 처리하는 슬롯 */
    void on_sbSlider_valueChanged(int value);
    void on_sharpenButton_clicked();
    void on_blurButton_clicked();

    /* cephaloForm ui의 gamma 값을 처리하는 슬롯 */
    void on_gammaPlusButton_clicked();
    void on_gammaMinusButton_clicked();
    void on_gammaSlider_valueChanged(int value);

    /* cephaloForm ui의 deNoise를 처리하는 슬롯 */
    void on_deNoiseSlider_valueChanged(int value);
    void on_deNoisePlusButton_clicked();
    void on_deNoiseMinusButton_clicked();

    /* cephaloForm의 프리셋 버튼을 처리하는 슬롯*/
    void on_ceph_Preset_Button1_clicked();
    void on_ceph_Preset_Button2_clicked();
    void on_ceph_Preset_Button3_clicked();
    void on_ceph_Preset_Button4_clicked();
    void on_ceph_Preset_Button5_clicked();
    void on_ceph_Preset_Button6_clicked();

    void on_resetButton_clicked();      //cephaloForm reset 슬롯

    void receieveImg(QPixmap&);         //연산클래스에서의 영상 연산 결과를 받는 슬롯

    void on_imageSaveButton_clicked();  //저장버튼 클릭 시, 처리 슬롯
    void cephImageSave(QImage&);        // View에서 후처리한 cephalo 영상을 저장하는 슬롯

    void on_hePushButton_clicked();     //평활화 버튼 클릭 시, 처리 슬롯

    void on_exitButton_clicked();       //종료 버튼 클릭 시, 처리 슬롯

    void on_filterPushButton_clicked(); //필터 버튼 클릭 시, 처리 슬롯

    /* filter 연산을 위해 cephalo 연산 클래스로 시그널 전송하는 슬롯*/
    void sendFourierSignal(int);
    void send2FourierSignal(int);
    void sendMedianSignal(int);

    void resetFilCalcValue();           //필터 연산 후, 초기화 슬롯

signals:
    void sendCephView(QPixmap); //View로 이미지를 전송하는 시그널
    void sendCephAdj(QPixmap&); //연산 클래스로 이미지 전송 시그널

    void sendCephValue(int, int, int, int, int);  //밝기, 대조, 선예도, 감마, deNoise 값 전송 시그널

    void saveCephSignal();          //저장 신호를 View로 전송하는 시그널

    void sendCephPrev(QPixmap&);    //프리셋 이미지를 연산클래스로 전송하는 시그널
    void sendCephPreset(int);       //프리셋 연산 클래스로 전송하는 시그널

    void sendSetReset();            //cephalo 연산 클래스로 리셋 신호 전송 시그널
    void sendResetCeph(QPixmap&);   //원본이미지를 View로 전송하는 시그널
    void exitCephSignal();          //mainForm 으로 종료 신호 전송 시그널

     /* filter 연산을 위해 cephalo연산 클래스로 파라미터값 전송 시그널 */
    void sendCutOffValue(int);
    void send2CutoffValue(int);
    void sendMedianValue(int);
};

#endif // CEPHALOFORM_H
