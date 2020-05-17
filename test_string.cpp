#include <iostream>
#include <string>
#include <vector>
#include "radix_tree.h"

using namespace std;

radix_tree<string, int> tree;
radix_tree<string, int>::iterator it_radix;
vector<radix_tree<string, int>::iterator> vec;
vector<radix_tree<string, int>::iterator>::iterator it_vec;

void print_vec()
{
    for (it_vec = vec.begin(); it_vec != vec.end(); ++it_vec)
    {
        cout << (*it_vec)->first << endl;
    }
}

void print_tree()
{
    it_radix = tree.begin();
    for (; it_radix != tree.end(); ++it_radix)
    {
        cout << it_radix->first << ":" << it_radix->second << endl;
    }
}

void insert()
{
    tree["apache"] = 0;
    tree["afford"] = 1;
    tree["available"] = 2;
    tree["affair"] = 3;
    tree["avenger"] = 4;
    tree["binary"] = 5;
    tree["bind"] = 6;
    tree["brother"] = 7;
    tree["brace"] = 8;
    tree["blind"] = 9;
    tree["bro"] = 10;
}

void longest_match(string key)
{
    it_radix = tree.longest_match(key);
    cout << "longest_match(" << key << ")" << endl;
    if (it_radix == tree.end())
        cout << "failed" << endl;
    else
        cout << it_radix->first << endl;
}

void prefix_match(string key)
{
    tree.prefix_match(key, vec);
    cout << "prefix_match(" << key << ")" << endl;
    print_vec();
}

void greedy_match(string key)
{
    tree.greedy_match(key, vec);
    cout << "greedy_match(" << key << ")" << endl;
    print_vec();
}

int main(int argc, char const *argv[])
{
    insert();
    print_tree();

    longest_match("binder");
    longest_match("bracelet");
    longest_match("apple");

    prefix_match("aff");
    prefix_match("bi");
    prefix_match("a");

    greedy_match("avoid");
    greedy_match("bring");
    greedy_match("attack");

    tree.erase("bro");
    prefix_match("bro");
}