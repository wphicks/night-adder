#include <gtest/gtest.h>
#include <stdlib.h>
extern "C" {
  #include "particle.h"
  #include "integrator.h"
}

class IntegratorTest : public ::testing::Test {
 protected:
  Integrator * calc;
  int particle_count = 4;
  Particle ** all_parts;
  int thread_count = 1;

  virtual void SetUp() {
    all_parts = (Particle **) malloc(particle_count * sizeof(Particle *));

    all_parts[0] = create_Particle(2, 2, 2);
    all_parts[0]->position->components[0] = 0.0f;
    all_parts[0]->position->components[1] = 0.0f;
    all_parts[0]->velocity->components[0] = 2.0f;
    all_parts[0]->velocity->components[1] = 2.0f;

    all_parts[1] = create_Particle(1, 1, 3);
    all_parts[1]->position->components[0] = 0.0f;
    all_parts[1]->position->components[1] = 2.0f;
    all_parts[1]->velocity->components[0] = 0.0f;
    all_parts[1]->velocity->components[1] = 1.0f;

    all_parts[2] = create_Particle(3, 1, 1);
    all_parts[2]->position->components[0] = 2.0f;
    all_parts[2]->position->components[1] = 0.0f;
    all_parts[2]->velocity->components[0] = -1.0f;
    all_parts[2]->velocity->components[1] = 0.0f;

    all_parts[3] = create_Particle(1, 1, 1);
    all_parts[3]->position->components[0] = -2.0f;
    all_parts[3]->position->components[1] = 0.0f;
    all_parts[3]->velocity->components[0] = -1.0f;
    all_parts[3]->velocity->components[1] = 0.0f;

    calc = create_Integrator(thread_count, all_parts, particle_count);
  }

  virtual void TearDown() {
    int i;
    for (i=0; i < particle_count; ++i) {
      free_Particle(all_parts[i]);
    }
    free_Integrator(calc);
  }
};

TEST_F(IntegratorTest, index_Test) {
  EXPECT_EQ(particle_count - 3 + 3, pair_index(calc, 1, 3));
  EXPECT_EQ(particle_count - 3 + 3, pair_index(calc, 3, 1));
}

TEST_F(IntegratorTest, create_Test) {
  EXPECT_FLOAT_EQ(9.0f, calc->square_sum_radii[pair_index(calc, 0, 1)]);
  EXPECT_FLOAT_EQ(2.0f, calc->pair_restitution[pair_index(calc, 0, 1)]);
  EXPECT_FLOAT_EQ(0.75f, calc->pair_reduced_mass[pair_index(calc, 1, 2)]);
}

TEST_F(IntegratorTest, samedir_Test) {
  EXPECT_EQ(1, collide(calc, 0, 1));
  EXPECT_FLOAT_EQ(2.0f, calc->particles[0]->velocity->components[0]);
  EXPECT_FLOAT_EQ(1.0f, calc->particles[0]->velocity->components[1]);
  EXPECT_FLOAT_EQ(0.0f, calc->particles[1]->velocity->components[0]);
  EXPECT_FLOAT_EQ(3.0f, calc->particles[1]->velocity->components[1]);
}

TEST_F(IntegratorTest, oppdir_Test) {
  EXPECT_EQ(1, collide(calc, 0, 2));
  EXPECT_FLOAT_EQ(-1.8f, calc->particles[0]->velocity->components[0]);
  EXPECT_FLOAT_EQ(2.0f, calc->particles[0]->velocity->components[1]);
  EXPECT_FLOAT_EQ(1.6f, calc->particles[2]->velocity->components[0]);
  EXPECT_FLOAT_EQ(0.0f, calc->particles[2]->velocity->components[1]);
}

TEST_F(IntegratorTest, nocollide_Test) {
  EXPECT_EQ(0, collide(calc, 1, 2));
  EXPECT_EQ(0, collide(calc, 2, 3));
  EXPECT_EQ(0, collide(calc, 0, 3));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
