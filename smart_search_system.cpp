#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
#include <queue>
using namespace std;

struct Document
{
    int id;
    string content;
    int clicks = 0;
    vector<int> citations;
};

unordered_map<int, Document> docs;

map<string, vector<int>> indexMap;

unordered_map<string, vector<string>> keywordMap;

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
    vector<int> results;

    //main keyword results
    if(indexMap.find(keyword) != indexMap.end()) {
        for (int id : indexMap[keyword]) {
            results.push_back(id);
        }
    }

    //related keyword results
    if(keywordMap.find(keyword) != keywordMap.end()) {
        for (string related : keywordMap[keyword]) {
            if (indexMap.find(related) != indexMap.end()) {
                for (int id : indexMap[related]) {
                    results.push_back(id);
                }
            }
        }
    }

    if (results.empty()) {
        cout << "No results\n";
        return;
    }
    
    //remove duplicates
    sort(results.begin(), results.end());
    results.erase(unique(results.begin(), results.end()), results.end());

    sort(results.begin(), results.end(), [](int a, int b) {
        return docs[a].clicks > docs[b].clicks;
    });

    for (int id : results)
    {
        cout << "Doc " << id << ": " << docs[id].content << endl;
    }
}

void openDocument(int id) {
    docs[id].clicks++;
    cout << "Opened: " << docs[id].content << endl;
}

void findPath(int start, int target) {
    queue<int> q;
    unordered_map<int, int> parent;
    
    q.push(start);
    parent[start] = -1;

    while(!q.empty()) {
        int curr = q.front();
        q.pop();

        if(curr == target) break;

        for (int next : docs[curr].citations) {
            if (parent.find(next) == parent.end()) {
                parent[next] = curr;
                q.push(next);
            }
        }
    } 

    if(parent.find(target) == parent.end()) {
        cout << "No path found\n";
        return;
    }

    cout << "Path: ";
    int curr = target;
    while (curr != -1) {
        cout << curr << " ";
        curr = parent[curr];
    }
    cout << endl;
}

int main()
{
    Document d1 = {1, "hello world"};
    Document d2 = {2, "c++ is fun"};
    Document d3 = {3, "search engine project"};

    docs[d1.id] = d1;
    docs[d2.id] = d2;
    docs[d3.id] = d3;

    docs[1].citations.push_back(2);
    docs[2].citations.push_back(3);

    keywordMap["car"] = {"vehicle", "automobile"};
    keywordMap["phone"] = {"mobile", "smartphone"};

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

    findPath(1, 3);
    return 0;
}