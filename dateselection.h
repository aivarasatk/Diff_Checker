#ifndef DATESELECTION_H
#define DATESELECTION_H

#include <QDialog>
#include <QDate>

namespace Ui {
class DateSelection;
}

class DateSelection : public QDialog
{
    Q_OBJECT

public:
    explicit DateSelection(QWidget *parent = 0);
    ~DateSelection();
    QDate getDate(){return selectedDate;}

private slots:
    void on_pushButton_clicked();

private:
    QDate selectedDate;
    Ui::DateSelection *ui;
};

#endif // DATESELECTION_H
