#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"dialog.h"
#include"highlighter.h"
#include"linenumberarea.h"
#include <QMainWindow>
#include<QMenu>
#include<QAction>
#include<QMenuBar>
#include<QFileDialog>
#include<QTabWidget>
#include<QDir>
#include<QString>
#include<QVector>
#include<QDebug>
#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QFileInfo>
#include<QMessageBox>
#include<QPushButton>
#include<QDir>
#include<QDialog>
#include<QDockWidget>
#include<QTreeView>
#include<QFileSystemModel>
#include<QListView>
#include<QStringListModel>
#include<QStringList>
#include<QErrorMessage>
#include<QWidgetItem>
#include<QToolBar>
#include<QIcon>
#include <QPlainTextEdit>
#include<QPainter>
#include<QTextBlock>
#include<QCloseEvent>
#include<QXmlStreamReader>
#include<QXmlAttributes>
#include <QXmlStreamAttribute>
#include <QXmlStreamAttributes>
#include<QRegularExpression>


struct HighlightingRule;



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

        friend class Dialog;

protected:

   static size_t newFileName;

   QFileSystemModel  fileSystem ;

   QStringList fileNames;

   QStringListModel strList;

   QVector<QDir> fileDirs;

   QToolBar * bar;

   void loadFileData(const QString &);

  QVector<HighlightingRule> loadXmlFile(const QString &);

  void removeTab();

  QVector<bool> isExistDoc;

  QVector<bool> isChangedDoc;


public:
     MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


private slots:

  void on_newAct_triggered();
  bool on_closeAct_triggered();
  void on_closeAllAct_triggered();
  void on_openAct_triggered();
  bool on_exitAct_triggered();
  void on_saveAct_triggered();
  void on_saveAsAct_triggered();
  void on_saveAllAct_triggered();

  void on_fileTreeDock_visibilityChanged(bool visible);
  void on_activeFilesDock_visibilityChanged(bool visible);
  void on_listView_clicked(const QModelIndex &index);
 // void on_treeView_clicked(const QModelIndex &index);

  void on_activeFilesAct(bool visible);
  void on_fileTreeAct(bool visible);

  void on_cutAct_triggered();
  void on_copyAct_triggered();
  void on_insertAct_triggered();
  void on_deleteAct_triggered();
  void on_highlightAct_triggered();

  void tabModification();

  void on_tabWidget_tabCloseRequested(int index);
  void on_treeView_doubleClicked(const QModelIndex &index);

  void closeEvent(QCloseEvent * closeEvent) override;

};
#endif // MAINWINDOW_H
