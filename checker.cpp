#include "checker.h"
#include "xlsxdocument.h"

#include <QDateTime>
#include <algorithm>
Checker::Checker()
{
    errorCounter = 0;
}
void Checker::compareDocuments(Data data){
    auto parentData = data.getParentProductData();
    auto childData = data.getChildProductData();
    auto docNames = data.getDocNames();
    auto childDataBreaks = data.getDocBreaks();

    int childDataArraySize = childData.size();
    bool isItemInChildVectorCheked[childDataArraySize] = {false};
    int childCheckedItemCount = 0;//jei sutaps su arraySize, neiteruosim per masyva

    int parentDataArraySize = parentData.size();
    bool isItemInParentVectorCheked[parentDataArraySize] = {false};
    int parentCheckedItemCount = 0;//jei sutaps su arraySize, neiteruosim per masyva

    int docIndex = 0;
    int childItemCount = 0;


    for(uint j = 0; j< parentData.size(); ++j){//eisim per visus produktus
        docIndex = 0;
        childItemCount = 0;

        std::vector<QString> certificateKeys = getCertificateKeyValues(parentData[j][(int)I07::CERTIFICATEKEY]);
        std::vector<QString> foundValues;
        if(!certificateKeys.empty()){
            for(uint i = 0; i < childData.size(); ++i)//sukamas ciklas pro visus dukterinius duomenis
            {
                std::vector<int> mismatchFields;
                if(parentData[j][ (int)I07::UNIQUELINENUMBER ] == childData[i][ (int)I07::UNIQUENUMBERPAP ])//jei sutampa
                {
                    itemChecked(isItemInChildVectorCheked, i, childCheckedItemCount);
                    itemChecked(isItemInParentVectorCheked, j, parentCheckedItemCount);

                    if(dateInInterval(parentData[j]) &&
                       !checkCertificateKey(docNames[docIndex], certificateKeys, foundValues))
                    {
                        if(foundValues.end() == (std::find(foundValues.begin(), foundValues.end(), docNames[docIndex]))){
                            addMisplacedProductToList(parentData[j], childData[i],
                                                      docNames[docIndex],certificateKeys.size());
                            ++errorCounter;

                        }else{
                            addMisplacedProductToList(parentData[j], childData[i],
                                                      docNames[docIndex],0);
                            ++errorCounter;
                        }

                    }else if(compareLines(parentData[j],childData[i],mismatchFields) == 0){//jei nesutampa eilutes
                        addParentToList(parentData[j]);
                        addChildToList(childData[i], docNames[docIndex]);
                        addMismatchToList(mismatchFields);
                        ++errorCounter;
                    }
                }

                if(childDataBreaks[docIndex] == childItemCount){//kada keisti failo pav
                    ++docIndex;
                }
                ++childItemCount;
            }
        }else if(dateInInterval(parentData[j])){
            itemChecked(isItemInParentVectorCheked, j, parentCheckedItemCount);
            addNotInChildren(parentData[j]);//nera sertifikato, nerasim dukteriniuose
            ++errorCounter;
        }


        if(!certificateKeys.empty() && dateInInterval(parentData[j])){//jei yra nerastu dukteriniuose failuose
            itemChecked(isItemInParentVectorCheked, j, parentCheckedItemCount);
            if(!onlyInGen(certificateKeys)){
                addUncheckedCertificateKeysToErrorList(parentData[j], certificateKeys);
                ++errorCounter;
            }

        }
    }

    //patikrinam ar visi gen failo produktai buvo vaikiniuose
    if(childCheckedItemCount != childDataArraySize){
        docIndex = 0;
        childItemCount = 0;

        for(int i = 0 ; i < childDataArraySize; ++i){
            if(!isItemInChildVectorCheked[i] && dateInInterval(childData[i])){
                addNotInGen(childData[i], docNames[docIndex]);
                ++errorCounter;
            }

            if(childDataBreaks[docIndex] == childItemCount){//kada keisti failo pav
                ++docIndex;
            }
            ++childItemCount;
        }
    }

    //patikrinam ar visi vaikiniu failo produktai buvo gen
    if(parentCheckedItemCount != parentDataArraySize){
        for(int i = 0 ; i < parentDataArraySize; ++i){
            if(!isItemInParentVectorCheked[i] && dateInInterval(parentData[i])){
                addNotInChildren(parentData[i]);
                ++errorCounter;
            }
        }
    }

    checkForDuplicates(parentData, true);
    checkForDuplicates(childData, false);
}

