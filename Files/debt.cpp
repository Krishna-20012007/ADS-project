#include <bits/stdc++.h>
using namespace std;


struct TrieNode {
    map<char, TrieNode*> children;
    bool end = false;
};

class Trie {
public:
    TrieNode* root = new TrieNode();

    void insert(string word) {
        TrieNode* node = root;
        for(char c : word) {
            if (!node->children[c])
                node->children[c] = new TrieNode();
            node = node->children[c];
        }
        node->end = true;
    }

    vector<string> searchPrefix(string prefix) {
        TrieNode* node = root;

        for(char c : prefix) {
            if (!node->children[c])
                return {};
            node = node->children[c];
        }

        vector<string> result;

        function<void(TrieNode*, string)> dfs = [&](TrieNode* n, string path) {
            if(n->end) result.push_back(path);

            for(auto &p : n->children)
                dfs(p.second, path + p.first);
        };

        dfs(node, prefix);
        return result;
    }
};


int main() {
    int mode;
    cin >> mode;  

    int n;
    cin >> n;

    vector<string> users(n);
    Trie trie;

    for(int i = 0; i < n; i++) {
        cin >> users[i];
        trie.insert(users[i]);
    }

    int m;
    cin >> m;

    vector<vector<double>> graph(n, vector<double>(n, 0));

    for(int i = 0; i < m; i++) {
        int d,c;
        double a;
        cin >> d >> c >> a;
        graph[d][c] += a;
    }

    
    if(mode == 1) {
        string prefix;
        cin >> prefix;

        auto res = trie.searchPrefix(prefix);

        for(auto &name : res)
            cout << name << endl;

        return 0;
    }

    
    vector<double> bal(n,0);

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            bal[i]-=graph[i][j];
            bal[j]+=graph[i][j];
        }
    }

    priority_queue<pair<double,int>> maxh;
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<>> minh;

    for(int i=0;i<n;i++){
        if(bal[i]>0) maxh.push({bal[i],i});
        else if(bal[i]<0) minh.push({bal[i],i});
    }

    while(!maxh.empty() && !minh.empty()){
        auto [c_amt,c] = maxh.top(); maxh.pop();
        auto [d_amt,d] = minh.top(); minh.pop();

        double amt = min(c_amt, -d_amt);

        cout << users[d] << " " << users[c] << " " << amt << endl;

        c_amt -= amt;
        d_amt += amt;

        if(c_amt>0) maxh.push({c_amt,c});
        if(d_amt<0) minh.push({d_amt,d});
    }
}