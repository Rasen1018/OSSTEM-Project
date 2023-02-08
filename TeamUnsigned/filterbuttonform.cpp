#include "filterbuttonform.h"
#include "ui_filterbuttonform.h"

#include <QString>
#include <QValidator>

typedef quint8 ubyte8;

FilterButtonForm::FilterButtonForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterButtonForm)
{
    ui->setupUi(this);
    ui->valueLineEdit->setEnabled(false);
    validator = new QIntValidator(1, 256, this);
    ui->valueLineEdit->setValidator(validator);
}


FilterButtonForm::~FilterButtonForm()
{
    delete ui;
    delete validator;
}


void FilterButtonForm::setTitle(QString title) {
    ui->titleLabel->setText(title);
}

QString FilterButtonForm::getTitle() {
    return ui->titleLabel->text();
}

void FilterButtonForm::exit() {
    this->close();
}

void FilterButtonForm::on_okPushButton_clicked()
{
    int idx = ui->sortComboBox->currentIndex();
    QString name = ui->titleLabel->text();

    if (idx == 1) {
        if (name == "Panorama")
            emit sendPanoMedian(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit sendCephMedian(ui->valueLineEdit->text().toInt());
    }
    else if (idx == 2) {
        if(ui->valueLineEdit->text().toInt() > 256) return;
        if (name == "Panorama")
            emit panoLowPassCutOff(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit cephLowPassCutOff(ui->valueLineEdit->text().toInt());
    }
    else if (idx == 3) {
        if(ui->valueLineEdit->text().toInt() > 256) return;
        if (name == "Panorama")
            emit panoHighPassCutOff(ui->valueLineEdit->text().toInt());
        else if (name == "Cephalo")
            emit cephHighPassCutOff(ui->valueLineEdit->text().toInt());
    }

    this->close();
}


void FilterButtonForm::on_sortComboBox_currentIndexChanged(int index)
{
    switch(index) {
    case 1:
        ui->valueLineEdit->setEnabled(false);
        ui->valueLineEdit->setText(0);
        break;
    case 2:
        ui->valueLineEdit->setEnabled(true);
        break;
    case 3:
        ui->valueLineEdit->setEnabled(true);
        break;
    default:
        ui->valueLineEdit->setEnabled(false);
        ui->valueLineEdit->clear();
        break;
    }
}

