#ifndef FILTERBUTTONFORM_H
#define FILTERBUTTONFORM_H

#include <QWidget>
#include <QValidator>

namespace Ui {
class FilterButtonForm;
}

class FilterButtonForm : public QWidget
{
    Q_OBJECT

public:
    explicit FilterButtonForm(QWidget *parent = nullptr);
    ~FilterButtonForm();
    void setTitle(QString title);
    QString getTitle();
    void exit();

    void cephReadSettings();
    void panoReadSettings();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void sendPanoMedian(int);
    void sendCephMedian(int);
    void panoLowPassCutOff(int);
    void cephLowPassCutOff(int);
    void panoHighPassCutOff(int);
    void cephHighPassCutOff(int);

private slots:
    void cephWriteSettings();
    void panoWriteSettings();

    void on_okPushButton_clicked();

    void on_sortComboBox_currentIndexChanged(int index);

private:
    Ui::FilterButtonForm *ui;
    QValidator* validator;
};

#endif // FILTERBUTTONFORM_H
