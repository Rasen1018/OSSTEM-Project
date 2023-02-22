#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include "mainwindow.h"
#include <QWidget>
#include <QFile>

class MainWindow;

namespace Ui {
class ListWidget;
class CephImageView;
class CephaloForm;

}
/* XrayDB 클래스의 입력된 정보를 담고있는 ListWidget 클래스 */
class ListWidget : public QWidget
{
    Q_OBJECT

public:
    /*
    *  @param 환자ID
    *  @param 이름
    *  @param 생일
    *  @param cephalo file 경로
    *  @param panorama file 경로
    *  @param type
    *  @param 성별
    *  @param 성별에 따른 icon 경로
    *
   */
    explicit ListWidget(int id, QString name, QString birthdate,
                        QString directory, QString directory2, QString type,
                        QString gender, QString path,QWidget *parent = nullptr);
    ~ListWidget();
    QFile* cephFile;                    //cephalo에 대한 파일 객체
    QFile* panoFile;                    //panorama에 대한 파일 객체

    QString getName() ;              //환자의 이름 반환
    void setName(QString&);          //환자의 이름이 수정 되었다면 바뀐 값으로 set

    QString getFilmDate();          //환자의 찍은 날짜 반환
    void setFilmdate(QString&);     //환자 정보가 수정 되었다면 바뀐 값으로 set

    QString getDirectory();          //Ceph의 path 반환
    void setDirectory(QString&);     //Ceph의 경로가 바뀌었다면 바뀐 값으로 set

    QString getDirectory2();         //Pano의 path 반환
    void setDirectory2(QString&);    //Pano의 경로가 바뀌었다면 바뀐 값으로 set

    QString getType();               //Image가 Pano인지 Ceph인지 반환
    void setType(QString&);          //수정 되었을 경우 변경된 값으로 set

    QString getGender();             //gender 반환
    void setGender(QString&);        //수정 되었을 경우 변경된 값으로 set

    QString getImage();              //icon( male.png, female.png) 경로 반환
    void setImage(QPixmap&);         //gender에 따른 이미지 set

    int id();                        //id값을 반환


signals:
    void setLoadMainWindow(QString, QString, QString);  // type, Ceph 경로, Path 경로를 보냄, type에 따라 초기화면 결정

private slots:
    void on_loadPushButton_clicked();   // type(Pano, Ceph)에 따른 후처리 뷰어 load 슬롯
    void delMainWindow();               // 종료 신호 발생 시, delete MainForm 슬롯

private:
    bool flag = false;

    Ui::ListWidget *ui;

    MainWindow* unsignedViewer;         //Main Form 객체


};

#endif // LISTWIDGET_H
