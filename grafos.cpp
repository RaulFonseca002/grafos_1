#include <iostream>
#include <regex>
#include <string>
#include <queue>
#include <vector> 
#include <fstream>
#include <chrono>

using namespace std;

ifstream inputFile("input.txt");
ofstream outputFile("output.txt");
vector<string> permutations;

typedef struct Node
{
    string name;
    int index;

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

        bool canReach(string root, string end){

            int indexRoot = getNodeIndex(root);
            int indexEnd = getNodeIndex(end);

            if(indexRoot != indexEnd){
                return (matrix[indexRoot][indexEnd]);
            }

            return false;
        }

        bool canReach(vector<int>path, int end){
            return (matrix[path.front()][end]);
        }

        bool isCircle(string path){
            return (    (path.size() > 4) && 
                        canReach(path.substr(0,1), path.substr((path.length()-2),1)));
        }

        void findCicles(char mode){

            this->mode = mode;

            for (Node n : nodes){
                findCicles(n.name);
            }

            if(mode == 'P'){
                for(string s : permutations){
                    if(isCircle(s)) addCicle(s);             
                }            
            }
        }

        void findCicles(string nodeName) {

            int index = getNodeIndex(nodeName);
            vector<int> path;
            vector<bool> founded(nodes.size(), false);

            findCicles(index, path, founded, index);

        }

        void findCicles(int index, vector<int> path, vector<bool> founded, const int root) {

            if(founded[index]){
                return;
            }

            path.push_back(index);
            founded[index] = true;

            if(mode == 'P') permutations.push_back(getPathString(path, root));

            for (int c = 0; c < nodes.size(); c++) {
                
                if (matrix[c][index]) {

                    if(c == root){
                        if(mode == 'W')
                            addCicle(getPathString(path, root));
                    }else{
                        findCicles(c, path, founded, root);
                    }

                    
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

        void addCicle(string path){

            if(isCircle(path) && !isInsertedCicle(path)){
                cicles.push_back(path);
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

        void clearCicles(){
            cicles.clear();
        }


};

int main(int argc, char const *argv[])
{ 
    Graph graph;
    string inputString;
    int count = 1;

    chrono::steady_clock::time_point begin, end;
    int64_t P_time;
    int64_t W_time;

    while (getline(inputFile, inputString)) {
        outputFile << "===================================\n";
        outputFile << "graph: " << count++ << endl;

        graph.set(inputString);

        begin = chrono::steady_clock::now();
        graph.findCicles('P');
        end = chrono::steady_clock::now();
        P_time =  chrono::duration_cast<chrono::microseconds>(end - begin).count();
        
        graph.clearCicles();

        begin = chrono::steady_clock::now();
        graph.findCicles('W');
        end = chrono::steady_clock::now();
        W_time =  chrono::duration_cast<chrono::microseconds>(end - begin).count();

        outputFile << "permutation time: " << P_time << endl;
        outputFile << "walk time: " << W_time << endl;

        graph.writeCicles();
        graph.clear();
    }

    outputFile << "===================================\n";

    inputFile.close();
    outputFile.close();
    return 0;
}
