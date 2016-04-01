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

template <class T> class Aho
{
private:
    struct Node
    {
        int suffix_link = -1;
        int parent;
        map <T, int> next;
        int terminal_pattern = -1; //who ends here
        T last_char; //char to this node
    };
    int ROOT = 0;
    int cur_node;
    int pattern_count = 0;
    vector<Node> trie;
    vector <vector <int> > numeric_patterns;
    int NewNode();
    vector <int> AddAllPatterns(vector <vector<T> > const & patterns);
    int AddPattern(const vector<T> & pattern);
    void BuildSuffixLinks();
    void BuildSuffixLink(int);
    int Go(int, T);
public:
    Aho(vector <vector <vector<T> > > const & patterns);
    vector <vector <int> > & getNumeric();
    int processNextChar(T);
    void reset();
};

template <class T> int Aho<T>::NewNode()
{
    trie.push_back(Node());
    return trie.size() - 1;
}

template <class T> vector <int> Aho<T>::AddAllPatterns(vector <vector<T> > const & patterns)
{
    int sum_length = 1;
    for (int i = 0; i < patterns.size(); ++i)
        sum_length += patterns[i].size();
    trie.reserve(sum_length);
    NewNode();
    vector <int> numbers;
    for (int i = 0; i < patterns.size(); ++i)
        numbers.push_back(AddPattern(patterns[i]));
    return numbers;
}

template <class T> int Aho<T>::AddPattern(const vector<T> & pattern)
{
    int cur_node = ROOT;
    for (int i = 0; i < pattern.size(); ++i)
    {
        T c = pattern[i];
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

template <class T> void Aho<T>::BuildSuffixLinks()
{
    queue <int> q;
    q.push(ROOT);
    while(!q.empty())
    {
        int now = q.front();
        q.pop();
        BuildSuffixLink(now);

        for (typename map <T, int>::iterator iter = trie[now].next.begin();
                iter != trie[now].next.end();
                ++iter)
        {
            q.push(iter->second);
        }
    }
}

template <class T> void Aho<T>::BuildSuffixLink(int node)
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

template <class T> int Aho<T>::Go(int node, T c)
{
    if (trie[node].next.count(c) != 0)
      return trie[node].next[c];
    if (node == ROOT)
      return ROOT;
    return Go(trie[node].suffix_link, c);
}

template <class T> Aho<T>::Aho(vector <vector <vector<T> > > const & patterns)
{
    for (int i = 0; i < patterns.size(); ++i)
    {
        numeric_patterns.push_back(AddAllPatterns(patterns[i]));
    }
    BuildSuffixLinks();
    cur_node = ROOT;
}

template <class T> int Aho<T>::processNextChar(T c)
{
    cur_node = Go(cur_node, c);
    return trie[cur_node].terminal_pattern;
}

template <class T> void Aho<T>::reset()
{
    cur_node = ROOT;
}

template <class T> vector <vector <int> > & Aho<T>::getNumeric()
{
    return numeric_patterns;
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
    vector <vector <vector <char> > > patterns_str(number);
    for (int i = 0; i < number; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            vector <char> row;
            for (int h = 0; h < l; ++h)
            {
                char c;
                cin >> c;
                row.push_back(c);
            }
            patterns_str[i].push_back(row);
        }
    }
    Aho<char> aho_1(patterns_str);
    vector <vector <vector <int>>> patterns_int; 
    patterns_int.push_back(aho_1.getNumeric());
    Aho<int> aho_2(patterns_int);

    vector <string> result(m); //_m_ sic!
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
            //cout << aho.processNextChar(matrix[i][j]) << ' ';
            result[j] += aho_1.processNextChar(matrix[i][j]);
        //cout << std::endl;
        aho_1.reset();
    }

    vector<int> counts(number, 0);
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            int pattern_id = aho_2.processNextChar(result[i][j]);
            if (pattern_id != -1)
                counts[pattern_id]++;
        }
        aho_2.reset();
    }
    
    vector<int> matrix_index = aho_2.getNumeric()[0];
    for (int i = 0; i < matrix_index.size(); ++i) //size must be equal to 'number'
    {
        cout << counts[matrix_index[i]] << ' ';
    }
    cout << std::endl;
    return 0;
}
