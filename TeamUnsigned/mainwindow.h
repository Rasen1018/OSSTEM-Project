#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CephaloForm;
class PanoramaForm;
class PanoValueAdjustment;
class CephValueAdjustment;
class PanoPreset;
class CephPreset;
/*
 * panorama, cephalo  영상 후처리를 담당하는 Main Form
 * @exception panorama 영상의 사이즈 : 3000x1628
 * @exception cephalo 영상의 사이즈 : 3000x 2400
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    PanoramaForm *panoramaForm;    // panorama form 객체
    CephaloForm *cephaloForm;      // cephalo form  객체
    PanoValueAdjustment* panoValueAdjustment;   // panorama 연산 클래스 객체
    CephValueAdjustment* cephValueAdjustment;   // cephalo 연산 클래스 객체
    PanoPreset* panoPreset;        // panorama 프리셋 연산 클래스 객체
    CephPreset* cephPreset;        // cephalo 프리셋 연산 클래스 객체

protected:
    void closeEvent(QCloseEvent *event) override;   // 메인 Form 종료를 알리는 closeEvent 재구현 함수
private:
    Ui::MainWindow *ui;

signals:
    void loadPanoDB(QString);   //panorama DB load 시그널
    void loadCephDB(QString);   //cephalo DB load 시그널
    void closeMainWindow();     // 메인 Form 종료 시그널

private slots:
    void on_panoToolButton_clicked();   // panorama 툴버튼 클릭 시, panoramaForm load 및 ui 설정
    void on_cephToolButton_clicked();   // cephalo 툴버튼 클릭 시, cephaloForm load 및 ui 설정

    void setReceiveMainWindow(QString, QString, QString);   // DB에서 load 시 type 에 맞는 Form load하는 슬롯.


};
#endif // MAINWINDOW_H
