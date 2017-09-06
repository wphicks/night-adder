#include <gtest/gtest.h>
extern "C" {
  #include "particle.h"
}

class particleTest : public ::testing::Test {
 protected:
  Particle * part1;
  double dt = 0.5;
  double result[2] = {0.5, -2.0};

  virtual void SetUp() {
    part1 = create_Particle(1, 1, 1);
    part1->position->components[0].as_double = 1;
    part1->position->components[1].as_double = -3;
    part1->velocity->components[0].as_double = -1;
    part1->velocity->components[1].as_double = 2;
  }

  virtual void TearDown() {
    free_Particle(part1);
  }
};

TEST_F(particleTest, update_Test) {
  update_Particle_position(part1, dt);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(part1->position->components[i].as_double, result[i]);
  }
}
TEST_F(particleTest, atomic_update_Test) {
  atomic_update_Particle_position(part1, dt);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(part1->position->components[i].as_double, result[i]);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
