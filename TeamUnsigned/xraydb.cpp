#include "xraydb.h"
#include "ui_xraydb.h"
#include "listwidget.h"

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QPixmap>
#include <QImage>
#include <QLabel>
#include <QMessageBox>
#include <QListWidget>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>

/* DB에 입력된 환자정보를 관리하는 XrayDB 클래스의 생성자
 * ui 설정 및 loadData 함수 호출
*/
XrayDB::XrayDB(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::XrayDB)
{
    ui->setupUi(this);

    QPixmap pixmap(":/images/folder.png");
    QIcon ButtonIcon(pixmap);

    ui->filePushButton->setIconSize(QSize(ui->filePushButton->width()*0.7,ui->filePushButton->height()*0.7));
    ui->filePushButton2->setIconSize(QSize(ui->filePushButton->width()*0.7,ui->filePushButton->height()*0.7));

    ui->filePushButton->setIcon(ButtonIcon);
    ui->filePushButton2->setIcon(ButtonIcon);

    /* type을 관리하는 ButtonGroup */
    type = new QButtonGroup(this);
    type->addButton(ui->panoramaRadioButton);
    type->addButton(ui->cephRadioButton);
    ui->panoramaRadioButton->setChecked(true);

    /* gender를 관리하는 ButtonGroup */
    gender = new QButtonGroup(this);
    gender->addButton(ui->maleRadioButton);
    gender->addButton(ui->femaleRadioButton);
    ui->maleRadioButton->setChecked(true);

    loadData();
}
/* DB에 입력된 환자정보를 관리하는 XrayDB 클래스의 소멸자
 * 메모리 해제 및 DB 종료
*/
XrayDB::~XrayDB()
{
    delete ui;

    QSqlDatabase db = patientModel->database();
    if(db.isOpen()) {
        patientModel->submitAll();   // DB 정보 Update
        db.close();
        db.removeDatabase("QSQLITE");
    }
    qDebug() << "Success Close Program" ;
}
/* 환자 ID 생성 함수 */
int XrayDB::makeId()
{
    /* 중복값 방지를 위한 if~else */
    if(dbList.size() == 0) {
        return 1;
    }
    else {
        auto id = dbList.lastKey();
        return ++id;
    }
}
/* DB의 데이터 load 함수, DB의 데이터를 ListWidget에 추가 */
void XrayDB::loadData()
{
    /* patient DB 생성 및 테이블 설정 */
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "patient connection");
    db.setDatabaseName("patient.db");
    if (db.open( )) {
        QSqlQuery query(db);        //bug 수정 -> query()안에 db를 넣어줘야함
        query.exec("CREATE TABLE IF NOT EXISTS patient(id INTEGER Primary Key NOT NULL, name VARCHAR(30),"
                   "film VARCHAR(20), directory VARCHAR(50), "
                   "directory2 VARCHAR(50), type VARCHAR(50), gender VARCHAR(50),"
                   "path VARCHAR(50));");

        patientModel = new QSqlTableModel(this, db);
        patientModel->setTable("patient");
        patientModel->select();
    }
    for(int i = 0; i < patientModel->rowCount(); i++) {
        int id = patientModel->data(patientModel->index(i, 0)).toInt();
        QString name = patientModel->data(patientModel->index(i, 1)).toString();
        QString film = patientModel->data(patientModel->index(i, 2)).toString();
        QString directory = patientModel->data(patientModel->index(i, 3)).toString();
        QString directory2 = patientModel->data(patientModel->index(i, 4)).toString();
        QString type = patientModel->data(patientModel->index(i, 5)).toString();
        QString gender = patientModel->data(patientModel->index(i, 6)).toString();
        QString path = patientModel->data(patientModel->index(i, 7)).toString();

        /* ListWidget의 정보를 QMap에 추가 */
        ListWidget* L = new ListWidget(id, name, film, directory, directory2, type, gender, path);
        dbList.insert(id,L);    // QMap에 추가

        /* DB정보로 ListWidgetItem 추가 */
        QListWidgetItem *item = new QListWidgetItem;
        ui->personList->addItem(item);
        item->setSizeHint(QSize(380,150));
        ui->personList->setItemWidget(item, L);
    }
}
/* 정보를 추가 시 QMap과 DB에 추가하는 슬롯 */
void XrayDB::on_addPushButton_clicked()
{
    QString name, film, directory, directory2, type, gender, path;

    /* ceph, pano의 path 저장 */
    directory = ui->directoryLineEdit->text();
    directory2 = ui->directoryLineEdit_2->text();

    int id = makeId();

    QPixmap image;

    name = ui->nameLineEdit->text();
    film = ui->filmDateLineEdit->text();

    /* Button Group의 type */
    if( ui->panoramaRadioButton->isChecked() ){
        type = "Pano";
    }
    else if( ui->cephRadioButton->isChecked() ){
        type = "Ceph";
    }
    /* Button Group의 gender에 따른 Icon set */
    if(ui->maleRadioButton->isChecked() ) {
        gender = "Male";
        path = ":/images/male.png";

        image = QPixmap(path);

    }
    else if(ui->femaleRadioButton->isChecked() ){
        gender = "Female";
        path = ":/images/female.png";

        image = QPixmap(path);
    }

    ui->nameLineEdit->setText(name);
    ui->filmDateLineEdit->setText(film);
    ui->directoryLineEdit->setText(directory);
    ui->directoryLineEdit_2->setText(directory2);

    /* 모든 정보가 입력될 경우 ListWidgetItem 생성 */
    if ((ui->nameLineEdit->text().length() && ui->filmDateLineEdit->text().length()
         && ui->directoryLineEdit->text().length())
            ||
            (ui->nameLineEdit->text().length() && ui->filmDateLineEdit->text().length()
             && ui->directoryLineEdit_2->text().length())
            ||
            (ui->nameLineEdit->text().length() && ui->filmDateLineEdit->text().length()
             && ui->directoryLineEdit->text().length() && ui->directoryLineEdit_2->text().length())){
        QListWidgetItem* item= new QListWidgetItem;
        item->setSizeHint(QSize(380,150));

        ListWidget* L ;  L = new ListWidget(id, name, film, directory, directory2, type, gender, path);
        ui->personList->addItem(item);
        ui->personList->setItemWidget(item, L);

        dbList.insert(id, L);

        /* 추가한 정보 DB에 Insert */
        if(name.length()) {
            QSqlQuery query(patientModel->database());
            query.prepare("INSERT INTO patient VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
            query.bindValue(0, id);
            query.bindValue(1, name);
            query.bindValue(2, film);
            query.bindValue(3, directory);
            query.bindValue(4, directory2);
            query.bindValue(5, type);
            query.bindValue(6, gender);
            query.bindValue(7, path);
            query.exec();
            patientModel->select();

        }

    }
    else {      //입력되지 않았을 경우 예외 처리
        QMessageBox::warning(this, "알림", "환자 정보를 입력해 주세요");
    }

    ui->nameLineEdit->clear();
    ui->filmDateLineEdit->clear();
    ui->directoryLineEdit->clear();
    ui->directoryLineEdit_2->clear();

    patientModel->submitAll();  //DB update
}
/* 환자 정보 검색 슬롯 */
void XrayDB::on_searchPushButton_clicked()
{
    QString name, film, directory, directory2, type, gender, path;
    QString index;
    QPixmap image;

    int id;

    ui->searchWidget->clear();
    index =  ui->searchLineEdit->text();
    id = index.toInt();

    /* DB에 저장된 id 검색 */
    auto Val = dbList.find( id );
    if( Val == dbList.end() )
    {
        QMessageBox::warning(this,"알림", "환자 정보가 존재하지 않습니다.");
    }
    else{
        name = Val.value()->getName();
        film = Val.value()->getFilmDate();
        directory = Val.value()->getDirectory();
        directory2 = Val.value()->getDirectory2();
        type = Val.value()->getType();
        path = Val.value()->getImage();
        gender = Val.value()->getGender();
        image = QPixmap(path);

        ListWidget *li = new ListWidget(id, name, film, directory, directory2, type, gender, path);
        QListWidgetItem* item= new QListWidgetItem;

        item->setSizeHint(QSize(380,150));
        ui->searchWidget->addItem(item);
        ui->searchWidget->setItemWidget(item, li);
        ui->searchWidget->addItem(item);
        ui->searchWidget->setItemWidget(item, li);
    }
}
/* 환자 정보 변경 슬롯 */
void XrayDB::on_modifyPushButton_clicked()
{
    /* 선택한 ListWidget의 index */
    QListWidgetItem* item =  ui->personList->currentItem();

    if (item != nullptr)    //예외 처리
    {
        ListWidget* li = (ListWidget*)ui->personList->itemWidget(item);

        QString name, film, directory, directory2, type, gender;
        QString path;
        QPixmap image;

        /* 변경 정보를 ui에 반영 */
        name = ui->nameLineEdit->text();
        film = ui->filmDateLineEdit->text();
        directory = ui->directoryLineEdit->text();
        directory2 = ui->directoryLineEdit_2->text();
        ui->nameLineEdit->setText(name);
        ui->filmDateLineEdit->setText(film);
        ui->directoryLineEdit->setText(directory);
        ui->directoryLineEdit_2->setText(directory2);
        if( ui->panoramaRadioButton->isChecked() ){
            type = "Pano";
        }
        else if( ui->cephRadioButton->isChecked() ){
            type = "Ceph";
        }
        if(ui->maleRadioButton->isChecked() ){
            gender = "Male";
            path = ":/images/male.png";
            image = QPixmap(path);
        }
        else if(ui->femaleRadioButton->isChecked() ){
            gender = "Female";
            path = ":/images/female.png";
            image = QPixmap(path);
        }

        /* 변경 정보를 DB에 UPDATE~SET */
        if (name.length()){
            item->setSizeHint(QSize(380,150));
            li->setName(name);
            li->setFilmdate(film);
            li->setDirectory(directory);
            li->setDirectory2(directory2);
            li->setImage(image);
            li->setType(type);
            li->setGender(gender);

            int id = li->id();

            QSqlQuery query(patientModel->database());
            query.prepare("UPDATE patient SET name = ?, film = ? , directory = ?, "
                          "directory2 = ?, type = ?, gender = ?, path = ? "
                          "WHERE id = ? ");

            query.bindValue(0, name);
            query.bindValue(1, film);
            query.bindValue(2, directory);
            query.bindValue(3, directory2);
            query.bindValue(4, type);
            query.bindValue(5, gender);
            query.bindValue(6, path);
            query.bindValue(7, id);
            query.exec();
            qDebug() << query.lastError();
            patientModel->select();
            patientModel->submitAll();

        }
        else {
            QMessageBox::warning(this, "알림", "환자 정보를 입력해 주세요");
        }
    }
}
/* ListWidgetItem 클릭시 해당 item을 ui에 출력하는 슬롯 */
void XrayDB::on_personList_itemClicked(QListWidgetItem *item)
{
    item =  ui->personList->currentItem();

    if (item != nullptr)
    {
        ListWidget* li = (ListWidget*)ui->personList->itemWidget(item);
        QString name, film, directory, directory2, type, gender;
        /* ui 설정 */
        QPixmap image;
        name = li->getName();
        film = li->getFilmDate();
        directory = li->getDirectory();
        directory2 = li->getDirectory2();
        image = li->getImage();
        ui->nameLineEdit->setText(name);
        ui->filmDateLineEdit->setText(film);
        ui->directoryLineEdit->setText(directory);
        ui->directoryLineEdit_2->setText(directory2);
        if( li->getGender() == "Female"){
            ui->femaleRadioButton->setChecked(true);
        }
        else if ( li->getGender() == "Male"){
            ui->maleRadioButton->setChecked(true);
        }
        if(  li->getType() == "Ceph"  ){
            ui->cephRadioButton->setChecked(true);
        }
        else  if ( li->getType() == "Pano"){
            ui->panoramaRadioButton->setChecked(true);
        }
    }
}
/* 환자 정보 삭제 슬롯 */
void XrayDB::on_deletePushButton_clicked()
{
    int id;
    QString name, film, directory, directory2, type, gender, path;

    QListWidgetItem* item =  ui->personList->currentItem();
    ListWidget *L = (ListWidget*)ui->personList->itemWidget(item);

    QModelIndex index = ui->personList->currentIndex();

    if(index.isValid()){    //데이터가 비어있는 경우예외 처리
        patientModel->removeRow(index.row());
        patientModel->select();
    }
    if (item != nullptr){   // ListWidget에서 item을 삭제
        delete item;
        id = L->id();
        dbList.remove(id);
    }
    update();
    patientModel->submitAll();  //DB Update
}
/* Cephalo File path 찾는 슬롯 */
void XrayDB::on_filePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file", "C:\\");

    ui->directoryLineEdit->setText(filename);
}
/* Panorama File path 받아오는 슬롯 */
void XrayDB::on_filePushButton2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file", "C:\\");

    ui->directoryLineEdit_2->setText(filename);
}

