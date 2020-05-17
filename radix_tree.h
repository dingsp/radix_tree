#ifndef RADIX_TREE
#define RADIX_TREE

#include <string>
#include <vector>
#include <cassert>
#include "radix_tree_it.h"
#include "radix_tree_node.h"

/**
 * 获取K的子序列
 * @par begin 复制到子序列的起始下标
 * 如果等于key的长度，返回空元素。
 * 如果大于key的长度，抛出异常。
 * @par num 复制到子序列的元素个数
 */
template <typename K>
K radix_substr(const K &key, int begin, int num);

template <>
inline std::string radix_substr<std::string>(const std::string &key, int begin, int num)
{
    return key.substr(begin, num);
}

template <typename K>
K radix_join(const K &key1, const K &key2);

template <>
inline std::string radix_join<std::string>(const std::string &key1, const std::string &key2)
{
    return key1 + key2;
}

template <typename K>
int radix_length(const K &key);

template <>
inline int radix_length<std::string>(const std::string &key)
{
    return key.size();
}

template <typename K, typename T>
class radix_tree
{
public:
    typedef K key_type;
    typedef T mapped_type;
    typedef std::pair<const K, T> value_type;
    typedef radix_tree_it<K, T> iterator;
    typedef std::size_t size_type;

    //构造函数
    radix_tree() : m_size(0), m_root(NULL) {}
    ~radix_tree()
    {
        delete m_root;
    }

    //成员函数
    /**
     * @brief 获取基数树元素个数
     */
    size_type size() const
    {
        return m_size;
    }

    /**
     * @brief 判断基数树是否为空
     */
    bool empty() const
    {
        return m_size == 0;
    }

    /**
     * @brief 清除基数树所占的内存
     */
    void clear()
    {
        delete m_root;
        m_root = NULL;
        m_size = 0;
    }

    /**
     * @brief 返回树中首个元素的迭代器，树为空时返回空的迭代器
     */
    iterator begin();

    /**
     * @brief 返回空的迭代器
     */
    iterator end();

    /**
     * @brief 根据key返回可以完全匹配序列（叶子节点）的迭代器，若无完全匹配的结果则返回空的迭代器
     */
    iterator find(const K &key);

    /**
     * @brief 寻找树中叶子节点，该叶子节点能够最长前缀匹配key，若没有返回空迭代器
     * @note 匹配结果长度小于key
     */
    iterator longest_match(const K &key);

    /**
     * @brief 在树中寻找key能够前缀匹配的所有叶子节点
     * @note 匹配结果长度大于key
     * @par key 为空匹配树中所有元素
     * @par vec 树的根节点为空或找不到序列，返回元素数量为空的vec
     */
    void prefix_match(const K &key, std::vector<iterator> &vec);

    /**
     * @brief 在树中寻找与key拥有公共前缀匹配的所有叶子节点
     * @par key 为空匹配树中所有元素
     * @par vec 树的根节点为空或找不到序列，返回元素数量为空的vec
     */
    void greedy_match(const K &key, std::vector<iterator> &vec);

    /**
     * @brief 在树中插入节点，如果根节点为空，新建根节点。
     * 插入成功返回最终新建的叶子节点和true
     */
    std::pair<iterator, bool> insert(const value_type &val);

    /**
     * @brief 用于向基数树中插入键值对
     * @return 要插入的叶子节点内部的T&，用于给插入节点中的pair<K,T>类型中的T赋值
     */
    T &operator[](const K &key);

    /**
     *  @brief 删除元素，如果it为空迭代器直接返回
     */
    void erase(iterator it);

    /**
     *  @brief 删除序列，如果树中不存在此序列返回false
     */
    bool erase(const K &key);

private:
    size_type m_size;
    radix_tree_node<K, T> *m_root;

    /**
     * @brief 在以node为根节点的树中查找key的最长前缀匹配序列对应节点
     * @par key 要匹配的序列，可以为空。
     * @par node 被匹配的树的根节点，需要非空，为空时出现异常。
     * @par matched 已经匹配的key的子序列长度，需要大于等于零，小于零时行为未定义。
     * @return 返回两种匹配情况
     *  -完全匹配：树中叶子节点
     *  -最长前缀匹配：树中非叶子节点
     */
    static radix_tree_node<K, T> *get_longest_prefix_node(const K &key, radix_tree_node<K, T> *node, int matched);

