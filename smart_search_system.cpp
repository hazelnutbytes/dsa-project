#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
#include <queue>
#include <vector>
#include <algorithm> 
#include <set>       
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

unordered_map<string, int> getKeywordDepth(string start)
{
    queue<pair<string, int>> q;
    unordered_map<string, int> depth;

    q.push({start, 0});
    depth[start] = 0;

    while (!q.empty())
    {
        pair<string, int> p = q.front(); 
        q.pop();

        string current = p.first;        
        int d = p.second;              

        for (string next : keywordMap[current])
        {
            if (depth.find(next) == depth.end())
            {
                depth[next] = d + 1;
                q.push({next, d + 1});
            }
        }
    }

    return depth;
}

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
    unordered_map<int, int> score;

    //get all hierarchy levels
    auto keywordDepth = getKeywordDepth(keyword);

    for (auto &pair : keywordDepth)
    {
        string k = pair.first;
        int depth = pair.second;

        //weight decreases as depth increases
        int weight = max(1, 3 - depth); 
        // depth 0 → 3 points
        // depth 1 → 2 points
        // depth 2+ → 1 point

        if (indexMap.find(k) != indexMap.end())
        {
            for (int id : indexMap[k])
            {
                score[id] += weight;
            }
        }
    }

    if (score.empty())
    {
        cout << "No results\n";
        return;
    }

    vector<int> results;
    for (auto &p : score)
    {
        results.push_back(p.first);
    }

    sort(results.begin(), results.end(), [&](int a, int b)
    {
        // 🔥 combine score + clicks
        if (score[a] == score[b])
            return docs[a].clicks > docs[b].clicks;
        return score[a] > score[b];
    });

    for (int id : results)
    {
        cout << "Doc " << id
             << " (Score: " << score[id]
             << ", Clicks: " << docs[id].clicks << "): "
             << docs[id].content << endl;
    }
}

void openDocument(int id)
{
    docs[id].clicks++;
    cout << "Opened: " << docs[id].content << endl;
}

void findPath(int start, int target)
{
    queue<int> q;
    unordered_map<int, int> parent;

    q.push(start);
    parent[start] = -1;

    while (!q.empty())
    {
        int curr = q.front();
        q.pop();

        if (curr == target)
            break;

        for (int next : docs[curr].citations)
        {
            if (parent.find(next) == parent.end())
            {
                parent[next] = curr;
                q.push(next);
            }
        }
    }

    if (parent.find(target) == parent.end())
    {
        cout << "No path found\n";
        return;
    }

    cout << "Path: ";
    int curr = target;
    while (curr != -1)
    {
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

    //multi-level hierarchy
    keywordMap["AI"] = {"ML"};
    keywordMap["ML"] = {"DeepLearning"};
    keywordMap["DeepLearning"] = {"NeuralNetworks"};

    keywordMap["car"] = {"vehicle"};
    keywordMap["vehicle"] = {"automobile"};

    keywordMap["phone"] = {"mobile"};
    keywordMap["mobile"] = {"smartphone"};

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
    search("hello");   //normal keyword
    search("AI");      //hierarchy keyword test
    openDocument(1);

    findPath(1, 3);

    return 0;
}