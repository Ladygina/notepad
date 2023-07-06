#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Имя ") << tr("Путь ") );
}

Dialog::~Dialog()
{
    delete ui;
}

   void Dialog::fillTable(const QVector<QDir>& fileDirs , const QStringList & fileNames, const QVector<bool>& isChangedDoc)
   {

      int tableIndex =0;
     for(int i =0; i < fileDirs.count() ; i++)
     {
         if(isChangedDoc.at(i))
         {
         ui->tableWidget->insertRow(tableIndex);
         ui->tableWidget->setItem(tableIndex,0,new QTableWidgetItem(fileNames[i],Qt::DisplayRole));
         ui->tableWidget->setItem(tableIndex,1,new QTableWidgetItem(fileDirs[i].path(),Qt::DisplayRole));
         ++tableIndex;
         }

     }

     ui->tableWidget->verticalHeader()->setStretchLastSection(true);
   }


    void Dialog::closeEvent(QCloseEvent* event)
    {
        emit done(-1);
    }
