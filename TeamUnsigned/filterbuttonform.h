#ifndef FILTERBUTTONFORM_H
#define FILTERBUTTONFORM_H

#include <QWidget>
#include <QValidator>

namespace Ui {
class FilterButtonForm;
}
/* cephalo, panorama Form 의 filterButton 클래스 */
class FilterButtonForm : public QWidget
{
    Q_OBJECT

public:
    explicit FilterButtonForm(QWidget *parent = nullptr);
    ~FilterButtonForm();

    void setTitle(QString title);   //filterButton 폼의 Title setter
    QString getTitle();             //filterButton 폼의 Title 반환 함수


protected:
    void closeEvent(QCloseEvent *event);    //창 닫기 시, 소멸자 호출 함수
private:
    Ui::FilterButtonForm *ui;
    QValidator* validator;      //입력갑 범위 지정 QVlidator 객체

signals:
    /* cephalo, panorama Form 으로 median Filter 시그널 전송 */
    void sendPanoMedian(int);
    void sendCephMedian(int);
    /* cephalo, panorama Form 으로 low-pass Filter 시그널 전송 */
    void panoLowPassCutOff(int);
    void cephLowPassCutOff(int);
    /* cephalo, panorama Form 으로 high-pass Filter 시그널 전송 */
    void panoHighPassCutOff(int);
    void cephHighPassCutOff(int);

private slots:
    void on_okPushButton_clicked();                         //ComboBox Index에 맞는 파라미터 값 시그널 전송 슬롯
    void on_sortComboBox_currentIndexChanged(int index);    //ComboBox Index에 따른 ui 설정



};

#endif // FILTERBUTTONFORM_H
