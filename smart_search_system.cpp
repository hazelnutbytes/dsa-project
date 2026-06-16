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
    string signature;
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

vector<map<string, vector<int>>> storageNodes(3);

int getNodeIndex(const string &key)
{
    hash<string> h;
    return h(key) % storageNodes.size();
}

unordered_map<string, vector<string>> keywordMap;

void indexDocument(Document &doc);

queue<string> computeQueue;

void addToQueue(string filename)
{
    computeQueue.push(filename);
}

string generateSignature(const string &content)
{
    hash<string> h;
    return to_string(h(content));
}

bool verifySignature(const Document &doc)
{
    return doc.signature == generateSignature(doc.content);
}

void processQueue()
{
    while (!computeQueue.empty())
    {
        string filename = computeQueue.front();
        computeQueue.pop();

        ifstream file(filename);
        string line;
        string fullContent;

        while (getline(file, line))
        {
            fullContent += line + " ";
        }

        file.close();

        int docId = -1;

        auto pos = filename.find('_');
        if (pos != string::npos)
        {
            docId = stoi(filename.substr(0, pos));
        }

        Document temp;
        temp.id = docId;
        temp.fileName = filename;
        temp.content = fullContent;
        temp.signature = generateSignature(fullContent);

        docs[temp.id] = temp;
        indexDocument(docs[temp.id]);
    }
}

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
        int node = getNodeIndex(word);
        storageNodes[node][word].push_back(doc.id);
    }
}

void removeFromIndex(int docId)
{
    for (auto &node : storageNodes)
    {
        for (auto &pair : node)
        {
            auto &vec = pair.second;
            vec.erase(remove(vec.begin(), vec.end(), docId), vec.end());
        }
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
    doc.signature = generateSignature(content);

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

int countKeywordFrequency(const string &content, const string &keyword)
{
    stringstream ss(content);
    string word;
    int count = 0;

    while (ss >> word)
    {
        if (word == keyword)
            count++;
    }

    return count;
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

        int node = getNodeIndex(k);

        if (storageNodes[node].find(k) != storageNodes[node].end())
        {
            for (int id : storageNodes[node][k])
            {
                if (docs.find(id) != docs.end())
                {
                    int freq = countKeywordFrequency(docs[id].content, k);
                    score[id] += weight * freq;
                }
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

        cout << "Signature valid: "
             << (verifySignature(docs[id]) ? "YES" : "NO") << endl;

        cout << "Keyword Frequency: "
             << countKeywordFrequency(docs[id].content, keyword)
             << endl;
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
    //Check if docs exist
    if (docs.find(start) == docs.end() || docs.find(target) == docs.end())
    {
        cout << "Invalid document ID\n";
        return;
    }

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

        if (docs.find(curr) == docs.end())
            continue;

        for (int next : docs[curr].citations)
        {
            if (parent.find(next) == parent.end())
            {
                parent[next] = curr;
                q.push(next);
            }
        }
    }

    //no path
    if (parent.find(target) == parent.end())
    {
        cout << "No path found\n";
        return;
    }

    vector<int> path;
    int curr = target;

    while (curr != -1)
    {
        path.push_back(curr);

        if (parent.find(curr) == parent.end())
            break;

        curr = parent[curr];
    }

    reverse(path.begin(), path.end());

    cout << "Path: ";
    for (int node : path)
    {
        cout << node << " ";
    }
    cout << endl;
}

void addCitation(int from, int to)
{
    if (docs.find(from) != docs.end() && docs.find(to) != docs.end())
    {
        docs[from].citations.push_back(to);
    }
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
        cout << "\n1.Add Document\n2.Add Keyword Relation\n3.Undo\n4.Process Queue\n5.Add citation\n6.Exit\nChoice: ";
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
        else if (choice == 4)
        {
            processQueue();
        }
        else if (choice == 5)
        {
            int a, b;
            cout << "From Doc -> To Doc: ";
            cin >> a >> b;
            addCitation(a, b);
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