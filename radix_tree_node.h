#ifndef RADIX_TREE_NODE
#define RADIX_TREE_NODE
#include <map>

template <typename K, typename T>
class radix_tree_node
{
    friend class radix_tree<K, T>;
    friend class radix_tree_it<K, T>;

    typedef std::pair<const K, T> value_type;
    /**
     * @note typename作用
     * 标识iterator为类型名，而非变量名，防止出现歧义
     */
    typedef typename std::map<K, radix_tree_node<K, T> *>::iterator iterator_child;

private:
    //成员变量

    /**
     * @note 成员中的K区别、pair中的K、map中的K
     *  -成员中的K：存储部分序列值
     *  -pair中的K:存储完整的序列，占用动态内存。
     *  -map中的K：存储部分序列值，等于成员中的K
     */
    K m_key;

    /**
     * @note 只有叶子节点存储不为空的value_type
     */
    value_type *m_value;

    /**
     * @note 用红黑树结构管理子节点指针(相当于加索引)
     * map中的value占用动态内存，map本身是成员变量。
     */
    std::map<K, radix_tree_node<K, T> *> m_children;
    radix_tree_node<K, T> *m_parent;

    int m_depth;
    bool m_is_leaf;

    //构造函数
    /**
     * @note 初始化列表中m_key()的含义
     * 由于该类是泛型类，最终生成的代码中：
     *  -如果m_key是整形，其执行时会被初始化为0。
     *  -如果m_key是class类型，则该类必须有默认构造函数，否则无法编译。
     */
    radix_tree_node() : m_key(), m_value(), m_children(), m_parent(nullptr), m_depth(0), m_is_leaf(false) {}
    radix_tree_node(const value_type &val) : m_key(), m_value(), m_children(), m_parent(nullptr), m_depth(0), m_is_leaf(false)
    {
        m_value = new value_type(val);
    }

    ~radix_tree_node()
    {
        iterator_child it;
        for (it = m_children.begin(); it != m_children.end(); ++it)
        {
            delete it->second; //删除动态内存部分
        }
        delete m_value;
    }
};
#endif //RADIX_TREE_NODE