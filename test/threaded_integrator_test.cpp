#include <gtest/gtest.h>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <thread>
extern "C" {
  #include "nathread.h"
  #include "particle.h"
  #include "integrator.h"
}

class ThreadedIntegratorTest : public ::testing::Test {
 protected:
  Integrator calc;
  int particle_count = 6;
  int pair_count = 15;
  Particle * all_parts;
  ParticlePair pairs[15];
  double pos[6][VECDIM] = {
    {0.0, 0.0},
    {2.0, 0.0},
    {-2.0, 0.0},
    {0.0, 5.0},
    {7.0, 0.0},
    {-6.0, 0.0},
  };
  double vel[6][VECDIM] = {
    {1.0, 1.0},
    {-1.0, 0.0},
    {-1.0, 0.0},
    {0.0, -1.0},
    {-1.0, 0.0},
    {1.0, 0.0},
  };
  double masses[6] = {3.0, 1.0, 1.0, 3.0, 1.0, 1.0};
  double radii[6] = {2.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  double restitutions[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

  double posfinal[6][VECDIM] = {
    {0.0, 1.0},
    {4.0, 0.0},
    {-3.0, 0.0},
    {0.0, 4.0},
    {6.0, 0.0},
    {-5.0, 0.0},
  };
  int32_t worker_count = 2;
  thread_t workers[2];

  virtual void SetUp() {
    int i;
    int j;
    int k;
    all_parts = (Particle *) malloc(particle_count * sizeof(Particle));
    for (i=0; i < particle_count; ++i) {
      init_Particle(all_parts + i, masses[i], radii[i], restitutions[i]);
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

    init_Integrator(&calc, particle_count, all_parts);
  }

  virtual void TearDown() {
    stop_workers(&calc, worker_count);
    cleanup_Integrator(&calc);
  }
};

TEST_F(ThreadedIntegratorTest, workerloop_Test) {
  int i;
  int j;
  Vec fudge;
  void * args = (void *) &calc;
  for (i=0; i < worker_count; ++i) {
    init_thread(workers + i, worker_loop, args);
  }
  for (i=0; i < worker_count; ++i) {
    detach_thread(workers[i]);
  }
  integrate_interval(&calc, 1.0);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  for (i=0; i < particle_count; ++i) {
    multiply_Vec(
      &(calc.particles[i].velocity), 2 * calc.interp_alpha.as_double * calc.dt,
      &fudge
    );
    for (j=0; j < VECDIM; ++j) {
      EXPECT_NEAR(
        posfinal[i][j],
        calc.particles[i].position.components[j].as_double,
        fabs(fudge.components[j].as_double)
      );
    }
  }
}

