#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<QStringList>
#include<QVector>
#include<QDir>



namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

   void fillTable(const QVector<QDir>& , const QStringList &,const QVector<bool>& isChangedDoc);

private slots:

   void closeEvent(QCloseEvent* event) override;

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
