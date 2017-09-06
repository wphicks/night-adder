#include <gtest/gtest.h>
extern "C" {
  #include "vector.h"
  #include "particle.h"
  #include "particle_pair.h"
}

class ParticlePairTest : public ::testing::Test {
 protected:
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
  }

  virtual void TearDown() {
    free(all_parts);
  }
};

TEST_F(ParticlePairTest, indexTest) {
  int i;
  int j;
  int k;

  k = 0;
  for (i=0; i < particle_count; ++i) {
    for (j=i+1; j < particle_count; ++j) {
      EXPECT_EQ(k, pair_index(i, j, particle_count));
      ++k;
    }
  }

  for (k=0; k < pair_count; ++k) {
    EXPECT_EQ(k, (pairs + k)->pair_index);
  }
}

TEST_F(ParticlePairTest, massTest) {
  EXPECT_DOUBLE_EQ(0.75, (pairs + 3)->reduced_mass);
}

TEST_F(ParticlePairTest, restitutionTest) {
  EXPECT_DOUBLE_EQ(2.0, (pairs + 0)->restitution);
}

TEST_F(ParticlePairTest, radiiTest) {
  EXPECT_DOUBLE_EQ(9.0, (pairs + 0)->square_sum_radii);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
