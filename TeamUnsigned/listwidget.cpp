#include "listwidget.h"
#include "ui_listwidget.h"

#include "mainwindow.h"

ListWidget::ListWidget(int id, QString name, QString birthdate, QString directory,
                       QString directory2, QString type, QString gender, QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListWidget)
{
    QString i;

    QPixmap image;

    ui->setupUi(this);
    ui->nameLabel->setText(name);
    ui->birthDateLabel->setText(birthdate);
    ui->directoryLabel->setText(directory);
    ui->directoryLabel_2->setText(directory2);
    ui->typeLabel->setText(type);
    ui->pathLabel->setText(path);
    ui->genderLabel->setText(gender);

    ui->idLabel->setText(i = QString::number(id));
    image = QPixmap(path);
    ui->imageLabel->setPixmap(image);

}
ListWidget::~ListWidget()
{
    delete ui;
}

QString ListWidget::getName()
{
    QString name;
    name = ui->nameLabel->text();
    return name;
}
void ListWidget::setName(QString& name)
{
    ui->nameLabel->setText(name);
}
QString ListWidget::getBirthdate()
{
    QString birthdate;
    birthdate = ui->birthDateLabel->text();
    return birthdate;
}
void ListWidget::setBirthdate(QString& birthdate)
{
    ui->birthDateLabel->setText(birthdate);
}
QString ListWidget::getDirectory()
{
    QString directory;
    directory = ui->directoryLabel->text();
    return directory;
}
void ListWidget::setDirectory(QString& directory)
{
    ui->directoryLabel->setText(directory);
}
QString ListWidget::getDirectory2()
{
    QString directory2;
    directory2 = ui->directoryLabel_2->text();
    return directory2;
}
void ListWidget::setDirectory2(QString& directory2)
{
    ui->directoryLabel_2->setText(directory2);
}
QString ListWidget::getType()
{
    QString type;
    type = ui->typeLabel->text();
    return type;
}
void ListWidget::setType(QString& type)
{
    ui->typeLabel->setText(type);
}

QString ListWidget::getImage()
{
    QString path;

    path = ui->pathLabel->text();

    return path;
}
void ListWidget::setImage(QPixmap& image)
{
    ui->imageLabel->setPixmap(image);
}
void ListWidget::setGender(QString& gender)
{
    ui->genderLabel->setText(gender);
}
QString ListWidget::getGender()
{
    QString gender;

    gender = ui->genderLabel->text();

    return gender;
}
int ListWidget::id()
{
    return ui->idLabel->text().toInt();
}

void ListWidget::on_loadPushButton_clicked()
{
    if(flag == true)    return;

    QString type;

    QString directory = ui->directoryLabel->text();
    QString directory2 = ui->directoryLabel_2->text();

    QPixmap *buffer = new QPixmap;

    QImage *image = new QImage(directory);


    *buffer = QPixmap::fromImage((*image));

    type = ui->typeLabel->text();

    cephFile = new QFile(directory);
    panoFile = new QFile(directory2);

    unsignedViewer = new MainWindow();
    connect(this, SIGNAL(setLoadMainWindow(QString, QString, QString) ),
            unsignedViewer, SLOT(setReceiveMainWindow(QString, QString, QString) ));
    unsignedViewer->show();

    emit setLoadMainWindow(type, cephFile->fileName(), panoFile->fileName());

    connect(unsignedViewer, SIGNAL(closeMainWindow( ) ),
            this, SLOT(delMainWindow() ));
    flag = true;
}
void ListWidget::delMainWindow(){
    flag = false;
    delete unsignedViewer;
}

