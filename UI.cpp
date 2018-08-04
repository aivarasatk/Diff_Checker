#include "UI.h"
#include "ui_UI.h"

#include <QMessageBox>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDir>
#include <QDebug>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    fileHandler(new InputFileHandler()), checker(new Checker()),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    data = new Data();
    checkPressed = false;

    if(!fileHandler->readKeyValueFile("dukteriniai_legenda.txt", checker->getFileIdentification())){
        QMessageBox::critical(this, "Klaida", "Nerastas dukterinių failų identifikaciją nusakantis failas");
        exit(1);
    }
    if(!fileHandler->readHowManyLinesToSkip("superImportantFile.txt", data)){
        QMessageBox::critical(this,"Klaida","Nerastas duomenų formatą nusakantis failas!");
        exit(1);
    }

    if(!fileHandler->readHeaderNames("headers.csv", data)){
        QMessageBox::warning(this,"Klaida","Nerastas antraščių failas. Galimas nepilnas atvaizdavimas!");
    }

}
Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonChoose_clicked()
{
    QString parentFile = fileHandler->selectFile(this);
    if(!parentFile.isEmpty()){
        data->setParentFile(parentFile);
    }
}

void Dialog::on_buttonChoose2_clicked()
{
    QString childFile = fileHandler->selectFile(this);
    if(!childFile.isEmpty()){
        data->setChildFile(childFile);
    }
}

void Dialog::on_buttonCheck_clicked()
{
    if(checkPressed){
        data->clearAll();
        checker->resetData();
    }
    checkPressed = true;

    if(data->getParentFile().isEmpty() || data->getChildFile().isEmpty()){
        QMessageBox::information(this,"Klaida","Nepasirinkti failai!");
    }else{
        if(!fileHandler->readDataFiles(data)){
            QMessageBox::warning(this,"Klaida","Nepavyko atidaryti pasirinktų failų.");
            return;
        }

        checker->compareDocuments(*data);
        ErrorWriter errorWriter(ErrorWriter::generateRandomName());
        errorWriter.writeErrorsToFile(checker->getErrors(),data->getHeaderNames());

        QString message = "Rasta " + QString::number(checker->getErrorCounter()) + " neatitikimai";

        if(checker->getErrorCounter() == 0){
            QMessageBox::information(this,"Baigta",message);
        }else{
            QMessageBox resultInformation(this);

            message += "\n";
            resultInformation.setText(message);

            resultInformation.setCheckBox(new QCheckBox("Atverti sukurtą dokumentą", this));
            resultInformation.exec();
            if(resultInformation.checkBox()->isChecked()){
                QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + '/' + errorWriter.getFileName()));
            }
        }


    }
}

void Dialog::on_dateButtonFrom_clicked()
{
    //naujas langas, kalendorius, nustatoma data nuo
    showCalendar();
    QDate selectedDate = dateSelection->getDate();
    ui->lineEditDateFrom->setText(selectedDate.toString("yyyy-MM-dd"));

    checker->setSearchFrom(selectedDate);
}

void Dialog::on_dateButtonTo_clicked()
{
    //naujas langas, kalendorius, nustatoma data iki
    showCalendar();
    QDate selectedDate = dateSelection->getDate();
    ui->lineEditDateTo->setText(selectedDate.toString("yyyy-MM-dd"));

    checker->setSearchTo(selectedDate);
}

void Dialog::showCalendar(){
    dateSelection = new DateSelection(this);
    dateSelection->setModal(true);
    dateSelection->exec();
}
