#ifndef ERRORWRITER_H
#define ERRORWRITER_H

#include "filewriter.h"
#include "errors.h"

class ErrorWriter
{
public:
    ErrorWriter(QString fileName);

    void writeErrorsToFile(Errors& errors, std::vector<QString> headers);

    static QString generateRandomName();
    QString getFileName(){return fileName;}
private:
    FileWriter fileWriter;
    QString fileName;

    void writeMismatches(Errors& errors);
    void writeCurrentProductErrors(std::vector<QString> &childDataItem, std::vector<int> &mismatchFields);

    void writeMissingItems(std::vector<std::vector<QString> > &missingItems, QString subHeaderText, int cellMergeSize);
    void writeDuplicateItems(std::vector<std::vector<QString> > &duplicateItems, QString subHeaderText, int cellMergeSize);
    void writeMissingItemError(std::vector<QString> item, QString headlineMessage = "");
};

#endif // ERRORWRITER_H