void Checker::checkForDuplicates(std::vector<std::vector<QString>> I07Data, bool isGenData){
    int monthEndIndex = 7;
    int truncateToEndNumber= 10;

    std::vector<QString> addedDuplicates = std::vector<QString>();
    for(auto entry : I07Data){
        int duplicateCounter = 0;
        for(auto possibleDuplicate : I07Data){
            if(entry[(int)I07::CODE] == possibleDuplicate[(int)I07::CODE]
               && entry[(int)I07::EXPIRATIONDATE].remove(monthEndIndex,truncateToEndNumber) == possibleDuplicate[(int)I07::EXPIRATIONDATE].remove(monthEndIndex,truncateToEndNumber)){
                duplicateCounter++;
            }
        }

        bool errorAdded = std::find(addedDuplicates.begin(), addedDuplicates.end(), entry[(int)I07::CODE]) != addedDuplicates.end();
        bool isDuplicate = isGenData ? duplicateCounter > 1 : duplicateCounter > getCertificateKeyValues(entry[(int)I07::CERTIFICATEKEY]).size();
        if(isDuplicate && !errorAdded){
            ++errorCounter;

            addedDuplicates.push_back(entry[(int)I07::CODE]);

            if(isGenData)
                addDuplicateInGen(entry);
            else
                addDuplicateInChildren(entry, "Patikrinti kuriame");
        }
    }
}

std::vector<QString> Checker::getCertificateKeyValues(QString keyList){
    std::vector<QString> keyValues;
    for(auto pair : fileIdentification){
        if(keyList.contains(pair.first)){
            keyValues.push_back(pair.first);
        }
    }
    return keyValues;
}

//-1 - nereikia tikrint (blogas intervalas); 0 - nelygios; 1 - lygios
int Checker::compareLines(std::vector<QString> parentLine, std::vector<QString> childLine, std::vector<int>& mismatchFields){
    //patikrinam ar data patenka i intervala
    int dateIndex = (int)I07::EXPIRATIONDATE;
    if(dateCompareResult(parentLine[dateIndex], childLine[dateIndex])  == -1){//nepatenka i tikrinama intervala; nebetikrinam ir baigiam darba
        mismatchFields.clear();
        return -1;
    }

    for(uint i = 0; i < parentLine.size(); ++i){//name indeksas = 1; praleidziam, koda, nes jau patikrinta
        if(i != (int)I07::UNIQUELINENUMBER && i !=(int)I07::UNIQUENUMBERPAP
           && i != (int)I07::NAME && i != (int)I07::CERTIFICATEKEY)//visada skirsis sie laukai
        {
            if(parentLine[i] != childLine[i])
            {
                mismatchFields.push_back(i);
            }
        }
    }
    if(mismatchFields.empty()){
        return 1;
    }
    return 0;
}

bool Checker::dateInInterval(std::vector<QString> item){
    int dateIndex = (int)I07::EXPIRATIONDATE;
    if(dateCompareResult(item[dateIndex],item[dateIndex]) == -1){
        return false;//nepatekus i intervala grazinam
    }
    return true;
}

QDate Checker::getDate(QString dateString){
    dateString = dateString.trimmed();
    QString format = "yyyy.MM.dd hh:mm";
    QDateTime dt = QDateTime::fromString(dateString,format);
    return dt.date();
}

bool Checker::isDate(std::vector<int> parseDateAt, int currIndex){
    for(auto it: parseDateAt){
        if(it == currIndex){
            return true;
        }
    }
    return false;
}

