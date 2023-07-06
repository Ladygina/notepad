#include "mainwindow.h"
#include "ui_mainwindow.h"


size_t MainWindow::newFileName = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->newAct->setIcon(QPixmap(tr(":/toolBar/images/new.jpg")));
    ui->openAct->setIcon(QPixmap(tr(":/toolBar/images/open.jpg")));
     ui->saveAct->setIcon(QPixmap(tr(":/toolBar/images/save.png")));
      ui->saveAllAct->setIcon(QPixmap(tr(":/toolBar/images/saveAll.png")));


    //!!! Под Linux нет такого каталога. Посмотрите методы Qt, возвращающие стандартные каталоги на любой платформе.
   fileSystem.setRootPath(QDir::rootPath());

   ui->treeView->setModel(&fileSystem);

   ui->fileTreeDock->setWidget(ui->treeView);

   ui->fileTreeDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

   ui->fileTreeDock->setWindowTitle(tr("Проводник"));


   ui->listView->setModel(&strList);

   ui->activeFilesDock->setWidget(ui->listView);

   ui->activeFilesDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

   ui->activeFilesDock->setWindowTitle(tr("Обозреватель открытых документов")); //!!! tr()

   this->setWindowTitle(tr("Текстовый редактор"));

   connect(ui->fileTreeAct,SIGNAL(toggled(bool)),this, SLOT(on_fileTreeAct(bool)));

   connect(ui->activeFilesAct,SIGNAL(toggled(bool)),this, SLOT(on_activeFilesAct(bool)));

   bar = new QToolBar(this);

  bar->addAction(ui->newAct);
  bar->addAction(ui->openAct);
  bar->addAction(ui->saveAct);
   bar->addAction(ui->saveAllAct);

   addToolBar(Qt::TopToolBarArea,bar);

};

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* closeEvent)
{
    if(!on_exitAct_triggered())
     closeEvent->ignore();
}


bool MainWindow::on_exitAct_triggered()
{
    bool isExistChangedDoc = false;

    for(auto changedDoc : isChangedDoc)
    {
        if(changedDoc)
        {
            isExistChangedDoc = true;
        }
    }
    if(!isExistChangedDoc)
    {
        qApp->quit();
        return true;
    }

    Dialog* dial = new Dialog(this);

    dial->setWindowTitle(tr("Хотите сохранить файлы перед выход ?"));  //!!! tr()

    dial->fillTable(fileDirs,fileNames,isChangedDoc);


     int mode = dial->exec();

    if(mode == QDialog::Accepted)
    {
    // on_saveAllAct_triggered();
        int count = ui->tabWidget->count();
        for(int index = 0 ; index < count ; index++)
          {
              ui->tabWidget->setCurrentIndex(index);
              if(isChangedDoc.at(index))
              on_saveAct_triggered();
         }
         qApp->quit();
         return true;
    }
    else if(mode == QDialog::Rejected )
    {
         qApp->quit();
         return true;
    }
    else if (mode == -1)
    {
        return false;
    }


}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
   on_closeAct_triggered();
}

bool MainWindow::on_closeAct_triggered()
{

     if(!isChangedDoc.at(ui->tabWidget->currentIndex()))
     {
         removeTab();
         return true;
     }

    QMessageBox msgBox;
    msgBox.setText(tr("Хотите сохранить изменения в файле ")+ui->tabWidget->tabText(ui->tabWidget->currentIndex())+ tr(" ?"));
    QPushButton *saveButton = msgBox.addButton(tr("Сохранить"), QMessageBox::ActionRole);
    QPushButton *closeButton = msgBox.addButton(tr("Закрыть"),QMessageBox::ActionRole);
    QPushButton *cancelButton = msgBox.addButton(tr("Отменить"),QMessageBox::ActionRole);

    msgBox.exec();

    if(msgBox.clickedButton() == cancelButton)
    {
     return false;
    }
    else{

        if (msgBox.clickedButton() == saveButton)
        {
                on_saveAct_triggered();
         }

    removeTab();
    }

    return true;

}

