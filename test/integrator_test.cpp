#include <gtest/gtest.h>
#include <stdlib.h>
extern "C" {
  #include "particle.h"
  #include "integrator.h"
}

class IntegratorTest : public ::testing::Test {
 protected:
  Integrator calc;
  int thread_count = 2;
  int particle_count = 4;
  int pair_count = 6;
  Particle * all_parts;
  ParticlePair pairs[6];
  double pos[4][VECDIM] = {
    {0.0, 0.0},
    {0.0, 2.0},
    {2.0, 0.0},
    {-2.0, 0.0}
  };
  double vel[4][VECDIM] = {
    {2.0, 2.0},
    {0.0, 1.0},
    {-1.0, 0.0},
    {-1.0, 0.0}
  };

  virtual void SetUp() {
    int i;
    int j;
    int k;
    all_parts = (Particle *) malloc(particle_count * sizeof(Particle));

    init_Particle(all_parts + 0, 2, 2, 2);
    init_Particle(all_parts + 1, 1, 1, 3);
    init_Particle(all_parts + 2, 3, 1, 1);
    init_Particle(all_parts + 3, 1, 1, 1);
    for (i=0; i < particle_count; ++i) {
      init_Vec(&((all_parts + i)->position), pos[i]);
      init_Vec(&((all_parts + i)->velocity), vel[i]);
    }

    k = 0;
    for (i=0; i < particle_count; ++i) {
      for (j=i+1; j < particle_count; ++j) {
        init_ParticlePair(pairs + k, i, j, particle_count, all_parts);
        ++k;
      }
    }

    init_Integrator(&calc, thread_count, particle_count, all_parts);
  }

  virtual void TearDown() {
    cleanup_Integrator(&calc);
  }
};

TEST_F(IntegratorTest, samedir_Test) {
  EXPECT_EQ(1,
    collide(&calc, calc.pairs + pair_index(0, 1, calc.particle_count))
  );
  EXPECT_FLOAT_EQ(2.0, calc.particles[0].velocity.components[0].as_double);
  EXPECT_FLOAT_EQ(1.0, calc.particles[0].velocity.components[1].as_double);
  EXPECT_FLOAT_EQ(0.0, calc.particles[1].velocity.components[0].as_double);
  EXPECT_FLOAT_EQ(3.0, calc.particles[1].velocity.components[1].as_double);
}

TEST_F(IntegratorTest, oppdir_Test) {
  EXPECT_EQ(1,
    collide(&calc, calc.pairs + pair_index(0, 2, calc.particle_count))
  );
  EXPECT_FLOAT_EQ(-1.6, calc.particles[0].velocity.components[0].as_double);
  EXPECT_FLOAT_EQ(2.0, calc.particles[0].velocity.components[1].as_double);
  EXPECT_FLOAT_EQ(1.4, calc.particles[2].velocity.components[0].as_double);
  EXPECT_FLOAT_EQ(0.0, calc.particles[2].velocity.components[1].as_double);
}

TEST_F(IntegratorTest, moving_apart_Test) {
  EXPECT_EQ(1,
    collide(&calc, calc.pairs + pair_index(0, 3, calc.particle_count))
  );
  EXPECT_FLOAT_EQ(2.0, calc.particles[0].velocity.components[0].as_double);
  EXPECT_FLOAT_EQ(2.0, calc.particles[0].velocity.components[1].as_double);
  EXPECT_FLOAT_EQ(-1.0, calc.particles[3].velocity.components[0].as_double);
  EXPECT_FLOAT_EQ(0.0, calc.particles[3].velocity.components[1].as_double);
}

TEST_F(IntegratorTest, nocollide_Test) {
  EXPECT_EQ(0,
    collide(&calc, calc.pairs + pair_index(1, 2, calc.particle_count))
  );
  EXPECT_EQ(0,
    collide(&calc, calc.pairs + pair_index(2, 3, calc.particle_count))
  );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
