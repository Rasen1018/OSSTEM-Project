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
class QSqlDatabase;
class QSqlTableModel;
class ListWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class XrayDB; }
QT_END_NAMESPACE

/* DB에 입력된 환자정보를 관리하는 XrayDB 클래스
 * 모든 정보는 해당 클래스에서 DB로 관리 됨.
*/
class XrayDB : public QWidget
{
    Q_OBJECT

public:
    XrayDB(QWidget *parent = nullptr);
    ~XrayDB();

    QListWidgetItem* item;      // personlist의 item 객체
    void loadData();            // DB의 데이터 load 함수, DB의 데이터를 ListWidget에 추가.

    QButtonGroup *type;         //type의 ButtonGroup 객체
    QButtonGroup *gender;       //gender의 ButtonGroup 객체
    QPixmap *image;             //load 할 icon 객체
    ListWidget *listWidget;     //ListWidget Class의 객체

    QFile* file;
    QMap<int, ListWidget*> dbList;  //DB 정보를 담고있는 QMap
    QSqlTableModel* patientModel;   //DB 정보를 담고있는 QSqlTableModel
private:
    int makeId();               //환자 ID 생성 함수

    Ui::XrayDB *ui;


private slots:
    void on_addPushButton_clicked();            //정보를 추가 시 QMap과 DB에 추가하는 슬롯
    void on_searchPushButton_clicked();         //환자 정보 검색 슬롯
    void on_modifyPushButton_clicked();         //환자 정보 변경 슬롯
    void on_personList_itemClicked(QListWidgetItem *item);  //ListWidgetItem 클릭시 해당 item을 ui에 출력하는 슬롯
    void on_deletePushButton_clicked();         //환자 정보 삭제 슬롯

    void on_filePushButton_clicked();           //Cephalo File path 받아오는 슬롯
    void on_filePushButton2_clicked();          //Panorama File path 받아오는 슬롯


};
#endif // XRAYDB_H
