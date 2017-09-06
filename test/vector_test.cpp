#include <gtest/gtest.h>
extern "C" {
  #include "nathread.h"
  #include "vector.h"
}

class vectorTest : public ::testing::Test {
 protected:
  Vec test_vecs[4];
  double vec0[VECDIM] = {3.0, 4.0};
  double vec1[VECDIM] = {-2.0, -2.0};
  double vec2[VECDIM] = {1.0, 1.0};
  double vec3[VECDIM] = {0.0, 0.0};
  double sum[VECDIM] = {1.0, 2.0};
  double diff[VECDIM] = {5.0, 6.0};
  double norm[VECDIM] = {0.6, 0.8};
  static const int thread_cycles = 100;
  static const int worker_count = 4;
  thread_t workers[4];

  static void threaded_sum(void * vecs) {
    Vec * vectors = (Vec *) vecs;
    int i;
    for (i=0; i < thread_cycles; ++i) {
      atomic_isum_Vec(vectors, vectors + 1);
    }
  }

  static void threaded_diff(void * vecs) {
    Vec * vectors = (Vec *) vecs;
    int i;
    for (i=0; i < thread_cycles; ++i) {
      atomic_idiff_Vec(vectors, vectors + 1);
    }
  }

  static void threaded_mult(void * vecs) {
    Vec * vectors = (Vec *) vecs;
    int i;
    for (i=0; i < thread_cycles; ++i) {
      atomic_imultiply_Vec(vectors, -1.0);
    }
  }

  virtual void SetUp() {
    init_Vec(test_vecs + 0, vec0);
    init_Vec(test_vecs + 1, vec1);
    init_Vec(test_vecs + 2, vec2);
    init_Vec(test_vecs + 3, vec3);
  }

  virtual void TearDown() {
  }
};

TEST_F(vectorTest, sumTest) {
  sum_Vec(test_vecs, test_vecs + 1, test_vecs + 3);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(sum[i], (test_vecs + 3)->components[i].as_double);
  }
}

TEST_F(vectorTest, atomic_isumTest) {
  atomic_isum_Vec(test_vecs, test_vecs + 1);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(sum[i], (test_vecs)->components[i].as_double);
  }
}

TEST_F(vectorTest, diffTest) {
  diff_Vec(test_vecs, test_vecs + 1, test_vecs + 3);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(diff[i], (test_vecs + 3)->components[i].as_double);
  }
}

TEST_F(vectorTest, atomic_idiffTest) {
  atomic_idiff_Vec(test_vecs, test_vecs + 1);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(diff[i], test_vecs->components[i].as_double);
  }
}

TEST_F(vectorTest, dotTest) {
  EXPECT_DOUBLE_EQ(-14.0f, dot_Vec(test_vecs, test_vecs + 1));
}

TEST_F(vectorTest, distTest) {
  EXPECT_DOUBLE_EQ(61.0f, distance_square(test_vecs, test_vecs + 1));
}

TEST_F(vectorTest, magnitudeTest) {
  EXPECT_DOUBLE_EQ(25.0f, magnitude_square(test_vecs));
  EXPECT_DOUBLE_EQ(5.0f, magnitude(test_vecs));
}

TEST_F(vectorTest, normTest) {
  norm_Vec(test_vecs, test_vecs + 3);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(norm[i], (test_vecs + 3)->components[i].as_double);
  }
}

TEST_F(vectorTest, threaded_isumTest) {
  int i;
  for (i=0; i < worker_count; ++i) {
    init_thread(workers + i, threaded_sum, (void *) (test_vecs + 1));
  }
  for (i=0; i < worker_count; ++i) {
    join_thread(workers[i]);
    cleanup_thread(workers[i]);
  }
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(worker_count * thread_cycles - 2, (test_vecs + 1)->components[i].as_double);
  }
}

TEST_F(vectorTest, threaded_idiffTest) {
  int i;
  for (i=0; i < worker_count; ++i) {
    init_thread(workers + i, threaded_diff, (void *) (test_vecs + 1));
  }
  for (i=0; i < worker_count; ++i) {
    join_thread(workers[i]);
    cleanup_thread(workers[i]);
  }
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(-worker_count * thread_cycles - 2, (test_vecs + 1)->components[i].as_double);
  }
}

TEST_F(vectorTest, threaded_imultTest) {
  int i;
  for (i=0; i < worker_count; ++i) {
    init_thread(workers + i, threaded_mult, (void *) (test_vecs + 1));
  }
  for (i=0; i < worker_count; ++i) {
    join_thread(workers[i]);
    cleanup_thread(workers[i]);
  }
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(-2 + 4 * ((worker_count * thread_cycles) % 2) , (test_vecs + 1)->components[i].as_double);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
