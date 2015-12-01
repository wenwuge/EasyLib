// a.h (2013-05-29)
// Li Xinjie (xjason.li@gmail.com)

#ifndef _A_H_
#define _A_H_

class NoneArg;

struct Msg {
    virtual ~Msg() {}
    virtual void Test() = 0;
};

struct Factory {
    virtual ~Factory() {}
    virtual Msg* New() const = 0;


    template <class T, class A1, class A2>
    static Factory* NewFactory(A1 a1, A2 a2);
    template <class T, class A1>
    static Factory* NewFactory(A1 a1);
    template <class T>
    static Factory* NewFactory();
};

template <class T, class A1 = NoneArg, class A2 = NoneArg>
struct FactoryImpl : public Factory {
    FactoryImpl(A1 a1, A2 a2)
        :a1_(a1), a2_(a2) {}
    Msg* New() const { return new T(a1_, a2_); }
    A1 a1_;
    A2 a2_;
};

template <class T>
struct FactoryImpl<T, NoneArg, NoneArg> : public Factory {
    FactoryImpl() {}
    Msg* New() const { return new T(); }
};

template <class T, class A1>
struct FactoryImpl<T, A1, NoneArg> : public Factory{
    FactoryImpl(A1 a1)
        :a1_(a1) {}
    Msg* New() const { return new T(a1_); }
    A1 a1_;
};

template <class T, class A1, class A2>
inline Factory* Factory::NewFactory(A1 a1, A2 a2) {
    return new FactoryImpl<T, A1, A2>(a1, a2);
}
template <class T, class A1>
inline Factory* Factory::NewFactory(A1 a1) {
    return new FactoryImpl<T, A1>(a1);
}
template <class T>
inline Factory* Factory::NewFactory() {
    return new FactoryImpl<T>();
}

#endif // _A_H_
