#include <iostream>
#include <arpa/inet.h>

#include "radix_tree.h"

using namespace std;

/**
 * @brief 基数树中的key，用来存储二进制序列的类型，类似string结构
 * @note 重载==，[]，<运算符，一个类型可以作为基数树key的另外还需要的条件是重载radix_tree.h中的全局模板函数
 */
class route_entry
{
public:
    in_addr_t addr;
    int len_prefix;

    route_entry() : addr(0), len_prefix(0){};
    route_entry(const char *network, int prefix_len)
    {
        addr = get_inaddr(network, prefix_len);
        len_prefix = prefix_len;
    };

    in_addr_t operator[](int n) const
    {
        if (addr & (0x80000000 >> n))
            return 1;
        else
            return 0;
    }

    bool operator==(const route_entry &r) const
    {
        return len_prefix == r.len_prefix && addr == r.addr;
    }

    bool operator<(const route_entry &rhs) const
    {
        if (addr == rhs.addr)
            return len_prefix < rhs.len_prefix;
        else
            return addr < rhs.addr;
    }

    /**
     * 将字符串网址network转换为主机顺序的二进制ip地址，并且取len_prefix位ip地址结果
     * @par len_prefix 需要大于0
     */
    static in_addr_t get_inaddr(const char *network, int len_prefix)
    {
        assert(len_prefix >= 0);
        in_addr in_add;
        in_addr_t in_addt;
        if (inet_aton(network, &in_add) == 0)
            //TODO:添加地址格式错误异常
            return 0;

        if (len_prefix == 0 || len_prefix >= 32)
        {
            in_addt = ntohl(in_add.s_addr);
        }
        else
        {
            int shift = 32 - len_prefix;
            in_addr_t mask = ~((1 << shift) - 1);
            in_addt = ntohl(in_add.s_addr) & mask;
        }
        return in_addt;
    }
};

/**
* @brief 截取route_entry子序列
*/
template <>
route_entry radix_substr<route_entry>(const route_entry &entry, int begin, int num)
{
    //TODO:使用异常来处理
    assert(begin < 32);
    assert(num >= 0);

    route_entry r;
    in_addr_t mask_substr;

    if (num == 0) //空序列
        r.addr = 0;
    else
    {
        if (begin + num > 32) //数量超出上限
            num = 32 - begin;
        //计算掩码位数
        //再移动掩码到begin处,偏移量为begin到结尾的序列长度-掩码长度
        mask_substr = ((1 << num) - 1) << (32 - begin - num);
        r.addr = (entry.addr & mask_substr) << begin; //移动子串顶头
    }
    r.len_prefix = num;
    return r;
}

template <>
route_entry radix_join<route_entry>(const route_entry &entry1, const route_entry &entry2)
{
    route_entry r;
    r.addr = entry1.addr;
    r.addr |= entry2.addr >> entry1.len_prefix;
    r.len_prefix = entry1.len_prefix + entry2.len_prefix;
    return r;
}

template <>
int radix_length<route_entry>(const route_entry &entry)
{
    return entry.len_prefix;
}

radix_tree<route_entry, in_addr> rttable;

/**
 * 在基数树中插入静态路由项(network、prefix、dst三元组)
 */
void insert(const char *network, int len_prefix, const char *dst)
{
    //新建entry插入
    route_entry entry(network, len_prefix);
    in_addr in_add;
    inet_aton(dst, &in_add);
    rttable[entry] = in_add;
}

/**
 * 在基数树中移除静态路由项
 */
bool remove(const char *network, int len_prefix)
{
    route_entry entry(network, len_prefix);
    return rttable.erase(entry);
}

void find(const char *dst)
{
    route_entry entry(dst, 32);
    radix_tree<route_entry, in_addr>::iterator it = rttable.longest_match(entry);
    if (it == rttable.end())
        cout << "no route to" << dst << endl;
    else
        cout << dst << "->" << inet_ntoa(it->second) << endl;
}

int main(int argc, char const *argv[])
{
    insert("0.0.0.0", 0, "192.168.0.1"); // default route
    insert("10.0.0.0", 8, "192.168.0.2");
    insert("172.16.0.0", 16, "192.168.0.3");
    insert("172.16.1.0", 24, "192.168.0.255");
    insert("172.17.0.0", 16, "192.168.0.4");
    insert("172.18.0.0", 16, "192.168.0.5");
    insert("172.19.0.0", 16, "192.168.0.6");
    insert("192.168.1.0", 24, "192.168.0.7");
    insert("192.168.2.0", 24, "192.168.0.8");
    insert("192.168.3.0", 24, "192.168.0.9");
    insert("192.168.4.0", 24, "192.168.0.10");

    find("10.1.1.1");
    find("172.16.1.3");
    find("172.16.1.3");
    find("172.17.0.5");
    find("172.18.10.5");
    find("172.19.200.70");
    find("192.168.1.10");
    find("192.168.2.220");
    find("192.168.3.80");
    find("192.168.4.100");
    find("172.20.0.1");
    return 0;
}