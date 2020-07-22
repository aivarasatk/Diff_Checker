#ifndef CHECKER_H
#define CHECKER_H

#include "data.h"
#include "filewriter.h"
#include "errors.h"

#include <QWidget>
#include <QDate>


class Checker
{
public:
    Checker();
    int getErrorCounter() {return errorCounter;}
    Errors& getErrors(){return errors;}
    std::map<QString,QString>& getFileIdentification() {return fileIdentification;}

    void resetData() {errorCounter = 0; errors.clearContainers();}

    void compareDocuments(Data data);
    int compareLines(std::vector<QString> parentLine, std::vector<QString> childLine, std::vector<int> &mismatchFields);

    std::vector<QString> getCertificateKeyValues(QString keyList);//tikrinami dukteriniu kodai

    bool dateInInterval(std::vector<QString> item);
    int dateCompareResult(QString dateString1, QString dateString2);
    int compareDates(QDate parentDate, QDate childDate);
    bool isDate(std::vector<int> parseDateAt, int currIndex);
    QDate getDate(QString dateString);
    int getMonth(QString dateString);

    void setSearchFrom(QDate &date) {searchFrom = date;}
    void setSearchTo(QDate &date) {searchTo = date;}

    void itemChecked(bool isItemInVectorCheked[], int index, int &checkedCounter);
    bool checkCertificateKey(QString docName, std::vector<QString>& certificateKeys,
                             std::vector<QString> &foundValues);
    //klaidu atsiminimas
    void addParentToList(std::vector<QString> parentItem);
    void addChildToList(std::vector<QString> childItem, QString& description);
    void addMismatchToList(std::vector<int>& mismatchFields);
    void addNotInGen(std::vector<QString>& item, QString docName);
    void addDuplicateInGen(std::vector<QString>& item);
    void addNotInChildren(std::vector<QString>& item);
    void addDuplicateInChildren(std::vector<QString>& item, QString docName);

    void addMisplacedProductToList(std::vector<QString> parentData, std::vector<QString> childData,
                                   QString docName, uint keyListSize);
    void addUncheckedCertificateKeysToErrorList(std::vector<QString> parentData,
                                                         std::vector<QString> certificateKeys);
    std::vector<QString> allFieldsEmpty(int size);
    std::vector<int> allFieldsMismatch(int size);
    bool onlyInGen(std::vector<QString>& certificateKeys);

private:
    QDate searchFrom;
    QDate searchTo;
    int errorCounter;

    Errors errors;
    std::map<QString,QString> fileIdentification;

    enum class I07 {UNIQUELINENUMBER = 0, CODE = 1, NAME = 2, CERTIFICATEKEY = 3,
                    AMOUNT = 4, EXPIRATIONDATE = 5, UNIQUENUMBERPAP = 6, DIMDATE = 7};
    enum class Sertificate{UNIQUEID = 0, MONTH = 1, IDENTIFIERS = 2};

    void checkForDuplicates(std::vector<std::vector<QString>> I07Data, bool isGenData);

};

#endif // CHECKER_H
