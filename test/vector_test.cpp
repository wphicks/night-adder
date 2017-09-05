#include <gtest/gtest.h>
extern "C" {
  #include "vector.h"
}

class vectorTest : public ::testing::Test {
 protected:
  Vec * vec1;
  Vec * vec2;
  Vec * result;
  double sum[2] = {1.0, 2.0};
  double diff[2] = {5.0, 6.0};
  double norm[2] = {0.6, 0.8};
  virtual void SetUp() {
    vec1 = create_Vec();
    vec2 = create_Vec();
    result = create_Vec();
    vec1->components[0].as_double = 3.0;
    vec1->components[1].as_double = 4.0;
    vec2->components[0].as_double = -2.0;
    vec2->components[1].as_double = -2.0;
  }

  virtual void TearDown() {
    free_Vec(vec1);
    free_Vec(vec2);
    free_Vec(result);
  }
};

TEST_F(vectorTest, sumTest) {
  sum_Vec(vec1, vec2, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(sum[i], result->components[i].as_double);
  }
}

TEST_F(vectorTest, diffTest) {
  diff_Vec(vec1, vec2, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(diff[i], result->components[i].as_double);
  }
}

TEST_F(vectorTest, dotTest) {
  EXPECT_DOUBLE_EQ(-14.0f, dot_Vec(vec1, vec2));
}

TEST_F(vectorTest, distTest) {
  EXPECT_DOUBLE_EQ(61.0f, distance_square(vec1, vec2));
}

TEST_F(vectorTest, magnitudeTest) {
  EXPECT_DOUBLE_EQ(25.0f, magnitude_square(vec1));
  EXPECT_DOUBLE_EQ(5.0f, magnitude(vec1));
}

TEST_F(vectorTest, normTest) {
  norm_Vec(vec1, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(norm[i], result->components[i].as_double);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