    /**
     * @brief 将node为根节的树中的所有叶子节点添加到vec中
     * @par node 给定树的根节点，需要非空，若为空则出现异常。
     * @par vec 包含结果的集合，需要非空，若为空则出现异常。
     */
    static void get_leafs(radix_tree_node<K, T> *node, std::vector<iterator> &vec);

    /**
     * @brief 获取node节点为根的子树中的首个叶子节点
     * @par node 需要非空，为空则出现异常
     */
    static radix_tree_node<K, T> *begin(radix_tree_node<K, T> *node);

    /**
     * @brief 为node节点添加子序列节点
     */
    static radix_tree_node<K, T> *add_child(radix_tree_node<K, T> *root, const value_type &val);

    /**
     * @brief 为node节点添加叶子序列节点
     */
    static radix_tree_node<K, T> *add_leaf(radix_tree_node<K, T> *node, const value_type &val);

    /**
     * @brief 将node和val的公共前缀序列提取出来作为新的node节点，将node和val序列剩余部分作为新节点的子节点
     * @note node节点是val序列在树中的最长前缀匹配节点，存在与val相同的前缀、不同的后缀
     */
    static radix_tree_node<K, T> *add_root(radix_tree_node<K, T> *node, const value_type &val);

    /**
     *  @brief 删除node节点，有可能造成兄弟节点与父节点的合并
     *  @note 删除一个节点有可能改变其父节点为根的基数树的性质，需要对其进行维护
     *  @par key 树中存储的序列
     */
    void erase(radix_tree_node<K, T> *node);

    /**
     * @brief 内部节点node合并其父节点
     * @par node为父节点唯一内部节点
     */
    void merge_node(radix_tree_node<K, T> *node);
};

template <typename K, typename T>
void radix_tree<K, T>::greedy_match(const K &key, std::vector<iterator> &vec)
{
    vec.clear();
    if (m_root == NULL)
        return;
    radix_tree_node<K, T> *node = get_longest_prefix_node(key, m_root, 0);
    if (node->m_is_leaf)
        node = node->m_parent;
    get_leafs(node, vec);
}

template <typename K, typename T>
void radix_tree<K, T>::prefix_match(const K &key, std::vector<iterator> &vec)
{
    vec.clear();
    if (m_root == NULL)
        return;
    radix_tree_node<K, T> *node = get_longest_prefix_node(key, m_root, 0);
    if (node->m_is_leaf)
        node = node->m_parent;
    int len_prefix = radix_length(key) - node->m_depth;
    K key_sub = radix_substr(key, node->m_depth, len_prefix);
    K node_key_sub = radix_substr(node->m_key, 0, len_prefix);
    if (key_sub != node_key_sub)
        return;
    get_leafs(node, vec);
}

template <typename K, typename T>
typename radix_tree<K, T>::iterator radix_tree<K, T>::longest_match(const K &key)
{
    if (m_root == NULL)
        return iterator(NULL);
    radix_tree_node<K, T> *node = get_longest_prefix_node(key, m_root, 0);
    if (node->m_is_leaf)
        return iterator(node);

    bool node_has_diff = !(node->m_key == radix_substr(key, node->m_depth, radix_length(node->m_key)));
    //node父节点与key一定没有不同后缀
    if (node_has_diff)
    {
        node = node->m_parent;
        node_has_diff = !(node->m_key == radix_substr(key, node->m_depth, radix_length(node->m_key)));
        assert(node_has_diff == false);
    }

    K nul = radix_substr(key, 0, 0);
    while (node != NULL)
    {
        typename radix_tree_node<K, T>::iterator_child it = node->m_children.find(nul);
        if (it != node->m_children.end())
        {
            assert(it->second->m_is_leaf == true);
            assert(radix_substr(key, node->m_depth, radix_length(node->m_key)) == node->m_key);
            return iterator(it->second);
        }
        node = node->m_parent;
    }

    return iterator(NULL);
}

template <typename K, typename T>
void radix_tree<K, T>::merge_node(radix_tree_node<K, T> *node)
{
    if (node == NULL)
        return;
    radix_tree_node<K, T> *parent = node->m_parent;
    if (!node->m_is_leaf || parent == m_root || parent->m_children.size() != 1)
        return;

    //重构node节点
    node->m_key = radix_join(node->m_key, parent->m_key);
    node->m_depth = parent->m_depth;
    node->m_parent = parent->m_parent;

    //删除node父节点
    radix_tree_node<K, T> *grandparent = parent->m_parent;
    typename radix_tree_node<K, T>::iterator_child it = grandparent->m_children.find(parent->m_key);
    grandparent->m_children.erase(it);
    delete parent;
}

