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

    type = new QButtonGroup(this);
    type->addButton(ui->panoramaRadioButton);
    type->addButton(ui->cephRadioButton);

    ui->panoramaRadioButton->setChecked(true);

    gender = new QButtonGroup(this);
    gender->addButton(ui->maleRadioButton);
    gender->addButton(ui->femaleRadioButton);

    ui->maleRadioButton->setChecked(true);

    loadData();
}
XrayDB::~XrayDB()
{
    delete ui;

    QSqlDatabase db = patientModel->database();
    if(db.isOpen()) {
        patientModel->submitAll();   // 데이터 베이스 닫기전 데이터 업데이트
        db.close();
        db.removeDatabase("QSQLITE");
    }
    qDebug() << "Success Close Program" ;
}
int XrayDB::makeId()
{
    if(dbList.size() == 0) {
        return 1;
    }

    else {
        auto id = dbList.lastKey();
        return ++id;
    }
}
void XrayDB::loadData()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "patient connection");
    db.setDatabaseName("patient.db");
    if (db.open( )) {
        QSqlQuery query(db);        //bug 수정 -> query()안에 db를 넣어줘야함
        query.exec("CREATE TABLE IF NOT EXISTS patient(id INTEGER Primary Key NOT NULL, name VARCHAR(30),"
                   "birthdate VARCHAR(20), directory VARCHAR(50), "
                   "directory2 VARCHAR(50), type VARCHAR(50), gender VARCHAR(50),"
                   "path VARCHAR(50));");

        patientModel = new QSqlTableModel(this, db);
        patientModel->setTable("patient");
        patientModel->select();
    }
    for(int i = 0; i < patientModel->rowCount(); i++) {
        int id = patientModel->data(patientModel->index(i, 0)).toInt();
        QString name = patientModel->data(patientModel->index(i, 1)).toString();
        QString birthdate = patientModel->data(patientModel->index(i, 2)).toString();
        QString directory = patientModel->data(patientModel->index(i, 3)).toString();
        QString directory2 = patientModel->data(patientModel->index(i, 4)).toString();
        QString type = patientModel->data(patientModel->index(i, 5)).toString();
        QString gender = patientModel->data(patientModel->index(i, 6)).toString();
        QString path = patientModel->data(patientModel->index(i, 7)).toString();


        ListWidget* L = new ListWidget(id, name, birthdate, directory, directory2, type, gender, path);

        dbList.insert(id,L);    // QMap에 추가

        QListWidgetItem *item = new QListWidgetItem;
        ui->personList->addItem(item);
        item->setSizeHint(QSize(380,150));
        ui->personList->setItemWidget(item, L);

    }
}

