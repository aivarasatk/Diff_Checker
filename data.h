#ifndef DATA_H
#define DATA_H
#include <QWidget>

#include <QDebug>
class Data
{
public:
    Data();
    ~Data();

    QString getParentFile() {return parentFile;}
    void setParentFile(QString path) {parentFile = path;}

    QString getChildFile() {return childFile;}
    void setChildFile(QString path) {childFile = path;}

    std::vector<int> getI06SkipData(){return I06SkipData;}
    void setI06SkipData(std::vector<int>& I06NewData){ I06SkipData = I06NewData;}

    std::vector<int> getI07SkipData(){return I07SkipData;}
    void setI07SkipData(std::vector<int>& I07NewData){ I07SkipData = I07NewData;}

    std::vector<int> getParseDateAt(){return parseDateAt;}
    void setParseDateAt(std::vector<int>& newParseDateAt){ parseDateAt = newParseDateAt;}

    void setDocNames(std::vector<QString>& names) {docNames = names;}
    std::vector<QString> getDocNames(){return docNames;}

    std::vector<std::vector<QString>> getParentProductData(){return parentProductData;}
    void setParentProductData(std::vector<std::vector<QString>> data) { parentProductData = data;}

    std::vector<std::vector<QString>> getChildProductData(){return childProductData;}
    void setChildProductData(std::vector<std::vector<QString>> data) { childProductData = data;}

    std::vector<int> getDocBreaks() {return docBreaks;}
    void setDocBreaks(std::vector<int> breaks) {docBreaks = breaks;}

    std::vector<QString>& getHeaderNames() {return headerNames;}

    void clearAll();
private:
    QString parentFile;
    QString childFile;

    std::vector<int> I06SkipData;//eiluciu praleidimas
    std::vector<int> I07SkipData;//eiluciu praleidimas
    std::vector<int> parseDateAt;//kuriose eilutese data/os

    std::vector<QString> docNames;//child failu pavadinimai
    std::vector<int> docBreaks;//ties kuriais duomenimis bagaigiasi tam tikras doc
    std::vector<std::vector<QString>> parentProductData;//informacija apie originalo produktus
    std::vector<std::vector<QString>> childProductData;//informacija dukteriniuose failuose

     std::vector<QString> headerNames;//isvedimo lenteles antrasciu pavadinimai

};

#endif // DATA_H