template <typename K, typename T>
void radix_tree<K, T>::erase(iterator it)
{
    if (it == end())
        return;

    radix_tree_node<K, T> *node = find(it->first, m_root, 0);
    assert(node->m_is_leaf == true);
    erase(node);
}

template <typename K, typename T>
bool radix_tree<K, T>::erase(const K &key)
{
    radix_tree_node<K, T> *node = get_longest_prefix_node(key, m_root, 0);
    if (!node->m_is_leaf)
        return false;
    erase(node);
    return true;
}

template <typename K, typename T>
void radix_tree<K, T>::erase(radix_tree_node<K, T> *node)
{
    assert(node->m_is_leaf == true);

    radix_tree_node<K, T> *parent = node->m_parent;

    //删除node节点
    parent->m_children.erase(node->m_key);
    delete node;

    if (parent == m_root || parent->m_children.size() > 1)
    {
        return;
    }
    //删除node后其父节点的子节点数为0
    else if (parent->m_children.size() == 0)
    {
        erase(parent);
    }
    //删除node后其父节点的子节点只剩一个节点
    else if (parent->m_children.size() == 1)
    {
        typename radix_tree_node<K, T>::iterator_child it = parent->m_children.begin();
        assert(it->second->m_is_leaf == false);
        merge_node(it->second);
    }
}

template <typename K, typename T>
T &radix_tree<K, T>::operator[](const K &key)
{
    iterator it = find(key);
    if (it == end())
    {
        std::pair<K, T> val;
        val.first = key;
        std::pair<iterator, bool> ret = insert(val);
        assert(ret.second == true);
        it = ret.first;
    }
    //->运算符被重载，返回pair<const K, T>类型
    return it->second;
}

template <typename K, typename T>
std::pair<typename radix_tree<K, T>::iterator, bool> radix_tree<K, T>::insert(const value_type &val)
{
    if (m_root == NULL)
    {
        K nul = radix_substr(val.first, 0, 0);
        m_root = new radix_tree_node<K, T>();
        m_root->m_key = nul;
    }

    radix_tree_node<K, T> *node = get_longest_prefix_node(val.first, m_root, 0);
    if (node->m_is_leaf)
    {
        //node参数会被编译器自动调用iterator的类型转换构造函数转换为iterator
        return std::pair<iterator, bool>(node, false);
    }
    else
    {
        m_size++;
        //判断node是否有后缀来调用不同的构建方法
        bool node_is_prefix = (node->m_key == radix_substr(val.first, node->m_depth, radix_length(node->m_key)));
        if (node == m_root || node_is_prefix)
        {
            node = add_child(node, val);
        }
        else
        {
            node = add_root(node, val);
        }
        return std::pair<iterator, bool>(node, true);
    }
}

template <typename K, typename T>
radix_tree_node<K, T> *radix_tree<K, T>::add_root(radix_tree_node<K, T> *node, const value_type &val)
{
    assert(node->m_is_leaf == false);

    //除去压缩前缀的node和val序列长度，统计其prefix
    int len1 = radix_length(node->m_key);
    int len2 = radix_length(val.first) - node->m_depth;
    int count;
    for (count = 0; count < len1 && count < len2; count++)
        if (node->m_key[count] != val.first[node->m_depth + count])
            break;
    //统计node和val的diff
    int len_diff_node = len1 - count;
    assert(count > 0);
    assert(len_diff_node > 0);

    //在node的父节点中移除node节点
    node->m_parent->m_children.erase(node->m_key);

    //在node原有位置新建公共前缀序列节点
    K key = radix_substr(node->m_key, 0, count);
    radix_tree_node<K, T> *p = new radix_tree_node<K, T>();
    p->m_key = key;
    p->m_depth = node->m_depth;
    p->m_is_leaf = false;
    p->m_parent = node->m_parent;
    p->m_parent->m_children[key] = p;

    //重构node节点的key
    node->m_parent = p;
    node->m_depth += count;
    node->m_key = radix_substr(node->m_key, count, len_diff_node);
    node->m_parent->m_children[node->m_key] = node;

    //添加val序列节点
    return add_child(p, val);
}

