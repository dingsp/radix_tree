# radix_tree
STL like

## 基数树是一种提供哈希功能的数据结构，空间复杂度低于哈希表，并额外提供快速的前缀匹配功能可用于字符串匹配或查找路由表。
## 该基数树实现的可以缓存key类型有：二进制类型，字符串类型。

## 设要查找关键字为key，基数树已存储元素集合E、数量为n，基数树在哈希查找的基础上额外提供如下功能：
## ·查找E中的元素e，e能够最长前缀匹配key。
## ·查找E中的元素集合e’，key能够前缀匹配E'中的元素。
## ·查找E中的元素集合E‘，key和E‘中的元素有公共前缀。
## 基数树为以上功能提供近似O(log(n))的时间复杂度，O（n）的空间复杂度。