void MainWindow::removeTab()
{
    int index =  ui->tabWidget->currentIndex();
     fileDirs.removeAt(index);
     fileNames.removeAt(index);
     isExistDoc.removeAt(index);
     isChangedDoc.removeAt(index);
     strList.setStringList(fileNames);
     ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}


void MainWindow::on_newAct_triggered()
{

    QString countTabsStr ;
    countTabsStr.setNum(++newFileName);
    QString nameFile = tr("Новый документ")+ countTabsStr +tr(".txt");
    fileNames.push_back(nameFile);
    strList.setStringList(fileNames);
    fileDirs.push_back(QDir(tr("")));
    QPlainTextEdit* textEd = new QPlainTextEdit(this);
    ui->tabWidget->addTab(textEd,nameFile);
    if(ui->tabWidget->currentIndex() == -1)
    {
        ui->tabWidget->setCurrentIndex(0);
    }
    else {
     ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1 );
    }

    connect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));

    isExistDoc.push_back(false);
    isChangedDoc.push_back(false);

}


void MainWindow::on_closeAllAct_triggered()
{
    int count = ui->tabWidget->count();
    int index = 0;
    for(int i = 0 ; i < count ; i++)
      {
          ui->tabWidget->setCurrentIndex(index);
          if(!on_closeAct_triggered())
          {
              ++index;
          }
     }
}

void MainWindow::on_openAct_triggered()
{

    //!!! А если пользователь python в файл добавит? Там свои расширения...

    QString openFileName = QFileDialog::getOpenFileName(this,tr("Выбрать файл для открытия"),QDir::rootPath())  ;   //!!! tr()

   loadFileData(openFileName);

}



void MainWindow::on_saveAct_triggered()
{

    QPlainTextEdit* textEd =  qobject_cast<QPlainTextEdit*> (ui->tabWidget->currentWidget());
   if( textEd==nullptr )
    {
       QErrorMessage * msg = new QErrorMessage(this);
       msg->showMessage(tr("Ошибка сохранения файла"));     //!!! tr()
          return;

    }
    int index = ui->tabWidget->currentIndex();
    if(!isExistDoc.at(index))
    {
        on_saveAsAct_triggered();
        return;
    }

    QString filePath = fileDirs.at(index).absolutePath() + tr("//") + fileNames.at(index);
    QFile file(filePath);

    QTextStream text(&file);
    if(file.open(QIODevice::WriteOnly))
    {
       text<<textEd->toPlainText();
       ui->tabWidget->setTabText(index,ui->tabWidget->tabText(index).remove(tr("*")));
       connect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));
       isChangedDoc[index] = false;
    }
    else
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Ошибка сохранения файла"));
        return;
    }

    file.close();
}

void MainWindow::on_saveAsAct_triggered()
{
     ///!!! Жесткая зависимость от расширений
    
    QString saveFileName = QFileDialog::getSaveFileName(this,tr("Выбрать файл для сохранения"),QDir::rootPath());   //!!! tr()

    QPlainTextEdit* textEd =  qobject_cast<QPlainTextEdit*> (ui->tabWidget->currentWidget());
   if( textEd==nullptr )
    {
       QErrorMessage * msg = new QErrorMessage(this);
       msg->showMessage(tr("Ошибка сохранения файла"));
          return;
    }

    QFile file(saveFileName);
    QTextStream text(&file);
    if(file.open(QIODevice::WriteOnly))
    {
       text<<textEd->toPlainText();  
       int index = ui->tabWidget->currentIndex();
   QStringList list = saveFileName.split(tr("/"));
   fileNames[index] = list.back();
   saveFileName.chop(list.back().size()+1);
   fileDirs[index] = QDir(saveFileName);
   isExistDoc[index] = true;
   ui->tabWidget->setTabText(index,list.back());

   connect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));
   isChangedDoc[index] = false;
    }
    else
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Ошибка сохранения файла"));
        return;

    }

    file.close();
}