void XrayDB::on_addPushButton_clicked()
{
    QString name, birthdate, directory, directory2, type, gender, path;

    directory = ui->directoryLineEdit->text();

    directory2 = ui->directoryLineEdit_2->text();

    int id = makeId();

    QPixmap image;

    name = ui->nameLineEdit->text();
    birthdate = ui->birthDateLineEdit->text();

    if( ui->panoramaRadioButton->isChecked() )
    {
        type = "Pano";
    }
    else if( ui->cephRadioButton->isChecked() )
    {
        type = "Ceph";
    }

    if(ui->maleRadioButton->isChecked() )
    {
        gender = "Male";

        path = ":/images/male.png";

        image = QPixmap(path);

    }
    else if(ui->femaleRadioButton->isChecked() )
    {
        gender = "Female";

        path = ":/images/female.png";

        image = QPixmap(path);
    }

    ui->nameLineEdit->setText(name);
    ui->birthDateLineEdit->setText(birthdate);
    ui->directoryLineEdit->setText(directory);
    ui->directoryLineEdit_2->setText(directory2);

    if ((ui->nameLineEdit->text().length() && ui->birthDateLineEdit->text().length()
         && ui->directoryLineEdit->text().length())
            ||
            (ui->nameLineEdit->text().length() && ui->birthDateLineEdit->text().length()
             && ui->directoryLineEdit_2->text().length())
            ||
            (ui->nameLineEdit->text().length() && ui->birthDateLineEdit->text().length()
             && ui->directoryLineEdit->text().length() && ui->directoryLineEdit_2->text().length())
            )
    {
        QListWidgetItem* item= new QListWidgetItem;
        item->setSizeHint(QSize(380,150));

        ListWidget* L ;  L = new ListWidget(id, name, birthdate, directory, directory2, type, gender, path);

        ui->personList->addItem(item);

        ui->personList->setItemWidget(item, L);

        dbList.insert(id, L);


        if(name.length()) {
            QSqlQuery query(patientModel->database());
            query.prepare("INSERT INTO patient VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
            query.bindValue(0, id);
            query.bindValue(1, name);
            query.bindValue(2, birthdate);
            query.bindValue(3, directory);
            query.bindValue(4, directory2);
            query.bindValue(5, type);
            query.bindValue(6, gender);
            query.bindValue(7, path);
            query.exec();
            patientModel->select();

        }

    }
    else {
        QMessageBox::warning(this, "알림", "환자 정보를 입력해 주세요");
    }

    ui->nameLineEdit->clear();
    ui->birthDateLineEdit->clear();
    ui->directoryLineEdit->clear();
    ui->directoryLineEdit_2->clear();

    patientModel->submitAll();
}
void XrayDB::on_searchPushButton_clicked()
{

    QString name, birthdate, directory, directory2, type, gender, path;

    QString index;

    QPixmap image;

    int id;

    ui->searchWidget->clear();

    index =  ui->searchLineEdit->text();

    id = index.toInt();

    auto Val = dbList.find( id );

    if( Val == dbList.end() )
    {
        QMessageBox::warning(this,"알림", "환자 정보가 존재하지 않습니다.");
    }
    else{
        name = Val.value()->getName();
        birthdate = Val.value()->getBirthdate();
        directory = Val.value()->getDirectory();
        directory2 = Val.value()->getDirectory2();
        type = Val.value()->getType();
        path = Val.value()->getImage();
        gender = Val.value()->getGender();
        image = QPixmap(path);

        ListWidget *li = new ListWidget(id, name, birthdate, directory, directory2, type, gender, path);

        QListWidgetItem* item= new QListWidgetItem;

        item->setSizeHint(QSize(380,150));
        ui->searchWidget->addItem(item);
        ui->searchWidget->setItemWidget(item, li);
        ui->searchWidget->addItem(item);
        ui->searchWidget->setItemWidget(item, li);
    }

}
void XrayDB::on_modifyPushButton_clicked()
{

    // QListWidgetItem *item, ListWidget *li ;

    QListWidgetItem* item =  ui->personList->currentItem();

    if (item != nullptr)
    {
        ListWidget* li = (ListWidget*)ui->personList->itemWidget(item);

        QString name, birthdate, directory, directory2, type, gender;

        QString path;

        QPixmap image;

        name = ui->nameLineEdit->text();
        birthdate = ui->birthDateLineEdit->text();
        directory = ui->directoryLineEdit->text();
        directory2 = ui->directoryLineEdit_2->text();

        ui->nameLineEdit->setText(name);
        ui->birthDateLineEdit->setText(birthdate);
        ui->directoryLineEdit->setText(directory);
        ui->directoryLineEdit_2->setText(directory2);

        if( ui->panoramaRadioButton->isChecked() )
        {
            type = "Pano";
        }
        else if( ui->cephRadioButton->isChecked() )
        {
            type = "Ceph";
        }
        if(ui->maleRadioButton->isChecked() )
        {
            gender = "Male";
            path = ":/images/male.png";
            image = QPixmap(path);
        }
        else if(ui->femaleRadioButton->isChecked() )
        {
            gender = "Female";
            path = ":/images/female.png";
            image = QPixmap(path);
        }

        if (name.length())
        {

            item->setSizeHint(QSize(380,150));
            li->setName(name);
            li->setBirthdate(birthdate);
            li->setDirectory(directory);
            li->setDirectory2(directory2);
            li->setImage(image);
            li->setType(type);
            li->setGender(gender);

            int id = li->id();


            QSqlQuery query(patientModel->database());
            query.prepare("UPDATE patient SET name = ?, birthdate = ? , directory = ?, "
                          "directory2 = ?, type = ?, gender = ?, path = ? "
                          "WHERE id = ? ");

            query.bindValue(0, name);
            query.bindValue(1, birthdate);
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
void XrayDB::on_personList_itemClicked(QListWidgetItem *item)
{
    item =  ui->personList->currentItem();

    if (item != nullptr)
    {
        ListWidget* li = (ListWidget*)ui->personList->itemWidget(item);
        QString name, birthdate, directory, directory2, type, gender;

        QPixmap image;

        name = li->getName();

        birthdate = li->getBirthdate();

        directory = li->getDirectory();

        directory2 = li->getDirectory2();

        image = li->getImage();

        ui->nameLineEdit->setText(name);

        ui->birthDateLineEdit->setText(birthdate);

        ui->directoryLineEdit->setText(directory);

        ui->directoryLineEdit_2->setText(directory2);

        if( li->getGender() == "Female")
        {
            ui->femaleRadioButton->setChecked(true);
        }
        else if ( li->getGender() == "Male")
        {
            ui->maleRadioButton->setChecked(true);
        }

        if(  li->getType() == "Ceph"  )
        {
            ui->cephRadioButton->setChecked(true);
        }
        else  if ( li->getType() == "Pano")
        {
            ui->panoramaRadioButton->setChecked(true);
        }
    }
}
void XrayDB::on_deletePushButton_clicked()
{
    int id;
    QString name, birthdate, directory, directory2, type, gender, path;

    QListWidgetItem* item =  ui->personList->currentItem();
    ListWidget *L = (ListWidget*)ui->personList->itemWidget(item);

    QModelIndex index = ui->personList->currentIndex();

    if(index.isValid())
    {
        patientModel->removeRow(index.row());
        patientModel->select();
    }

    if (item != nullptr)
    {
        delete item;
        id = L->id();
        dbList.remove(id);
    }
    update();
    patientModel->submitAll();
}

void XrayDB::on_filePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);

    ui->directoryLineEdit->setText(filename);
}


void XrayDB::on_filePushButton2_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this);

    ui->directoryLineEdit_2->setText(filename);
}

