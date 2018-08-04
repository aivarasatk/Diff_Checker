#include "inputfilehandler.h"

#include <QFileDialog>
#include <QTextCodec>
#include <QByteArray>
#include <QDebug>
#include <fstream>
#include <string>
InputFileHandler::InputFileHandler()
{
    currentDirectory = QDir::currentPath();
}

QString InputFileHandler::selectFile(QWidget* window){
    QString file = QFileDialog::getOpenFileName(window, "Atidarykit faila", currentDirectory, "*.eip");
    if(!file.isEmpty())
    {
        QFileInfo fileLocation = QFileInfo(file);
        currentDirectory = fileLocation.absolutePath();
    }
    return file;
}

bool InputFileHandler::readHowManyLinesToSkip(QString fileName, Data *data){
    std::ifstream read(fileName.toStdString());
    if(!read.is_open()){
        return false;
    }
    std::string tempBuff;

    std::vector<int> I06SkipData;
    std::vector<int> I07SkipData;
    std::vector<int> parseDateAt;

    getline(read, tempBuff);            //failo pradzioje siuksles

    bool I07 = false;
    do{
        bool parseI07 = false;//ar datos laukas
        std::string number;
        if(getline(read, number)){

            QString qNumber = QString::fromStdString(number);
            int realNumber = -1;//konvertuotas skaicius is string
            if(qNumber.length() < 3 && qNumber[qNumber.length()-1] != 'n'){
                realNumber = qNumber.toInt();

            }else if(qNumber[qNumber.length()-1] == 'n'){//indikatorius kad laukas bus data
                //qDebug() << "L" << qNumber.length();
                qNumber.remove(qNumber.length()-1,1);
                realNumber = qNumber.toInt();
                parseI07 = true;//isiminsim kad reikes versti i skaiciu
            }else if(qNumber.length() > 3){
                I07 = true;//eilute ilga (paaiskinimai), zyme, kad prasideda I07 info
                continue;
            }


            if(!I07){
                I06SkipData.push_back(realNumber);
            }else{
                I07SkipData.push_back(realNumber);
                if(parseI07){//jei skaitinis laukas, reikes perdirbti i skaiciu; isimenam kelintas elementas
                    parseDateAt.push_back(I07SkipData.size()-1);
                }
            }
      }

    }while(read);

    data->setI06SkipData(I06SkipData);
    data->setI07SkipData(I07SkipData);
    data->setParseDateAt(parseDateAt);

    read.close();
    return true;
}
bool InputFileHandler::readDataFiles(Data* data){

    std::ifstream file(data->getParentFile().toStdString());
    if(!file.is_open()){
        return false;
    }

    data->setParentProductData(readDataFile(file, data));
    file.close();

    std::ifstream file2(data->getChildFile().toStdString());
    if(!file2.is_open()){
        return false;
    }

    data->setChildProductData(readDataFile(file2, data));
    file2.close();

    return true;

}

QString InputFileHandler::parseLine(QString line){

    int closingBracketIndex = line.indexOf('>');//informacijos pradzia
    int secondStartingBracketIndex = line.indexOf('<',closingBracketIndex+1);//informacijos pabaiga

    QString result;
    for(int i = closingBracketIndex+1; i<secondStartingBracketIndex; ++i){
        result += line[i];
    }
    result = result.trimmed();
    return result;
}

