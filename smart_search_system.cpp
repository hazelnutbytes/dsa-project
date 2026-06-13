#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
#include <queue>
#include <vector>
#include <algorithm>
#include <set>
#include <stack>
using namespace std;

struct Document
{
    int id;
    string content;
    int clicks = 0;
    vector<int> citations;
};

struct Action
{
    string type;
    int docId;
    Document backup;
    string key;
    string related;
};

stack<Action> rollbacklog;

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

void indexDocument(Document &doc)
{
    stringstream ss(doc.content);
    string word;

    while (ss >> word)
    {
        indexMap[word].push_back(doc.id);
    }
}

void addDocument(Document doc)
{
    docs[doc.id] = doc;
    rollbacklog.push({"ADD_DOC", doc.id, doc, "", ""});
    indexDocument(docs[doc.id]);
}

void addKeywordRelation(string a, string b)
{
    keywordMap[a].push_back(b);
    rollbacklog.push({"ADD_KEYWORD", -1, {}, a, b});
}

void undo()
{
    if (rollbacklog.empty())
        return;

    Action last = rollbacklog.top();
    rollbacklog.pop();

    if (last.type == "ADD_DOC")
    {
        docs.erase(last.docId);
    }
    else if (last.type == "ADD_KEYWORD")
    {
        auto &vec = keywordMap[last.key];
        vec.erase(remove(vec.begin(), vec.end(), last.related), vec.end());
    }
}

void search(string keyword)
{
    unordered_map<int, int> score;

    auto keywordDepth = getKeywordDepth(keyword);

    for (auto &pair : keywordDepth)
    {
        string k = pair.first;
        int depth = pair.second;

        int weight = max(1, 3 - depth);

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
        if (score[a] == score[b])
            return docs[a].clicks > docs[b].clicks;
        return score[a] > score[b]; });

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
    if (docs.find(id) != docs.end())
    {
        docs[id].clicks++;
        cout << "Opened: " << docs[id].content << endl;
    }
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
    Document d1;
    d1.id = 1;
    d1.content = "hello world";

    Document d2;
    d2.id = 2;
    d2.content = "c++ is fun";

    Document d3;
    d3.id = 3;
    d3.content = "search engine project";

    addDocument(d1);
    addDocument(d2);
    addDocument(d3);

    docs[1].citations.push_back(2);
    docs[2].citations.push_back(3);

    addKeywordRelation("AI", "ML");
    addKeywordRelation("ML", "DeepLearning");
    addKeywordRelation("DeepLearning", "NeuralNetworks");

    addKeywordRelation("car", "vehicle");
    addKeywordRelation("vehicle", "automobile");

    addKeywordRelation("phone", "mobile");
    addKeywordRelation("mobile", "smartphone");

    for (auto &pair : docs)
    {
        cout << pair.first << " -> " << pair.second.content << endl;
    }

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
    search("AI");

    findPath(1, 3);

    undo();

    return 0;
}