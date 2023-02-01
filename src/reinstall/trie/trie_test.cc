#include "gtest/gtest.h"
#include "reinstall/trie/trie.h"

using namespace reinstall::trie;

TEST(TrieTest, maxSharedPrefixSize) {
    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("", "abc"), 0);
    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("abc", ""), 0);
    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("cba", "abc"), 0);

    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("a", "abcdefgh"), 1);
    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("abcdefgh", "a"), 1);

    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("abcdef", "abc"), 3);
    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("abc", "abcdef"), 3);
    EXPECT_EQ(Trie<int>::maxSharedPrefixSize("abc", "abc"), 3);
}

TEST(TrieTest, Find) {
    Trie<int> t{};

    t.insert("abced-fghik", 1);
    t.insert("abced-abcde", 2);
    t.insert("abcde-fghik-lmnop", 3);
    t.insert("12345-abcde", 4);
    t.insert("12345-fghik", 5);
    t.insert("abcde-fghik-lmnop-12345", 6);
    t.insert("abcde", 7);
    t.insert("abcde-", 8);
    t.insert("a", 9);

    auto r1 = t.find("-");
    EXPECT_FALSE(r1.second);
    EXPECT_EQ(r1.first, 0);

    auto r2 = t.find("abcde");
    EXPECT_TRUE(r2.second);
    EXPECT_EQ(r2.first, 7);

    auto r3 = t.find("abcde-");
    EXPECT_TRUE(r3.second);
    EXPECT_EQ(r3.first, 8);

    auto r4 = t.find("abcde-123");
    EXPECT_FALSE(r4.second);
    EXPECT_EQ(r4.first, 0);

    auto r5 = t.find("12345-fghik");
    EXPECT_TRUE(r5.second);
    EXPECT_EQ(r5.first, 5);

    auto r6 = t.find("a");
    EXPECT_TRUE(r6.second);
    EXPECT_EQ(r6.first, 9);

    auto r7 = t.find("abced-abcde");
    EXPECT_TRUE(r7.second);
    EXPECT_EQ(r7.first, 2);
}

TEST(TrieTest, FindSeries) {
    Trie<int> t{};

    t.insert("abcde-fghik", 1);
    t.insert("abcde-abcde", 2);
    t.insert("abcde-fghik-lmnop", 3);
    t.insert("12345-abcde", 4);
    t.insert("12345-fghik", 5);
    t.insert("abcde-fghik-lmnop-12345", 6);
    t.insert("abcde", 7);
    t.insert("abcde-", 8);
    t.insert("a", 9);
    t.insert("abcde-fghik-p", 10);

    auto r1 = t.findSeries("-");
    EXPECT_EQ(r1.size(), 0);

    auto r2 = t.findSeries("abcde-fghik-lmnop-");
    EXPECT_EQ(r2.size(), 5);
    EXPECT_EQ(r2[0], 9);
    EXPECT_EQ(r2[1], 7);
    EXPECT_EQ(r2[2], 8);
    EXPECT_EQ(r2[3], 1);
    EXPECT_EQ(r2[4], 3);

    auto r3 = t.findSeries("abcde-");
    EXPECT_EQ(r3.size(), 3);
    EXPECT_EQ(r3[0], 9);
    EXPECT_EQ(r3[1], 7);
    EXPECT_EQ(r3[2], 8);

    auto r4 = t.findSeries("abcde-12345");
    EXPECT_EQ(r4.size(), 3);
    EXPECT_EQ(r4[0], 9);
    EXPECT_EQ(r4[1], 7);
    EXPECT_EQ(r4[2], 8);

    auto r5 = t.findSeries("12345-");
    EXPECT_EQ(r5.size(), 0);

    auto r6 = t.findSeries("abcde-abcde");
    EXPECT_EQ(r6.size(), 4);
    EXPECT_EQ(r6[0], 9);
    EXPECT_EQ(r6[1], 7);
    EXPECT_EQ(r6[2], 8);
    EXPECT_EQ(r6[3], 2);

    auto r7 = t.findSeries("abcde-f-p");
    EXPECT_EQ(r7.size(), 3);
    EXPECT_EQ(r7[0], 9);
    EXPECT_EQ(r7[1], 7);
    EXPECT_EQ(r7[2], 8);
}