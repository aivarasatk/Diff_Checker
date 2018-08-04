#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QString>
#include "xlsxdocument.h"

class FileWriter
{
public:
    FileWriter(QString fName);

    void saveFile();
    void writeRow(std::vector<QString> itemList, QXlsx::Format format  = QXlsx::Format());
    void writeDataToExcelAppend(std::vector<QString> itemList, std::vector<int> missmatchFields);
    void writeHeaderData(std::vector<QString> headerData);
    void setFileNameHeadlineCurrentRow(QString name, int mergeCellsTo);
    void separateRecords(int mergeTo);

private:
   const static int defaultFrom = -1;
   QString fileName;
   QXlsx::Document* excelFile;
   int defaultRow, defaultColumn;
   int currentRow, currentColumn;

   bool widthsSetToDefault = false;
   const int characterWidth = 8;
   std::vector<int> columnWidths;

   void writeStringToCurrentRowAppend(QString data, QXlsx::Format format = QXlsx::Format());
   void prepareNextLine();

   void setDefaultColumnWidths(int itemLength);
   void adjustColumnWidths(int itemLength);

   void mergeCurrentRowColumns(int to, QXlsx::Format defaultFormat = QXlsx::Format());//merger from defaultColumn to defaultColumn+to

   QXlsx::Format getFormatColor(QColor color);
};

#endif // FILEWRITER_H