void MainWindow::on_saveAllAct_triggered()
{
    int prev_index = ui->tabWidget->currentIndex();
    int count = ui->tabWidget->count();
    for( int index = 0 ; index < count; index++)
    {
        ui->tabWidget->setCurrentIndex(index);
        on_saveAct_triggered();
    }

    ui->tabWidget->setCurrentIndex(prev_index);

}


void MainWindow::on_fileTreeDock_visibilityChanged(bool visible)
{
    ui->fileTreeAct->setChecked(visible);
}

void MainWindow::on_activeFilesDock_visibilityChanged(bool visible)
{
    ui->activeFilesAct->setChecked(visible);
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    if(!index.isValid())
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Возникла ошибка в обозревателе открытых документов"));   //!!! tr()
        return;
    }
    ui->tabWidget->setCurrentIndex(index.row());
}

void MainWindow::on_fileTreeAct(bool visible)
{

    if(visible)
    {
        ui->fileTreeDock->show();
    }
    else
    {
      ui->fileTreeDock->close();
    }
}
void MainWindow::on_activeFilesAct(bool visible)
{
    if(visible)
    {
        ui->activeFilesDock->show();
    }
    else
    {
        ui->activeFilesDock->close();
    }
}



void MainWindow::on_cutAct_triggered()
{
    QPlainTextEdit* textEd =  qobject_cast<QPlainTextEdit*> (ui->tabWidget->currentWidget());
    if(textEd == nullptr)
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Ошибка вырезки"));
        return;
    }

    connect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));

    textEd->cut();
}

void MainWindow::on_copyAct_triggered()
{
    QPlainTextEdit* textEd =  qobject_cast<QPlainTextEdit*> (ui->tabWidget->currentWidget());
    if(textEd == nullptr)
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Ошибка копирования"));
        return;
    }

    textEd->copy();
}

void MainWindow::on_insertAct_triggered()
{
    QPlainTextEdit* textEd =  qobject_cast<QPlainTextEdit*> (ui->tabWidget->currentWidget());
    if(textEd == nullptr)
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Ошибка вставки"));
        return; //
    }

    connect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));


    textEd->paste();
}

void MainWindow::on_deleteAct_triggered()
{
    QPlainTextEdit* textEd =  qobject_cast<QPlainTextEdit*> (ui->tabWidget->currentWidget());
    if(textEd == nullptr)
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Ошибка удаления"));
        return; //
    }


    connect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));

    textEd->textCursor().removeSelectedText();
}

