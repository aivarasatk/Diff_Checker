#ifndef UI_H
#define UI_H

#include <QDialog>

#include "inputfilehandler.h"
#include "checker.h"
#include "dateselection.h"
#include "errorwriter.h"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_buttonChoose_clicked();

    void on_buttonChoose2_clicked();

    void on_buttonCheck_clicked();

    void on_dateButtonFrom_clicked();

    void on_dateButtonTo_clicked();

private:
    InputFileHandler *fileHandler;
    Checker *checker;

    Ui::Dialog *ui;

    Data *data;
    DateSelection *dateSelection;
    void showCalendar();

    bool checkPressed;

};

#endif // UI_H
