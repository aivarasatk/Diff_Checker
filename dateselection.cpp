#include "dateselection.h"
#include "ui_dateselection.h"

DateSelection::DateSelection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateSelection)
{
    ui->setupUi(this);
    ui->calendarWidget->setSelectedDate(QDate::currentDate());
}

DateSelection::~DateSelection()
{
    delete ui;
}

void DateSelection::on_pushButton_clicked()
{
    selectedDate = ui->calendarWidget->selectedDate();
    close();
}
