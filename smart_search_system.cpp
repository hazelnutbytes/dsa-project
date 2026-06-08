#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
using namespace std;

struct Document {
    int id;
    string content;
};

unordered_map<int, Document> docs;

map<string, vector<int>> indexMap;

void indexDocument(Document &doc) {
    stringstream ss(doc.content);
    string word;
    
    while (ss >> word) {
        indexMap[word].push_back(doc.id);
    }
}

int main() {
    Document d1 = {1, "hello world"};
    Document d2 = {2, "c++ is fun"};
    Document d3 = {3, "search engine project"};

    docs[d1.id] = d1;
    docs[d2.id] = d2;
    docs[d3.id] = d3;

    for (auto &pair : docs) {
        cout << pair.first << " -> " << pair.second.content << endl;
    }

    indexDocument(d1);
    indexDocument(d2);
    indexDocument(d3);

    for (auto &pair : indexMap) {
    cout << pair.first << ":";
    for (int id : pair.second) {
        cout << id << " ";
    }
    cout << endl;
}

    return 0;
}