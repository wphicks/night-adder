#include <gtest/gtest.h>
extern "C" {
  #include "particle.h"
}

class particleTest : public ::testing::Test {
 protected:
  Particle part1;
  double dt = 0.5;
  double position[2] = {1.0, -3.0};
  double velocity[2] = {-1.0, -2.0};
  double result[2] = {0.5, -4.0};

  virtual void SetUp() {
    init_Particle(&part1, 1, 1, 1);
    init_Vec(&(part1.position), position);
    init_Vec(&(part1.velocity), velocity);
  }

  virtual void TearDown() {
  }
};

TEST_F(particleTest, update_Test) {
  update_Particle_position(&part1, dt);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(result[i], part1.position.components[i].as_double);
    EXPECT_DOUBLE_EQ(position[i], part1.prev_position.components[i].as_double);
  }
}

TEST_F(particleTest, interpolate_Test) {
  Vec interp;
  double alpha = 0.25;
  interpolate_Particle_position(&part1, alpha, &interp);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(position[i]*0.75, interp.components[i].as_double);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