void MainWindow::on_highlightAct_triggered()
{
    QPlainTextEdit* textEd =  qobject_cast<QPlainTextEdit*> (ui->tabWidget->currentWidget());
    if(textEd == nullptr)
    {
        QErrorMessage * msg = new QErrorMessage(this);
        msg->showMessage(tr("Ошибка выделить все"));      //!!! tr()
        return;
    }

    textEd->selectAll();
}


  void MainWindow::tabModification()
  {
     int index =  ui->tabWidget->currentIndex();

      if(! ui->tabWidget->tabText(index).contains(tr("*")))
      {
     ui->tabWidget->setTabText(index, ui->tabWidget->tabText(index) + tr("*"));
     isChangedDoc[index] = true ;
      }
      else
      {
          QPlainTextEdit* textEd = qobject_cast<QPlainTextEdit*>(ui->tabWidget->widget(index) ) ;

          if( textEd == nullptr )
          {

              QErrorMessage * msg = new QErrorMessage(this);
              msg->showMessage(tr("Ошибка исправления вкладки виджета "));  //!!! tr()
              return; //
          }

             disconnect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));
      }


  }

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
    {
        QErrorMessage* msg = new QErrorMessage(this);
        msg->showMessage(tr("Oшибка открытия файла"));
    }
   QString path = fileSystem.filePath(index);
   if(path.contains(tr(".")))
   {
        loadFileData(path);
   }
   else
   {
       return;
       // кликнул на папку , выходишь
   }
}

   void MainWindow::loadFileData(const QString& path)
   {
       QFile file(path);
       if(!file.open(QIODevice::ReadOnly))
       {
           QErrorMessage * msg = new QErrorMessage(this);
           msg->showMessage(tr("Файл с данным расширением не открывается"));
              return;
       }

       QFileInfo fileInfo (file);


       QPlainTextEdit* textEd ;
       CodeEditor* codeEditor;

    QVector<HighlightingRule> highlightingRule =   loadXmlFile(path);

       //!!! Зависимость от расширений
       if(!highlightingRule.isEmpty())
       {

          codeEditor = new CodeEditor(this);

          Highlighter*   highLighter  = new Highlighter (codeEditor->document(),highlightingRule);

          codeEditor->setPlainText(file.readAll());

          ui->tabWidget->addTab(codeEditor,fileInfo.baseName() + tr(".") + fileInfo.completeSuffix());

          connect(codeEditor,SIGNAL(textChanged()),this , SLOT(tabModification()));

       }
       else
       {
           textEd = new QPlainTextEdit(this);

           textEd->setPlainText(file.readAll());

           ui->tabWidget->addTab(textEd,fileInfo.baseName() + tr(".") + fileInfo.completeSuffix());

           connect(textEd,SIGNAL(textChanged()),this , SLOT(tabModification()));
       }



       fileNames.push_back(QString (fileInfo.baseName() + tr(".") + fileInfo.completeSuffix()) );

       fileDirs.push_back(fileInfo.path());

       strList.setStringList(fileNames);

       ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1 );

       file.close();

       isExistDoc.push_back(true);

       isChangedDoc.push_back(false);

   }


  QVector<HighlightingRule> MainWindow::loadXmlFile(const QString &path)
   {

             QFile file(tr(":xml/color.xml") ); //!!! Это нужно считывать в основмном окне, т.к. там эти данные нужны.
                                              //!!! Сюда надо передавать контейнер.

             QFileInfo fileInfo(path);

             QVector<HighlightingRule> highlightingRules;

                       if(file.open(QFile::ReadOnly | QFile::Text))
                       {
                           QXmlStreamReader xmlStream;

                           HighlightingRule rule;

                           QTextCharFormat format;

                           QRegularExpression pattern;

                           xmlStream.setDevice(&file);

                              while(!xmlStream.atEnd() && !xmlStream.hasError())
                           {
                               xmlStream.readNext();


                               if( xmlStream.isStartElement() )
                               {
                                   if(xmlStream.name() == tr("syntax"))
                                   {
                                    QXmlStreamAttribute attr =   xmlStream.attributes().at(2);

                                    QString exstensions = attr.value().toString();

                                   QStringList Exstensionlist = exstensions.split(tr(" "));


                                    bool isCodeInFile = false;

                                    for ( auto exstension :  Exstensionlist )
                                   {



                                    if(fileInfo.completeSuffix() == exstension)
                                    {
                                        isCodeInFile = true;
                                    }

                                    }

                                    if(!isCodeInFile)
                                    {
                                        return   highlightingRules;;
                                    }


                                   }

                                       if ( xmlStream.name() == tr("pattern") )
                                       {

                                        QRegularExpression reg(xmlStream.attributes().at(0).value().toString());

                                        rule.pattern = reg;
                                        qDebug()<<reg;

                                        }


                                         if ( xmlStream.name() == tr("format") )
                                         {
                                            format.setForeground(QBrush(QColor((xmlStream.attributes().at(0).value().toString()))));

                                            qDebug()<<xmlStream.attributes().at(0).value().toString();

                                           format.setFont(xmlStream.attributes().at(1).value().toString());

                                           qDebug() << xmlStream.attributes().at(1).value().toString();

                                           rule.format = format;

                                           highlightingRules.push_back(rule);

                                          }

                               }

                          }

                       }
                       else
                       {

                       }

                       return  highlightingRules;


   }
