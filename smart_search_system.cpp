#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

struct Document {
    int id;
    string content;
};

unordered_map<int, Document> docs;

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
    
    return 0;
}