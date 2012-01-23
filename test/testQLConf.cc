
#include "gtest/gtest.h"

#include "../src/qlconf.h"

#include <vector>


/**
 * This class is used to test protected methods in QLConf
 */
class TestQLConf: public QLConf {

    public:
        bool testParseTrueFalse(const char * str) {
            return parseTrueFalse(str);
        }

        int testComp(const char * str, const char * pattern) {
            return comp(str, pattern);
        }
};



TEST(QLConfTest, parseTrueFalse) {
    
    TestQLConf conf = TestQLConf();
    
    EXPECT_TRUE(conf.testParseTrueFalse("true"));

    EXPECT_FALSE(conf.testParseTrueFalse("false"));
    EXPECT_FALSE(conf.testParseTrueFalse(" true"));
    EXPECT_FALSE(conf.testParseTrueFalse("true "));
    EXPECT_FALSE(conf.testParseTrueFalse("TRUE"));

}


TEST(QLConfTest, compEquals) {

    TestQLConf conf = TestQLConf();

    EXPECT_EQ(0, conf.testComp("option", "option"));
}


TEST(QLConfTest, compNotEquals) {

    TestQLConf conf = TestQLConf();

// size differs
    EXPECT_EQ(-1, conf.testComp("string", " string"));
    EXPECT_EQ(-1, conf.testComp(" string", "string"));

// string differs
    EXPECT_LT(0, conf.testComp("string ", " string"));
    EXPECT_GT(0, conf.testComp(" string", "string "));

}




