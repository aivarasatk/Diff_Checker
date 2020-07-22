#include "errorwriter.h"

#include <QDateTime>
#include <QDebug>
ErrorWriter::ErrorWriter(QString fileName):
    fileWriter(fileName)
{
    this->fileName = fileName;
}


QString ErrorWriter::generateRandomName(){
    QString name = "rezultatas-";
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh.mm.ss");
    QString extension = ".xlsx";

    return name+date+extension;
}

void ErrorWriter::writeErrorsToFile(Errors &errors, std::vector<QString> headers){

    fileWriter.writeHeaderData(headers);

    writeMismatches(errors);

    if(errors.notInGenFile.size() != 0){
        writeMissingItems(errors.notInGenFile,"GEN FAILE NERASTA", headers.size()-1);
    }

    if(errors.notInChildFiles.size() != 0){
        writeMissingItems(errors.notInChildFiles,"DUKTERINIUOSE FAILUOSE NERASTA", headers.size()-1);
    }

    if(errors.duplicatesInGenFile.size() != 0){
        writeDuplicateItems(errors.duplicatesInGenFile,"GEN FAILE RASTI DUBLIAI", headers.size()-1);
    }

    if(errors.duplicatesInChildFiles.size() != 0){
        writeDuplicateItems(errors.duplicatesInChildFiles,"DUKTERINIUOSE FAILUOSE RASTI DUBLIAI", headers.size()-1);
    }
    fileWriter.saveFile();
}

void ErrorWriter::writeMismatches(Errors& errors){
    for(uint i = 0; i < errors.foundErrorsParent.size(); ++i){
        fileWriter.writeRow(errors.foundErrorsParent[i]);
        fileWriter.writeDataToExcelAppend(errors.foundErrorsChild[i], errors.mismatchInChild[i]);
        fileWriter.separateRecords(errors.foundErrorsParent[i].size()-1);
    }
}
void ErrorWriter::writeCurrentProductErrors(std::vector<QString>& childDataItem, std::vector<int>& mismatchFields){
    fileWriter.writeDataToExcelAppend(childDataItem, mismatchFields);
}

void ErrorWriter::writeMissingItems(std::vector<std::vector<QString>>& missingItems, QString subHeaderText, int cellMergeSize){
    fileWriter.setFileNameHeadlineCurrentRow(subHeaderText, cellMergeSize);//nustatom texta antrastes

    for(auto item : missingItems){
        writeMissingItemError(item);
    }
}

void ErrorWriter::writeDuplicateItems(std::vector<std::vector<QString> > &duplicateItems, QString subHeaderText, int cellMergeSize){
    fileWriter.setFileNameHeadlineCurrentRow(subHeaderText, cellMergeSize);//nustatom texta antrastes

    for(auto item : duplicateItems){
        writeMissingItemError(item);
    }
}

void ErrorWriter::writeMissingItemError(std::vector<QString> item,QString headlineMessage){
    if(!headlineMessage.isEmpty()){
        item.push_back(headlineMessage);
    }

    fileWriter.writeRow(item);
}
