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
#include <fstream>
using namespace std;

struct Document
{
    int id;
    string fileName;
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

queue<string> computeQueue;

void addToQueue(string filename) {
    computeQueue.push(filename);
}

unordered_map<string, vector<string>> keywordMap;

string readFileContent(const string &fileName)
{
    ifstream file(fileName);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void createFile(const string &fileName, const string &content)
{
    ofstream file(fileName);
    file << content;
    file.close();
}

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

void removeFromIndex(int docId)
{
    for (auto &pair : indexMap)
    {
        auto &vec = pair.second;
        vec.erase(remove(vec.begin(), vec.end(), docId), vec.end());
    }
}

void addDocument(int id, string fileName, string content)
{
    string fullName = to_string(id) + "_" + fileName + ".txt";

    createFile(fullName, content);

    Document doc;
    doc.id = id;
    doc.fileName = fullName;
    doc.content = content;

    docs[id] = doc;

    rollbacklog.push({"ADD_DOC", id, doc, "", ""});

    indexDocument(docs[id]);
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
        removeFromIndex(last.docId);
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
                if (docs.find(id) != docs.end())
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
        results.push_back(p.first);

    sort(results.begin(), results.end(), [&](int a, int b)
         {
        if (score[a] == score[b])
            return docs[a].clicks > docs[b].clicks;
        return score[a] > score[b]; });

    for (int id : results)
    {
        cout << "Doc " << id
             << " (Score: " << score[id]
             << ", Clicks: " << docs[id].clicks
             << ", File: " << docs[id].fileName << "): "
             << docs[id].content << endl;
    }
}

void openDocument(int id)
{
    if (docs.find(id) != docs.end())
    {
        docs[id].clicks++;
        cout << "Opened: " << docs[id].fileName << endl;
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

void userMenu()
{
    int choice;
    while (true)
    {
        cout << "\n1.Search\n2.Open Document\n3.Find Path\n4.Exit\nChoice: ";
        cin >> choice;

        if (choice == 1)
        {
            string key;
            cout << "Keyword: ";
            cin >> key;
            search(key);
        }
        else if (choice == 2)
        {
            int id;
            cout << "Doc ID: ";
            cin >> id;
            openDocument(id);
        }
        else if (choice == 3)
        {
            int a, b;
            cout << "Start and Target: ";
            cin >> a >> b;
            findPath(a, b);
        }
        else
            break;
    }
}

void adminMenu()
{
    int choice;
    while (true)
    {
        cout << "\n1.Add Document\n2.Add Keyword Relation\n3.Undo\n4.Exit\nChoice: ";
        cin >> choice;

        if (choice == 1)
        {
            int id;
            string name, content;

            cout << "ID: ";
            cin >> id;

            cout << "File Name: ";
            cin >> name;

            cin.ignore();
            cout << "Content: ";
            getline(cin, content);

            addDocument(id, name, content);
        }
        else if (choice == 2)
        {
            string a, b;
            cout << "Parent Child: ";
            cin >> a >> b;
            addKeywordRelation(a, b);
        }
        else if (choice == 3)
        {
            undo();
        }
        else
            break;
    }
}

int main()
{
    int role;
    while (true)
    {
        cout << "\n1.User\n2.Admin\n3.Exit\nChoice: ";
        cin >> role;

        if (role == 1)
            userMenu();
        else if (role == 2)
            adminMenu();
        else
            break;
    }
    return 0;
}