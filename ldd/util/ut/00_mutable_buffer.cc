// 00_mutable_buffer.cc (2013-11-19)
// Li Xinjie (xjason.li@gmail.com)

#include <gtest/gtest.h>
#include <ldd/util/mutable_buffer.h>

using namespace ldd::util;

class MutableBufferTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        b1_ = MutableBuffer();
        ptr2_ = malloc(10);
        b2_ = MutableBuffer(ptr2_, 10);
        ptr3_ = (char*)malloc(20);
        b3_ = MutableBuffer(ptr3_, 20);
    }
    virtual void TearDown() {
        free(ptr2_);
        free(ptr3_);
    }

    MutableBuffer b1_;
    void* ptr2_;
    MutableBuffer b2_;
    char* ptr3_;
    MutableBuffer b3_;
};

TEST_F(MutableBufferTest, Getter) {
    ASSERT_EQ(b1_.data(), "");
    ASSERT_EQ(b1_.size(), 0UL);
    ASSERT_TRUE(b1_.empty());

    ASSERT_EQ(b2_.data(), ptr2_);
    ASSERT_EQ(b2_.size(), 10UL);
    ASSERT_FALSE(b2_.empty());
    ASSERT_EQ(&b2_[0], ptr2_);

    ASSERT_EQ(b3_.data(), ptr3_);
    ASSERT_EQ(b3_.size(), 20UL);
    ASSERT_FALSE(b3_.empty());
    ASSERT_EQ(&b3_[0], ptr3_);
}

