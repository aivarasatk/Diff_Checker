#include "filewriter.h"

#include "xlsxdocument.h"
#include <QDebug>

FileWriter::FileWriter(QString fName):
    fileName(fName), excelFile(new QXlsx::Document(fileName)),
    defaultRow(2), defaultColumn(2),
    currentRow(defaultRow), currentColumn(defaultColumn)
{
}

void FileWriter::saveFile(){
    excelFile->save();
}
void FileWriter::writeStringToCurrentRowAppend(QString data, QXlsx::Format format){
    excelFile->write(currentRow,currentColumn,data, format);
    ++currentColumn;
}
void FileWriter::writeRow(std::vector<QString> itemList, QXlsx::Format format){

    format.setBorderStyle(format.BorderStyle::BorderThin);

    for(auto item: itemList){
        if(!widthsSetToDefault){
            setDefaultColumnWidths(item.length());
        }else if(widthsSetToDefault){
           adjustColumnWidths(item.length());
        }

        writeStringToCurrentRowAppend(item, format);
    }

    if(!widthsSetToDefault){
        widthsSetToDefault = true;
    }
    prepareNextLine();

}
void FileWriter::writeDataToExcelAppend(std::vector<QString> itemList, std::vector<int> missmatchFields){
    writeRow(itemList);

    for(int index: missmatchFields){
        QString data = excelFile->read(currentRow-1, index + 1 + defaultColumn - 1).toString();

        QXlsx::Format colorRed = getFormatColor(QColor(218,150,148));

        excelFile->write(currentRow-1, index + 1 + defaultColumn - 1, data, colorRed);//cannot add format without writing
    }
}

void FileWriter::writeHeaderData(std::vector<QString> headerData){
    QXlsx::Format boldFont;
    boldFont.setFontBold(true);
    writeRow(headerData, boldFont);
}

void FileWriter::prepareNextLine(){
    ++currentRow;
    currentColumn = defaultColumn;
}

void FileWriter::setDefaultColumnWidths(int itemLength){
    columnWidths.push_back(itemLength);
    excelFile->setColumnWidth(currentColumn,columnWidths.back());
}
void FileWriter::adjustColumnWidths(int itemLength){
    if(itemLength > columnWidths[currentColumn-defaultColumn]){
     columnWidths[currentColumn-defaultColumn] = itemLength;
     excelFile->setColumnWidth(currentColumn,itemLength);
    }
}

QXlsx::Format FileWriter::getFormatColor(QColor color){
    QXlsx::Format colorFormat;
    colorFormat.setPatternBackgroundColor(color);
    colorFormat.setFillPattern(QXlsx::Format::PatternSolid);

    return colorFormat;
}

void FileWriter::mergeCurrentRowColumns(int to, QXlsx::Format defaultFormat){
    to+=defaultColumn;
    char fromLetter = 'A' - 1;

    QString rangeStart = (fromLetter + defaultColumn) + QString::number(currentRow);
    QString rangeEnd = (fromLetter + to) + QString::number(currentRow);
    QString cellRange = rangeStart + ":" + rangeEnd;

    defaultFormat.setHorizontalAlignment(defaultFormat.HorizontalAlignment::AlignHCenter);
    defaultFormat.setFontBold(true);
    excelFile->mergeCells(cellRange, defaultFormat);
}

void FileWriter::setFileNameHeadlineCurrentRow(QString name, int mergeCellsTo){

    QXlsx::Format coloredBordered = getFormatColor(QColor(235,241,222));
    coloredBordered.setHorizontalAlignment(coloredBordered.HorizontalAlignment::AlignHCenter);
    mergeCurrentRowColumns(mergeCellsTo);
    writeStringToCurrentRowAppend(name, coloredBordered);

    prepareNextLine();
}

void FileWriter::separateRecords(int mergeTo){
    QXlsx::Format fillColor = getFormatColor(QColor(217,217,217));
    mergeCurrentRowColumns(mergeTo, fillColor);
    prepareNextLine();
}
