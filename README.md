# SemanticFind – Semantic Text Document Search Router

## 2.1 Project Title

**SemanticFind: Semantic Text Document Search Router**

---

## 2.2 Problem Statement

SemanticFind is an enterprise-level document search system designed to efficiently organize, index, and retrieve documents based on semantic keyword relationships. Traditional systems suffer from slow keyword hierarchy traversal, lack of rollback mechanisms, unordered indexing, inefficient document verification, and no structured citation tracking.

This project addresses these challenges by implementing a fast, structured, and scalable system using advanced data structures in C++.

---

## 2.3 Objectives

* Build a hierarchical keyword mapping system for semantic search
* Enable rollback functionality for configuration changes
* Ensure strict FIFO processing of documents
* Implement fast document verification using digital signatures
* Rank search results based on keyword frequency
* Model document relationships using citation graphs
* Find shortest paths between documents
* Simulate distributed storage across cloud nodes

---

## 2.4 System Overview / Architecture

The system consists of multiple interconnected modules:

* **Document Storage** → Stores all documents with metadata
* **Keyword Hierarchy Graph** → Represents semantic keyword relationships
* **Indexing Engine** → Maps keywords to documents
* **Rollback System** → Tracks and reverses changes using a stack
* **Compute Queue** → Ensures ordered document processing
* **Signature System** → Verifies document integrity
* **Search Engine** → Ranks results using weighted scoring
* **Citation Graph** → Models relationships between documents
* **Storage Balancer** → Distributes index across multiple nodes

---

## 2.5 Data Structures and Algorithms Used

| Feature                | Data Structure / Algorithm                   |
| ---------------------- | -------------------------------------------- |
| Document Storage       | `unordered_map<int, Document>`               |
| Keyword Indexing       | `map<string, vector<int>>`                   |
| Distributed Storage    | `vector<map<string, vector<int>>>`           |
| Keyword Hierarchy      | Graph (Adjacency List using `unordered_map`) |
| Rollback System        | `stack<Action>`                              |
| Compute Queue          | `queue<string>`                              |
| Search Ranking         | Sorting + Hashing                            |
| Signature Verification | Hashing                                      |
| Citation Network       | Graph                                        |
| Pathfinding            | BFS (Breadth First Search)                   |

---

## 2.6 Implementation Approach

1. **Document Addition**

   * Documents are stored and indexed immediately
   * Signature generated using hashing

2. **Keyword Indexing**

   * Words extracted and mapped to storage nodes
   * Load distributed using hashing

3. **Search**

   * BFS used on keyword hierarchy
   * Weighted scoring based on keyword depth
   * Results sorted by score and clicks

4. **Rollback**

   * Every action stored in stack
   * Undo reverses last operation

5. **Queue Processing**

   * Files processed in FIFO order
   * Ensures consistent indexing

6. **Citation Mapping**

   * Documents linked using graph structure
   * BFS used for shortest path

---

## 2.7 Time and Space Complexity Analysis

| Operation              | Time Complexity    | Space Complexity |
| ---------------------- | ------------------ | ---------------- |
| Add Document           | O(n)               | O(n)             |
| Search                 | O(k * d + r log r) | O(r)             |
| Indexing               | O(n)               | O(n)             |
| BFS (Pathfinding)      | O(V + E)           | O(V)             |
| Rollback               | O(1)               | O(n)             |
| Signature Verification | O(n)               | O(1)             |

Where:

* n = number of words
* k = keywords
* d = depth
* r = results

---

## 2.8 Execution Steps

1. Compile the code:

```
g++ main.cpp -o semanticfind
```

2. Run the program:

```
./semanticfind
```

3. Choose role:

```
1. User
2. Admin
```

4. Admin can:

* Add documents
* Add keyword relations
* Undo operations
* Process queue
* Add citations

5. User can:

* Search documents
* Open documents
* Find citation paths

---

## 2.9 Sample Inputs and Outputs

### Input

```
Add Document:
ID: 1
File Name: sample
Content: data structures and algorithms
```

### Search

```
Keyword: data
```

### Output

```
Doc 1 (Score: 3, Clicks: 0, File: 1_sample.txt)
Signature valid: YES
Keyword Frequency: 1
```

---

## 2.10 Screenshots

* Menu interface
  
  <img width="622" height="354" alt="Screenshot 2026-06-17 at 7 56 10 AM" src="https://github.com/user-attachments/assets/216d0135-1e7b-4274-b33b-df6b0b878636" />
  <img width="537" height="396" alt="Screenshot 2026-06-17 at 7 56 24 AM" src="https://github.com/user-attachments/assets/6a30b506-c4c1-480b-8326-afbbb16595c2" />
* Search results
  <img width="928" height="539" alt="Screenshot 2026-06-17 at 7 56 41 AM" src="https://github.com/user-attachments/assets/47e17335-3bce-463c-9bce-5bf9822cfee3" />

* Pathfinding output
  <img width="283" height="162" alt="Screenshot 2026-06-17 at 7 56 56 AM" src="https://github.com/user-attachments/assets/4f2e8a8c-190e-4fc7-8fc9-f139e221ab92" />
---

## 2.11 Results and Observations

* Keyword hierarchy significantly improves search relevance
* BFS ensures efficient traversal and shortest path detection
* Hash-based indexing provides fast lookup
* Rollback mechanism ensures system reliability
* Distributed storage improves scalability

---

## 2.12 Conclusion

SemanticFind successfully demonstrates how advanced data structures can be used to build a scalable and efficient semantic search system. The project integrates concepts like graphs, hashing, queues, stacks, and sorting to simulate real-world search engines like Elasticsearch and Apache Lucene.

This system can be further enhanced with:

* GUI interface
* Real-time indexing
* Machine learning-based ranking
* Distributed cloud deployment

---

**Author:** Hazel
**GitHub:** hazelnutbytes
