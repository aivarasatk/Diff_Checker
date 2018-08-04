#ifndef INPUTFILEHANDLER_H
#define INPUTFILEHANDLER_H
#include <QWidget>

#include "Data.h"

class InputFileHandler
{
public:
    InputFileHandler();

    QString selectFile(QWidget *window);
    bool readHowManyLinesToSkip(QString fileName, Data *data);

    bool readDataFiles(Data *data);
    std::vector<std::vector<QString>> readDataFile(std::ifstream& file, Data *data);
    QString parseLine(QString line);
    void skipLines(int skips, std::ifstream& file);
    std::vector<QString> readBlock(std::vector<int>& Data, std::ifstream& file);

    bool readHeaderNames(QString fileName, Data *data);
    bool readKeyValueFile(QString fileName, std::map<QString,QString>& fileIdentification);
    std::pair<QString, QString> mapPair(QString readLine);
private:
    QString currentDirectory;

    enum class I06{FILENAME = 0};

};

#endif // FILEHANDLER_H
