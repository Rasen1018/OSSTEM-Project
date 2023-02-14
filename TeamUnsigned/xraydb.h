#ifndef XRAYDB_H
#define XRAYDB_H

#include <QGroupBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QStandardItemModel>
#include <QFile>
#include <QImage>
#include <QModelIndex>
#include <QListWidgetItem>
class QMenu;
class QTreeWidgetItem;
class QSqlDatabase;
class QSqlTableModel;
class QMenu;

QT_BEGIN_NAMESPACE
namespace Ui { class XrayDB; }
QT_END_NAMESPACE

class ListWidget;

class XrayDB : public QWidget
{
    Q_OBJECT

public:

    XrayDB(QWidget *parent = nullptr);
    ~XrayDB();

    QListWidgetItem* item;
    void loadData();

private slots:
    void on_searchPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_personList_itemClicked(QListWidgetItem *item);
    void on_deletePushButton_clicked();

    void on_filePushButton_clicked();
    void on_filePushButton2_clicked();

    void on_addPushButton_clicked();

private:
    int makeId();
    QButtonGroup *type;
    QButtonGroup *gender;
    QPixmap *image;
    ListWidget *listWidget;

signals:
    void listadd(QString, QString, QString);

private:
    Ui::XrayDB *ui;
    QSqlTableModel* imageModel;
    qint64 loadSize;
    qint64 byteToWrite;
    qint64 totalSize;
    QByteArray outBlock;
    QFile* file;

    QMap<int, ListWidget*> dbList;
    QSqlTableModel* patientModel;
};
#endif // XRAYDB_H
