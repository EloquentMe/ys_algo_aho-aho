#include <string>
#include <vector>
#include <map>
#include <queue>
#include <iostream>

using std::string;
using std::vector;
using std::map;
using std::queue;
using std::cin;
using std::cout;

class Aho
{
private:
    struct Node
    {
        int suffix_link = -1;
        int parent;
        map <char, int> next;
        int terminal_pattern = -1; //who ends here
        char last_char; //char to this node
    };
    int ROOT = 0;
    int cur_node;
    int pattern_count = 0;
    vector<Node> trie;
    vector <vector <int> > numeric_patterns;
    int NewNode();
    vector <int> AddAllPatterns(vector<string> const & patterns);
    int AddPattern(const string & pattern);
    void BuildSuffixLinks();
    void BuildSuffixLink(int);
    int Go(int, char);
public:
    Aho(vector <vector <string> > const & patterns);
    vector <vector <int> > & getNumeric();
    int processNextChar(char);
    void reset();
};

int Aho::NewNode()
{
    trie.push_back(Node());
    return trie.size() - 1;
}

vector <int> Aho::AddAllPatterns(vector<string> const & patterns)
{
    int sum_length = 1;
    for (int i = 0; i < patterns.size(); ++i)
        sum_length += patterns[i].length();
    trie.reserve(sum_length);
    NewNode();
    vector <int> numbers;
    for (int i = 0; i < patterns.size(); ++i)
        numbers.push_back(AddPattern(patterns[i]));
    return numbers;
}

int Aho::AddPattern(const string & pattern)
{
    int cur_node = ROOT;
    for (int i = 0; i < pattern.length(); ++i)
    {
        char c = pattern[i];
        if (trie[cur_node].next.count(c) != 0)
            cur_node = trie[cur_node].next[c];
        else
        {
            int new_node = NewNode();
            trie[new_node].parent = cur_node;
            trie[new_node].last_char = c;
            trie[cur_node].next[c] = new_node;
            cur_node = new_node;
        }
    }
    // -1 - we create new pattern
    // else it's an existing one
    if (trie[cur_node].terminal_pattern == -1)
    {
        trie[cur_node].terminal_pattern = pattern_count;
        ++pattern_count;
    }
    return trie[cur_node].terminal_pattern;
}

void Aho::BuildSuffixLinks()
{
    queue<int> q;
    q.push(ROOT);
    while(!q.empty())
    {
        int now = q.front();
        q.pop();
        BuildSuffixLink(now);

        for (map<char, int>::iterator iter = trie[now].next.begin();
                iter != trie[now].next.end();
                ++iter)
        {
            q.push(iter->second);
        }
    }
}

void Aho::BuildSuffixLink(int node)
{
    if (node == ROOT || trie[node].parent == ROOT)
    {
        trie[node].suffix_link = ROOT;
    }
    else
    {
        trie[node].suffix_link = Go(trie[trie[node].parent].suffix_link,
                                    trie[node].last_char);
    }
}

int Aho::Go(int node, char c)
{
    if (trie[node].next.count(c) != 0)
      return trie[node].next[c];
    if (node == ROOT)
      return ROOT;
    return Go(trie[node].suffix_link, c);
}

Aho::Aho(vector <vector <string> > const & patterns)
{
    for (int i = 0; i < patterns.size(); ++i)
    {
        numeric_patterns.push_back(AddAllPatterns(patterns[i]));
    }
    BuildSuffixLinks();
    cur_node = ROOT;
}

int Aho::processNextChar(char c)
{
    cur_node = Go(cur_node, c);
    return trie[cur_node].terminal_pattern;
}

void Aho::reset()
{
    cur_node = ROOT;
}

int main()
{
    int n, m;
    cin >> n >> m;
    vector <string> matrix;
    for (int i = 0; i < n; ++i)
    {
        string row;
        cin >> row;
        matrix.push_back(row);
    }

    int number, k, l;
    cin >> number >> k >> l;
    vector <vector <string> > patterns(number);
    for (int i = 0; i < number; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            string row;
            cin >> row;
            patterns[i].push_back(row);
        }
    }
    Aho aho(patterns);
    //vector <string> result(m); //_m_ sic!
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < matrix[i].length(); ++j)
            cout << aho.processNextChar(matrix[i][j]) << ' ';
            //result[j] += aho.processNextChar(matrix[i][j]);
        cout << std::endl;
        aho.reset();
    }
    return 0;
}
