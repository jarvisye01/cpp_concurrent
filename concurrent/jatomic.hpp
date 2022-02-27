#ifndef J_ATOMIC_H
#define J_ATOMIC_H

namespace jarvis
{


/*
 * AtomicInt实现了一个原子int类型
 * 均采用乐观锁实现
 * 1.Add 在原子变量上加上一个值并且返回当前值
 * 2.Sub 在原子变量上减去一个值并且返回当前值
 * 3.Get 获取原子变量的值
 * 4.Set 设置原子变量的值
 */
class JAtomicInt
{
public:
    JAtomicInt(int n = 0);
    ~JAtomicInt();
    JAtomicInt(const JAtomicInt&) = delete;
    JAtomicInt& operator=(const JAtomicInt&) = delete;

    int Add(int n = 1);
    int Sub(int n = 1);
    int Get() const;
    int Set(int n);
private:
    int num;
};

}   // namespace jarvis

#endif
