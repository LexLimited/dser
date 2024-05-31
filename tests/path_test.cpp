#include <gtest/gtest.h>

#include <dser/path.h>

TEST(path_test, split)
{
    dser::path p("/");
    EXPECT_EQ(p.get_segments().size(), 0);
    
    p.set("a/b");
    EXPECT_EQ(p.get_segments().size(), 0);
    EXPECT_EQ(p.get_segment(0), "a");
    EXPECT_EQ(p.get_segment(1), "b");

    p.set("/a");
    EXPECT_EQ(p.get_segments().size(), 1);
    EXPECT_EQ(p.get_segment(0), "a");
}

