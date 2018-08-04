#ifndef ERRORS_H
#define ERRORS_H

#include <cstdlib>
#include <vector>
#include <QString>

struct Errors{
    std::vector<std::vector<QString>> notInGenFile;
    std::vector<std::vector<QString>> notInChildFiles;

    std::vector<std::vector<QString>> foundErrorsParent;//same size
    std::vector<std::vector<QString>> foundErrorsChild;//same size
    std::vector<std::vector<int>> mismatchInChild;//same size

    void clearContainers(){
        notInGenFile.clear();
        notInChildFiles.clear();
        foundErrorsParent.clear();
        foundErrorsChild.clear();
        mismatchInChild.clear();
    }
};

#endif // ERRORS_H
