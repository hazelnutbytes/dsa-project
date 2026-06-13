#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
using namespace std;

struct Document
{
    int id;
    string content;
    int clicks = 0;
};

unordered_map<int, Document> docs;

map<string, vector<int>> indexMap;

// indexing logic
void indexDocument(Document &doc)
{
    stringstream ss(doc.content);
    string word;

    while (ss >> word)
    {
        indexMap[word].push_back(doc.id);
    }
}

void search(string keyword)
{
    if (indexMap.find(keyword) == indexMap.end())
    {
        cout << "No results\n";
        return;
    }

    vector<int> results = indexMap[keyword];

    sort(results.begin(), results.end(), [](int a, int b) {
        return docs[a].clicks > docs[b].clicks;
    });

    for (int id : results)
    {
        cout << "Doc" << id << " (Clicks: " << docs[id].clicks << ":" << docs[id].content << endl;
    }
}

void openDocument(int id) {
    docs[id].clicks++;
    cout << "Opened: " << docs[id].content << endl;
}

int main()
{
    Document d1 = {1, "hello world"};
    Document d2 = {2, "c++ is fun"};
    Document d3 = {3, "search engine project"};

    docs[d1.id] = d1;
    docs[d2.id] = d2;
    docs[d3.id] = d3;

    for (auto &pair : docs)
    {
        cout << pair.first << " -> " << pair.second.content << endl;
    }

    indexDocument(d1);
    indexDocument(d2);
    indexDocument(d3);

    for (auto &pair : indexMap)
    {
        cout << pair.first << ":";
        for (int id : pair.second)
        {
            cout << id << " ";
        }
        cout << endl;
    }
    openDocument(1);
    search("hello");
    openDocument(1);

    return 0;
}