void InputFileHandler::skipLines(int skips, std::ifstream& file){

    for(int i=0; i<skips; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::vector<QString> InputFileHandler::readBlock(std::vector<int>& skipData, std::ifstream &file){

    std::vector<QString> extractedStrings;
    for(uint j = 0; j<skipData.size()-1; ++j){
        skipLines(skipData[j], file);
        std::string someString;
        getline(file,someString);
        if(!file){//baigesi failas nutraukiam darba
            extractedStrings.clear();//ne pilnai uzpildyta duomenimis, todel pilnai isvalom ir baigiam darba
            return extractedStrings;
        }

        /*QTextCodec *codecFrom  = QTextCodec::codecForName("System");

        QString string = codecFrom->toUnicode(QByteArray::fromStdString(someString));


        //QString string = QString::fromStdString(someString);
        QTextCodec *codecTo = QTextCodec::codecForName("Windows-1257");
        QByteArray encodedStringRes = codecTo->fromUnicode(string);
        qDebug() << encodedStringRes;


        QString parsedLine = parseLine(QString::fromUtf8(encodedStringRes));*/

        QString parsedLine = parseLine(QString::fromStdString(someString));
        extractedStrings.push_back(parsedLine);

    }
    //qDebug() << "\n";
    skipLines(skipData[skipData.size()-1], file);//skipinam iki bloko pabaigos
    return extractedStrings;
}

std::vector<std::vector<QString>> InputFileHandler::readDataFile(std::ifstream& file,Data* data){
    bool adjustedLineIgnores = false;

    std::vector<int> I06SkipData = data->getI06SkipData();
    std::vector<int> I07SkipData = data->getI07SkipData();

    std::vector<std::vector<QString>> fileData;

    std::vector<QString> docNames;
    std::vector<int> docBreaks;
    while(file){
        //I06
         std::vector<QString> I06DocName = readBlock(I06SkipData,file);
        if(I06DocName.size() == 0){//baigem I06 bloka ir baigesi failas. tada baigiam cikla
            break;
        }else{
            docNames.push_back(I06DocName[ (int)I06::FILENAME ]);//visada bus tik vienas pavadinimas
        }
        while(file){//I07, kol nesutiksim I06 pabaigos

            std::vector<QString> blockData = readBlock(I07SkipData,file);
            fileData.push_back(blockData);

            std::string I06EndCheck;
            getline(file, I06EndCheck);//tikrinam ar nepasiekem I06 failo pabaigos
            if(!adjustedLineIgnores){//kadangi NUSKAITEM papildoma eilute, reikia sumazint line ignore
                I07SkipData[0] = I07SkipData[0]-1;//sumazinam sekanti pirma nuskaityma 1, kadangi jau nuskaitem jo eilute
                adjustedLineIgnores = true;
            }

            if((QString::fromStdString(I06EndCheck)).contains("</I06>")){//is naujo I07 prasideda. skaitymas resetinasi
                I07SkipData[0] = I07SkipData[0]+1;//normaliai skaitysim, nes failas baigesi. (nenuskaitem I07 pradzios)
                docBreaks.push_back(fileData.size()-1);//ties size()-1 elementu baigiasi dokumentas
                adjustedLineIgnores = false;
                break;
            }
        }

    }
    data->setDocBreaks(docBreaks);
    data->setDocNames(docNames);
    return fileData;
}

bool InputFileHandler::readHeaderNames(QString fileName, Data *data){
    std::ifstream openFile(fileName.toStdString());

    if(!openFile.is_open()){//jei neatidarem failo, metam klaida
        return false;
    }
    std::string csvString;
    getline(openFile,csvString);


    //parsing
    QStringList headerNames = QString::fromStdString(csvString).split(',');
    if(headerNames.size() == 0){
        return false;
    }

    auto& gottenHeaderNames = data->getHeaderNames();
    for(int i=0; i< headerNames.size(); ++i){//setting headeNames
        gottenHeaderNames.push_back(headerNames.at(i));
    }
    return true;
}

bool InputFileHandler::readKeyValueFile(QString fileName, std::map<QString,QString>& fileIdentification){
    std::ifstream file(fileName.toStdString());

    if(file.is_open()){
        while(file){
            std::string lineString;
            getline(file,lineString);

            QString line = QString::fromStdString(lineString);
            if(!line.isEmpty()){
                fileIdentification.insert(mapPair(line));
            }
        }
        return true;
    }else{
        return false;
    }
}

std::pair<QString, QString> InputFileHandler::mapPair(QString readLine){
    auto splitLine = readLine.split('-');
    QString key = splitLine[0].trimmed();
    QString value = splitLine[1].trimmed();

    return std::pair<QString, QString>(key, value);

}
