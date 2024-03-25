#include <iostream>
#include <regex>
#include <string>
#include <stack>
#include <vector> 
#include <fstream>
using namespace std;

ifstream inputFile("input.txt");
ofstream outputFile("output.txt");
vector<string> permutations;

typedef struct Node
{
    string name;
    int index;
    vector<bool> reachable;

    Node(string name, int index){
        this->name = name;
        this->index = index;
    }

} Node;

string invertString(string str){

    string resp = "";

    //ignore the last char becuse it is a space 
    for(int c = str.length() -2; c >= 0; c--){ 
        resp += str[c];
    }
    //add back the space ignored before
    resp += " ";

    return resp;
}

//codigo de geeksforgeeks
string rotateString(string s, int d){

    reverse(s.begin(), s.begin()+d);
    reverse(s.begin()+d, s.end());
    reverse(s.begin(), s.end());

    return s;
}

class Graph{

    private:

        vector<Node> nodes;
        vector<vector<bool>> matrix;
        vector<string> cicles;
        bool directed;
        char mode;

        int getNodeIndex(string str){

            for(Node n : nodes){
                if(n.name.compare(str) == 0){
                    return n.index;
                }
            }

            return -1;
        }

        void setNodes(string str){

            regex pattern(",");
            regex_token_iterator<string::iterator> tokenIterator(str.begin(), str.end(), pattern, -1);
            regex_token_iterator<string::iterator> end;
            int index = 0;

            while (tokenIterator != end) {
                nodes.push_back(Node(tokenIterator->str(), index));
                index++;
                tokenIterator++;
            }

            setMatrix();
        }

        void setMatrix(){

            int nodesSize = nodes.size();

            matrix.resize(nodesSize);

            for(vector<bool> &n : matrix){
                n.resize(nodesSize);
            }


        }

        void setVerticies(string str){

            regex pattern("\\{([a-zA-Z]+),([a-zA-Z]+)\\}");
            smatch matches;
            
            while (std::regex_search(str, matches, pattern)) {

                matrix[getNodeIndex(matches[2])][getNodeIndex(matches[1])] = true;   

                if(!directed){
                    matrix[getNodeIndex(matches[1])][getNodeIndex(matches[2])] = true;
                }   

                str = matches.suffix();
            }

        }

    public:

        void set(string str){
            clear();
            regex pattern("/");
            regex_token_iterator<string::iterator> iterator(str.begin(), str.end(), pattern, -1);
            regex_token_iterator<string::iterator> end;
            
            if(iterator->str().compare("d") == 0){
                directed = true;
            }else if(iterator->str().compare("nd") == 0){
                directed = false;
            }else{
                directed = false;
                outputFile << "invalid flag, set to not directed\n";
            }

            iterator++;
            setNodes(iterator->str());
            iterator++;
            setVerticies(iterator->str());
            

        }

        Graph(){

        }

        Graph(string str){
            set(str);
        }

        bool isCicle(string path){
            return true;
        }

        void validatePermutations(){

            for(string s : permutations){
                if(isCicle(s) && !isInsertedCicle(s)){
                    cicles.push_back(s);        
                }
            }

        }

        void findCicles(char mode){

            if(mode != 'W' && mode != 'P') return;

            this->mode = mode;

            for (Node n : nodes){
                findCicles(n.name);
            }

            if(mode == 'P'){
                validatePermutations();
            }
        }

        void findCicles(string nodeName) {

            int currentNodeIndex = getNodeIndex(nodeName);
            vector<int> path;
            vector<bool> founded(nodes.size(), false);

            nodes[currentNodeIndex].reachable.resize(nodes.size(), false);

            findCicles(currentNodeIndex, path, founded, currentNodeIndex);
 
        }

        void findCicles(int index, vector<int> path, vector<bool> founded, const int root) {
            
            if(founded[index]){
                if (path[path.size()-2] != nodes[index].index){

                    if(mode == 'W'){
                        addCicle(path, index);
                    }else{
                        permutations.push_back(getPathString(path, index));
                    }
                }
                
                return;
            }

            nodes[root].reachable[index] = true;
            path.push_back(index);
            founded[index] = true;

            for (int c = 0; c < nodes.size(); c++) {
                if (matrix[c][index]) {
                    findCicles(c, path, founded, root);
                }
            }
        }

        bool isEqualCicle(string a, string b){

            string reversedCicle = invertString(a);

            if(a.size() == b.size()){
                for(int c = 0; c < a.size(); c++){
                    if((b.compare(rotateString(a, c)) == 0) ||
                        b.compare(rotateString(reversedCicle, c)) == 0){
                        return true;
                    }
                }
            }

            return false;

        }

        bool isInsertedCicle(string cicle){
            for(string str : cicles){
                if(isEqualCicle(cicle, str)) return true;
            }

            return false;
        }
        
        string getPathString(vector<int>path, int root){

            string resp = "";

            while(path.front() != root){
                path.erase(path.begin());
            }

            for(int index : path){
                resp += nodes[index].name + " ";
            }
            
            return resp;

        }

        void addCicle(vector<int>path, int root){

            string cicleStr = getPathString(path, root);

            if(!isInsertedCicle(cicleStr)){
                cicles.push_back(cicleStr);
            }

        }

        void writeCicles(){

            int counter = 1;

            for(string str : cicles){
                outputFile << "(" << counter++ << "): ";
                outputFile << str << endl;
            }
        }

        void clear(){
            nodes.clear();
            matrix.clear();
            cicles.clear();
            permutations.clear();
        }


};

int main(int argc, char const *argv[])
{ 
    Graph graph;
    string inputString;
    int count = 1;

    while (getline(inputFile, inputString)) {

        outputFile << "===================================\n";
        outputFile << "graph: " << count++ << endl;
        graph.set(inputString);
        graph.findCicles('W');
        graph.writeCicles();
    }

    outputFile << "===================================\n";

    inputFile.close();
    outputFile.close();
    return 0;
}