int Checker::dateCompareResult(QString dateString1, QString dateString2){
    QDate dateParent = getDate(dateString1);
    QDate dateChild = getDate(dateString2);

    return compareDates(dateParent,dateChild);
}
int Checker::compareDates(QDate parentDate, QDate childDate){//-1 - netikrinti produkto; 0 - nesutampa datos; 1 - sutampa datos
    if(!searchFrom.isNull() && !searchTo.isNull()){//jei pasirinktas datu intervalas
        if((parentDate < searchFrom || parentDate > searchTo)
            || (childDate < searchFrom || childDate > searchTo)){
            return -1;
        }
    }
    if(parentDate.year() == childDate.year()){
        if(parentDate.month() != childDate.month()){
            return 0;
        }
    }else if(parentDate.year() != childDate.year()){
        return 0;
    }
    return 1;
}

void Checker::itemChecked(bool isItemInVectorCheked[], int index, int &checkedCounter){
    isItemInVectorCheked[index] = true;
    ++checkedCounter;
}

bool Checker::checkCertificateKey(QString docName, std::vector<QString>& certificateKeys,
                                  std::vector<QString>& foundValues){
    int removeKeyAt = -1;
    for(uint i = 0; i < certificateKeys.size(); ++i){
        if(docName == fileIdentification.find(certificateKeys[i])->second){
            removeKeyAt = i;
            foundValues.push_back(docName);
            break;
        }
    }
    if(removeKeyAt != -1){
        certificateKeys.erase(certificateKeys.begin() + removeKeyAt);
        return true;
    }
    return false;
}

void Checker::addParentToList(std::vector<QString> parentItem){
    parentItem.push_back("GEN");
    errors.foundErrorsParent.push_back(parentItem);
}
void Checker::addChildToList(std::vector<QString> childItem, QString& description){
    childItem.push_back(description);
    errors.foundErrorsChild.push_back(childItem);
}
void Checker::addMismatchToList(std::vector<int>& mismatchFields){
    errors.mismatchInChild.push_back(mismatchFields);
}
void Checker::addNotInGen(std::vector<QString>& item, QString docName){
    item.push_back(docName);
    errors.notInGenFile.push_back(item);
}

void Checker::addDuplicateInGen(std::vector<QString>& item){
    item.push_back("");
    errors.duplicatesInGenFile.push_back(item);
}

void Checker::addNotInChildren(std::vector<QString>& item){
    item.push_back("GEN");
    errors.notInChildFiles.push_back(item);
}

void Checker::addDuplicateInChildren(std::vector<QString>& item,  QString docName){
    item.push_back(docName);
    errors.duplicatesInChildFiles.push_back(item);
}


void Checker::addUncheckedCertificateKeysToErrorList(std::vector<QString> parentData,
                                                     std::vector<QString> certificateKeys){
    for(auto key : certificateKeys){
        addParentToList(parentData);
        QString desc = fileIdentification.find(key)->second + "(Nerasta)";
        addChildToList(allFieldsEmpty(parentData.size()), desc);

        auto allMismatch = allFieldsMismatch(parentData.size());
        addMismatchToList(allMismatch);
    }
}

void Checker::addMisplacedProductToList(std::vector<QString> parentData, std::vector<QString> childData,
                                        QString docName, uint keyListSize){
    addParentToList(parentData);

    QString text = "(Nesutampa nustatyti gamybos padaliniai)";
    if(keyListSize == 0){
        text = "(Rastas dublis)";
    }
    QString description = docName + text;
    addChildToList(childData, description);
    auto allMismatch = allFieldsMismatch(childData.size());
    addMismatchToList(allMismatch);
}

std::vector<QString> Checker::allFieldsEmpty(int size){
    std::vector<QString> allEmpty;
    for(int i = 0; i < size; ++i){
        allEmpty.push_back("-");
    }
    return allEmpty;
}
std::vector<int> Checker::allFieldsMismatch(int size){
    std::vector<int> allMismatch;
    for(int i = 0; i < size; ++i){
        //if(i != (int)I07::CERTIFICATEKEY){
            allMismatch.push_back(i);
        //}
    }
    return allMismatch;
}

bool Checker::onlyInGen(std::vector<QString>& certificateKeys){
    for(auto item : certificateKeys){
        for(auto pair : fileIdentification){
            if(pair.first == item && pair.second == "GEN"){
                return true;
            }
        }
    }
    return false;
}


