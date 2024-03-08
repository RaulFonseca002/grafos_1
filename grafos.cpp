#include <iostream>
#include <regex>
#include <string>

using namespace std;
bool **matrix;
int matrixSize;


int getLastValue(string str){

    string lastValue;
    regex pattern(",");
    regex_token_iterator<string::iterator> iterator(str.begin(), str.end(), pattern, -1);
    regex_token_iterator<string::iterator> end;

    while (iterator != end) {
        lastValue = iterator->str();
        ++iterator;
    }

    return stoi(lastValue);

}

void fillMatrix(){

    for(int c = 0; c < matrixSize; c++){
        for(int i = 0; i < matrixSize; i++){
            matrix[c][i] = 0;
        }
    }
}

void setMatrix(string str) {

    matrixSize = getLastValue(str) + 1;
    
    matrix = new bool*[matrixSize];

    for(int c = 0; c < matrixSize; c++){
        matrix[c] = new bool[matrixSize];
    }

    fillMatrix();

}

void setEdges(string str){

    regex pattern("\\{(\\d+),(\\d+)\\}");
    smatch matches;
    int line, colum;

    while (regex_search(str, matches, pattern)) {

        line = stoi(matches[1]);
        colum = stoi(matches[2]);
        matrix[line][colum] = 1;

        str = matches.suffix();
    }
}

void setGraph(string str){

    regex pattern("\n");
    regex_token_iterator<string::iterator> iterator(str.begin(), str.end(), pattern, -1);
    regex_token_iterator<string::iterator> end;


    setMatrix(iterator->str().c_str());
    iterator++;
    setEdges(iterator->str().c_str());
}

void freeMatrix(){

    for (int c = 0; c < matrixSize; c++)
    {
        delete[] matrix[c];
    }

    delete[] matrix;
    
}

void printmatrix(){

    for(int c = 0; c < matrixSize; c++){
        for(int i = 0; i < matrixSize; i++){
            printf(" %i", matrix[c][i]);
        }
        
        printf("\n");

    }
}

int main() {
    
    string input = "0,1,2,3,4,5\n{1,2},{2,5}";
    setGraph(input);
    printmatrix();
    freeMatrix();

    return 0;
}
