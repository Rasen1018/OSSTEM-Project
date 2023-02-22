#include "filterbuttonform.h"
#include "ui_filterbuttonform.h"

#include <QString>
#include <QValidator>
#include <QSettings>

typedef quint8 ubyte8;
/* filterButton 클래스의 생성자
* 초기 설정
*/
FilterButtonForm::FilterButtonForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterButtonForm)
{

    ui->setupUi(this);
    ui->valueLineEdit->setEnabled(false);
    /* 8bit 연산진행하므로 최소 ~ 최대값 설정 */
    validator = new QIntValidator(1, 256, this);
    ui->valueLineEdit->setValidator(validator);

    this->setWindowTitle("Filter");

}
/* filterButton 클래스의 소멸자
* filterButton 종료 시 메모리 제거
*/
FilterButtonForm::~FilterButtonForm()
{
    delete ui;
    delete validator;
}
/* filterButton 폼의 Title setter 함수
* @param title
*/
void FilterButtonForm::setTitle(QString title) {
    ui->titleLabel->setText(title);
}
/* filterButton 폼의 Title 을 반환하는 함수 */
QString FilterButtonForm::getTitle() {
    return ui->titleLabel->text();
}
/* 창 닫기 시, 소멸자 호출 함수 */
void FilterButtonForm::closeEvent(QCloseEvent *event){
    Q_UNUSED(event);
    FilterButtonForm::~FilterButtonForm();
}


/* ComboBox Index에 맞는 파라미터 값 시그널 전송 슬롯 */
void FilterButtonForm::on_okPushButton_clicked()
{
    int idx = ui->sortComboBox->currentIndex();
    QString name = ui->titleLabel->text();
    /* ceph, pano Form 으로 median Fiter 시그널 전송 */
    if (idx == 1) {
        if (name == "Panorama")
            emit sendPanoMedian(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit sendCephMedian(ui->valueLineEdit->text().toInt());
    }
    /* ceph, pano Form 으로 low-pass Fiter 시그널, 파라미터 값 전송 */
    else if (idx == 2) {
        if(ui->valueLineEdit->text().toInt() > 256 || ui->valueLineEdit->text() == "") return;
        if (name == "Panorama")
            emit panoLowPassCutOff(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit cephLowPassCutOff(ui->valueLineEdit->text().toInt());
    }
    /* ceph, pano Form 으로 high-pass Fiter 시그널, 파라미터 값 전송 */
    else if (idx == 3) {
        if(ui->valueLineEdit->text().toInt() > 256 || ui->valueLineEdit->text() == "") return;
        if (name == "Panorama")
            emit panoHighPassCutOff(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit cephHighPassCutOff(ui->valueLineEdit->text().toInt());
    }

    this->close();
}
/* ComboBox Index에 따른 ui 설정  */
void FilterButtonForm::on_sortComboBox_currentIndexChanged(int index)
{
    switch(index) {
    case 1:
        ui->valueLineEdit->setEnabled(false);
        ui->valueLineEdit->setText(0);
        ui->valueLineEdit->setPlaceholderText("Click OK Button");
        break;
    case 2:
        ui->valueLineEdit->setEnabled(true);
        ui->valueLineEdit->setPlaceholderText("Input Cut-Off freq");
        break;
    case 3:
        ui->valueLineEdit->setEnabled(true);
        ui->valueLineEdit->setPlaceholderText("Input Cut-Off freq");
        break;
    default:
        ui->valueLineEdit->setEnabled(false);
        ui->valueLineEdit->setPlaceholderText("Choose Filter");
        ui->valueLineEdit->clear();
        break;
    }
}

