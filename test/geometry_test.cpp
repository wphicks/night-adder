#include <gtest/gtest.h>
extern "C" {
  #include "geometry.h"
}

class geometryTest : public ::testing::Test {
 protected:
  Vec * vec1;
  Vec * vec2;
  Vec * result;
  float sum[2] = {1.0f, 2.0f};
  float diff[2] = {5.0f, 6.0f};
  float norm[2] = {0.6f, 0.8f};
  virtual void SetUp() {
    vec1 = create_Vec();
    vec2 = create_Vec();
    result = create_Vec();
    vec1->components[0] = 3.0f;
    vec1->components[1] = 4.0f;
    vec2->components[0] = -2.0f;
    vec2->components[1] = -2.0f;
  }

  virtual void TearDown() {
    free_Vec(vec1);
    free_Vec(vec2);
    free_Vec(result);
  }
};

TEST_F(geometryTest, sumTest) {
  sum_Vec(vec1, vec2, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_FLOAT_EQ(sum[i], result->components[i]);
  }
}

TEST_F(geometryTest, diffTest) {
  diff_Vec(vec1, vec2, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_FLOAT_EQ(diff[i], result->components[i]);
  }
}

TEST_F(geometryTest, dotTest) {
  EXPECT_FLOAT_EQ(-14.0f, dot_Vec(vec1, vec2));
}

TEST_F(geometryTest, distTest) {
  EXPECT_FLOAT_EQ(61.0f, distance_square(vec1, vec2));
}

TEST_F(geometryTest, magnitudeTest) {
  EXPECT_FLOAT_EQ(25.0f, magnitude_square(vec1));
  EXPECT_FLOAT_EQ(5.0f, magnitude(vec1));
}

TEST_F(geometryTest, normTest) {
  norm_Vec(vec1, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_FLOAT_EQ(norm[i], result->components[i]);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