template <typename K, typename T>
radix_tree_node<K, T> *radix_tree<K, T>::add_child(radix_tree_node<K, T> *parent, const value_type &val)
{
    assert(parent->m_key == radix_substr(val.first, parent->m_depth, radix_length(parent->m_key)));
    int len_prefix = parent->m_depth + radix_length(parent->m_key);
    int len_diff = radix_length(val.first) - len_prefix;

    if (len_diff)
    {
        radix_tree_node<K, T> *node = new radix_tree_node<K, T>();
        K key = radix_substr(val.first, len_prefix, len_diff);
        node->m_key = key;
        node->m_depth = parent->m_depth + radix_length(parent->m_key);
        node->m_is_leaf = false;
        node->m_parent = parent;
        parent->m_children[key] = node;
        parent = node;
    }
    return add_leaf(parent, val);
}

template <typename K, typename T>
radix_tree_node<K, T> *radix_tree<K, T>::add_leaf(radix_tree_node<K, T> *parent, const value_type &val)
{
    assert(radix_length(val.first) == (parent->m_depth + radix_length(parent->m_key)));
    radix_tree_node<K, T> *node = new radix_tree_node<K, T>(val);
    K nul = radix_substr(val.first, 0, 0);
    node->m_key = nul;
    parent->m_children[nul] = node;
    node->m_parent = parent;
    node->m_depth = radix_length(val.first);
    node->m_is_leaf = true;
    return node;
}

template <typename K, typename T>
typename radix_tree<K, T>::iterator radix_tree<K, T>::end()
{
    return iterator(NULL);
}

template <typename K, typename T>
typename radix_tree<K, T>::iterator radix_tree<K, T>::begin()
{
    radix_tree_node<K, T> *node;

    if (m_root == NULL)
        return NULL;
    else
        node = begin(m_root);

    return iterator(node);
}

template <typename K, typename T>
radix_tree_node<K, T> *radix_tree<K, T>::begin(radix_tree_node<K, T> *node)
{
    assert(node != NULL);
    if (node->m_is_leaf)
        return node;

    return begin(node->m_children.begin()->second);
}

template <typename K, typename T>
void radix_tree<K, T>::get_leafs(radix_tree_node<K, T> *node, std::vector<iterator> &vec)
{
    if (node->m_is_leaf)
    {
        vec.push_back(node); //会调用转换构造函数
        return;
    }

    typename radix_tree_node<K, T>::iterator_child it;
    for (it = node->m_children.begin(); it != node->m_children.end(); ++it)
        get_leafs(it->second, vec);
}

template <typename K, typename T>
typename radix_tree<K, T>::iterator radix_tree<K, T>::find(const K &key)
{
    if (m_root == NULL)
        return iterator(NULL);

    radix_tree_node<K, T> *node = get_longest_prefix_node(key, m_root, 0);

    if (!node->m_is_leaf)
        return iterator(NULL);
    else
        return iterator(node);
}

template <typename K, typename T>
radix_tree_node<K, T> *radix_tree<K, T>::get_longest_prefix_node(const K &key, radix_tree_node<K, T> *node, int matched)
{
    if (node->m_children.empty())
        return node;

    typename radix_tree_node<K, T>::iterator_child it;
    int unmatched = radix_length(key) - matched;

    for (it = node->m_children.begin(); it != node->m_children.end(); ++it)
    {
        //完全匹配成功，继续寻找叶子节点
        if (unmatched == 0)
        {
            if (it->second->m_is_leaf)
                return it->second;
            else
                continue;
        }

        //匹配子节点的key，返回至少有公共前缀的节点，或者完全匹配当前节点，继续递归查找其子节点
        if (!it->second->m_is_leaf && key[matched] == it->first[0])
        {
            int next_match_len = radix_length(it->first);
            K key_next_match = radix_substr(key, matched, next_match_len);

            if (key_next_match == it->first)
                return get_longest_prefix_node(key, it->second, matched + next_match_len);
            else
                //匹配树中叶子节点失败，返回当前最长前缀匹配节点
                return it->second;
        }
    }

    //完全匹配时node不存在叶子节点，或者node的子节点无法匹配当前序列，返回node
    return node;
}
#endif //RADIX_TREE