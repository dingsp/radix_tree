#ifndef RADIX_TREE_IT
#define RADIX_TREE_IT
#include <iterator>

template <typename K, typename T> class radix_tree;
template <typename K, typename T> class radix_tree_node;

template <typename K, typename T>
class radix_tree_it : public std::iterator<std::forward_iterator_tag, std::pair<const K, T> >
{
    friend class radix_tree<K, T>;

public:
    //构造函数
    radix_tree_it() : m_pointer(NULL) {
    }
    radix_tree_it(const radix_tree_it &r) : m_pointer(r.m_pointer) {
    }
    ~radix_tree_it(){};

    //重载运算符
    radix_tree_it &operator=(const radix_tree_it &r)
    {
        m_pointer = r.m_pointer;
        return *this;
    }

    std::pair<const K, T> &operator*() const
    {
        return *m_pointer->m_value;
    }

    std::pair<const K, T> *operator->() const
    {
        return m_pointer->m_value;
    }

    radix_tree_it<K, T> &operator++()
    {
        if (m_pointer != NULL)
        {
            m_pointer = get_next_leaf(m_pointer);
        }
        return *this;
    }
    
    radix_tree_it<K, T> operator++(int)
    {
        radix_tree_it<K, T> copy(*this);
        ++(*this);
        return copy;
    }

    bool operator!=(const radix_tree_it<K, T> &r) const{
        return m_pointer!=r.m_pointer;
    }

    bool operator==(const radix_tree_it<K, T> &r) const{
        return m_pointer==r.m_pointer;
    }

private:
    //成员变量
    /**
     * @note 迭代器以操作node结点作为各API实现的基础，而通过迭代器获取实际元素则通过运算符
     */
    radix_tree_node<K, T> *m_pointer;

    //构造函数
    radix_tree_it(radix_tree_node<K, T> *p) : m_pointer(p) {
    }

    //基本函数
    /**
     * @brief 获取下一个叶子节点，树的叶子节点间的线性关系为从左到右
     * 获取节点的下一个兄弟节点为根的树的首个叶子节点，如果这个节点是兄弟中的最
     * 后一个节点，则沿着父节点向上递归寻找兄弟节点，直到来到根节点时返回空，表
     * 示该节点没有下一个叶子节点，是树中的最后一个叶子节点。
     * @return 
     *  -null:      表示node为树中最后一个叶子节点
     *  -none null: node兄弟节点的最左叶子节点
     */
    radix_tree_node<K, T> *get_next_leaf(radix_tree_node<K, T> *node) const
    {
        radix_tree_node<K, T> *parent = node->m_parent;
        //当前参数为根节点时退出
        if (parent == NULL)
            return NULL;

        typename radix_tree_node<K, T>::iterator_child it = parent->m_children.find(node->m_key);
        ++it;

        if (it == parent->m_children.end())
            return get_next_leaf(parent);
        else
            return get_first_leaf(it->second);
    }

    /**
     * @brief 返回节点子树中的最左叶子节点
     * @note 递归实现
     */
    radix_tree_node<K, T> *get_first_leaf(radix_tree_node<K, T> *node) const
    {
        if (node->m_is_leaf)
            return node;
        typename radix_tree_node<K, T>::iterator_child it = node->m_children.begin();
        return get_first_leaf(it->second);
    }
};
#endif //RADIX_TREE_IT