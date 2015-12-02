// factory.h (2013-05-29)
// Li Xinjie (xjason.li@gmail.com)

#ifndef LDD_NET_FACTORY_H_
#define LDD_NET_FACTORY_H_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace ldd {
namespace net {

struct NoneArg;

/*
 * 工厂类，负责创建对象，最多支持十个参数
 * */
template <class T>
struct Factory {
    virtual ~Factory() {}
    virtual T* New() const = 0;
    virtual boost::shared_ptr<T> MakeShared() const = 0;

    template <class D>
    static Factory* NewFactory();

    template <class D, class A1>
    static Factory* NewFactory(A1 a1);

    template <class D, class A1, class A2>
    static Factory* NewFactory(A1 a1, A2 a2);

    template <class D, class A1, class A2, class A3>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3);

    template <class D, class A1, class A2, class A3, class A4>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3, A4 a4);

    template <class D, class A1, class A2, class A3, class A4, class A5>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);

    template <class D, class A1, class A2, class A3, class A4, class A5, class A6>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6);

    template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7);

    template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8);

    template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9);

    template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    static Factory* NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10);
};

template <class T, class D,
         class A1 = NoneArg, class A2 = NoneArg, class A3 = NoneArg,
         class A4 = NoneArg, class A5 = NoneArg, class A6 = NoneArg,
         class A7 = NoneArg, class A8 = NoneArg, class A9 = NoneArg,
         class A10 = NoneArg>
struct FactoryImpl;

template <class T, class D>
struct FactoryImpl<T, D, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl() {}
    T* New() const { return new D(); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(); }
};

template <class T, class D, class A1>
struct FactoryImpl<T, D, A1, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1)
        :a1_(a1) {}
    T* New() const { return new D(a1_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_); }
    A1 a1_;
};

template <class T, class D, class A1, class A2>
struct FactoryImpl<T, D, A1, A2, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2)
        :a1_(a1), a2_(a2) {}
    T* New() const { return new D(a1_, a2_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_); }
    A1 a1_; A2 a2_;
};

template <class T, class D, class A1, class A2, class A3>
struct FactoryImpl<T, D, A1, A2, A3, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3)
        :a1_(a1), a2_(a2), a3_(a3) {}
    T* New() const { return new D(a1_, a2_, a3_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_); }
    A1 a1_; A2 a2_; A3 a3_;
};

template <class T, class D, class A1, class A2, class A3, class A4>
struct FactoryImpl<T, D, A1, A2, A3, A4, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3, A4 a4)
        :a1_(a1), a2_(a2), a3_(a3), a4_(a4) {}
    T* New() const { return new D(a1_, a2_, a3_, a4_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_, a4_); }
    A1 a1_; A2 a2_; A3 a3_; A4 a4_;
};

template <class T, class D, class A1, class A2, class A3, class A4, class A5>
struct FactoryImpl<T, D, A1, A2, A3, A4, A5, NoneArg, NoneArg, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
        :a1_(a1), a2_(a2), a3_(a3), a4_(a4), a5_(a5) {}
    T* New() const { return new D(a1_, a2_, a3_, a4_, a5_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_, a4_, a5_); }
    A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_;
};

template <class T, class D, class A1, class A2, class A3, class A4, class A5, class A6>
struct FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, NoneArg, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
        :a1_(a1), a2_(a2), a3_(a3), a4_(a4), a5_(a5), a6_(a6) {}
    T* New() const { return new D(a1_, a2_, a3_, a4_, a5_, a6_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_, a4_, a5_, a6_); }
    A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_; A6 a6_;
};

template <class T, class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, A7, NoneArg, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
        :a1_(a1), a2_(a2), a3_(a3), a4_(a4), a5_(a5), a6_(a6), a7_(a7) {}
    T* New() const { return new D(a1_, a2_, a3_, a4_, a5_, a6_, a7_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_, a4_, a5_, a6_, a7_); }
    A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_; A6 a6_; A7 a7_;
};

template <class T, class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
struct FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, A7, A8, NoneArg, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
        :a1_(a1), a2_(a2), a3_(a3), a4_(a4), a5_(a5), a6_(a6), a7_(a7), a8_(a8) {}
    T* New() const { return new D(a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_); }
    A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_; A6 a6_; A7 a7_; A8 a8_;
};

template <class T, class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
struct FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, A7, A8, A9, NoneArg> : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
        :a1_(a1), a2_(a2), a3_(a3), a4_(a4), a5_(a5), a6_(a6), a7_(a7), a8_(a8), a9_(a9) {}
    T* New() const { return new D(a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_); }
    A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_; A6 a6_; A7 a7_; A8 a8_; A9 a9_;
};

template <class T, class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
struct FactoryImpl : public Factory<T> {
    FactoryImpl(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
        :a1_(a1), a2_(a2), a3_(a3), a4_(a4), a5_(a5), a6_(a6), a7_(a7), a8_(a8), a9_(a9), a10_(a10) {}
    T* New() const { return new D(a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_, a10_); }
    boost::shared_ptr<T> MakeShared() const { return boost::make_shared<D>(a1_, a2_, a3_, a4_, a5_, a6_, a7_, a8_, a9_, a10_); }
    A1 a1_; A2 a2_; A3 a3_; A4 a4_; A5 a5_; A6 a6_; A7 a7_; A8 a8_; A9 a9_; A10 a10_;
};

template <class T> template <class D>
inline Factory<T>* Factory<T>::NewFactory() {
    return new FactoryImpl<T, D>();
}
template <class T> template <class D, class A1>
inline Factory<T>* Factory<T>::NewFactory(A1 a1) {
    return new FactoryImpl<T, D, A1>(a1);
}
template <class T> template <class D, class A1, class A2>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2) {
    return new FactoryImpl<T, D, A1, A2>(a1, a2);
}
template <class T> template <class D, class A1, class A2, class A3>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3) {
    return new FactoryImpl<T, D, A1, A2, A3>(a1, a2, a3);
}
template <class T> template <class D, class A1, class A2, class A3, class A4>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3, A4 a4) {
    return new FactoryImpl<T, D, A1, A2, A3, A4>(a1, a2, a3, a4);
}
template <class T> template <class D, class A1, class A2, class A3, class A4, class A5>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
    return new FactoryImpl<T, D, A1, A2, A3, A4, A5>(a1, a2, a3, a4, a5);
}
template <class T> template <class D, class A1, class A2, class A3, class A4, class A5, class A6>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
    return new FactoryImpl<T, D, A1, A2, A3, A4, A5, A6>(a1, a2, a3, a4, a5, a6);
}
template <class T> template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) {
    return new FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, A7>(a1, a2, a3, a4, a5, a6, a7);
}
template <class T> template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) {
    return new FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, A7, A8>(a1, a2, a3, a4, a5, a6, a7, a8);
}
template <class T> template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) {
    return new FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, A7, A8, A9>(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
template <class T> template <class D, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
inline Factory<T>* Factory<T>::NewFactory(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) {
    return new FactoryImpl<T, D, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}


} // namespace net
} // namespace ldd

#endif // LDD_NET_FACTORY_H_
