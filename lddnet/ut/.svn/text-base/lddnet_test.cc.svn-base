// lddnet_test.cc (2014-07-17)
// Yan Gaofeng (yangaofeng@360.cn)

#include <glog/logging.h>
#include <gtest/gtest.h>

#include "my_lddnet.h"

class MyLddNetTest : public testing::Test {
public:
    virtual void SetUp();
    virtual void TearDown();

protected:
    MyLddNet my_ldd_net_;
};

void MyLddNetTest::SetUp()
{
    my_ldd_net_.Init();
}

void MyLddNetTest::TearDown()
{
    my_ldd_net_.Finalize();
}

TEST_F(MyLddNetTest, LddNetTest)
{
    ASSERT_TRUE(my_ldd_net_.Echo());
}

