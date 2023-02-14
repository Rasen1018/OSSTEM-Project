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
class ListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ListWidget(int id, QString name, QString birthdate,
                        QString directory, QString directory2, QString type,
                        QString gender, QString path,QWidget *parent = nullptr);
    ~ListWidget();

    QString getName() ;
    void setName(QString&);

    QString getBirthdate();
    void setBirthdate(QString&);

    QString getDirectory();
    void setDirectory(QString&);

    QString getDirectory2();
    void setDirectory2(QString&);

    QString getType();
    void setType(QString&);

    QString getGender();
    void setGender(QString&);


    QString getImage();
    void setImage(QPixmap&);

    int id();

signals:
    void setLoadMainWindow(QString, QString, QString);

private slots:
    void on_loadPushButton_clicked();
    void delMainWindow();

private:
    bool flag = false;

    Ui::ListWidget *ui;

    MainWindow* unsignedViewer;

    QImage defaultImg;
    QPixmap defaultPixmap, prevPixmap;

    qint64 loadSize;
    qint64 byteToWrite;
    qint64 totalSize;
    QByteArray outBlock;
    QFile* cephFile;
    QFile* panoFile;
};

#endif // LISTWIDGET_H
