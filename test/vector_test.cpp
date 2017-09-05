#include <gtest/gtest.h>
extern "C" {
  #include "nathread.h"
  #include "vector.h"
}

class vectorTest : public ::testing::Test {
 protected:
  Vec * vec1;
  Vec * vec2;
  Vec * vec3;
  Vec * result;
  double sum[2] = {1.0, 2.0};
  double diff[2] = {5.0, 6.0};
  double norm[2] = {0.6, 0.8};
  const static int thread_cycles = 100;
  const static int worker_count = 4;
  thread_t workers[4];

  static void threaded_sum(void * vecs) {
    Vec ** vectors = (Vec **) vecs;
    int i;
    for (i=0; i < thread_cycles; ++i) {
      atomic_isum_Vec(*(vectors), *(vectors + 1));
    }
  }

  static void threaded_diff(void * vecs) {
    Vec ** vectors = (Vec **) vecs;
    int i;
    for (i=0; i < thread_cycles; ++i) {
      atomic_idiff_Vec(*(vectors), *(vectors + 1));
    }
  }

  static void threaded_mult(void * vecs) {
    Vec ** vectors = (Vec **) vecs;
    int i;
    for (i=0; i < thread_cycles; ++i) {
      atomic_imultiply_Vec(*(vectors), -1.0);
    }
  }

  virtual void SetUp() {
    vec1 = create_Vec();
    vec2 = create_Vec();
    vec3 = create_Vec();
    result = create_Vec();
    vec1->components[0].as_double = 3.0;
    vec1->components[1].as_double = 4.0;
    vec2->components[0].as_double = -2.0;
    vec2->components[1].as_double = -2.0;
    vec3->components[0].as_double = 1.0;
    vec3->components[1].as_double = 1.0;
  }

  virtual void TearDown() {
    free_Vec(vec1);
    free_Vec(vec2);
    free_Vec(vec3);
    free_Vec(result);
  }
};

TEST_F(vectorTest, sumTest) {
  sum_Vec(vec1, vec2, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(sum[i], result->components[i].as_double);
  }
}

TEST_F(vectorTest, atomic_isumTest) {
  atomic_isum_Vec(vec1, vec2);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(sum[i], vec1->components[i].as_double);
  }
}

TEST_F(vectorTest, diffTest) {
  diff_Vec(vec1, vec2, result);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(diff[i], result->components[i].as_double);
  }
}

TEST_F(vectorTest, atomic_idiffTest) {
  atomic_idiff_Vec(vec1, vec2);
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(diff[i], vec1->components[i].as_double);
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

TEST_F(vectorTest, threaded_isumTest) {
  int i;
  Vec ** vectors = (Vec **) malloc(2 * sizeof(Vec *));
  vectors[0] = vec2;
  vectors[1] = vec3;
  for (i=0; i < worker_count; ++i) {
    workers[i] = create_thread(threaded_sum, (void *) vectors);
  }
  for (i=0; i < worker_count; ++i) {
    join_thread(workers[i]);
    cleanup_thread(workers[i]);
  }
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(worker_count * thread_cycles - 2, vec2->components[i].as_double);
  }
  free(vectors);
}

TEST_F(vectorTest, threaded_idiffTest) {
  int i;
  Vec ** vectors = (Vec **) malloc(2 * sizeof(Vec *));
  vectors[0] = vec2;
  vectors[1] = vec3;
  for (i=0; i < worker_count; ++i) {
    workers[i] = create_thread(threaded_diff, (void *) vectors);
  }
  for (i=0; i < worker_count; ++i) {
    join_thread(workers[i]);
    cleanup_thread(workers[i]);
  }
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(-worker_count * thread_cycles - 2, vec2->components[i].as_double);
  }
  free(vectors);
}

TEST_F(vectorTest, threaded_imultTest) {
  int i;
  Vec ** vectors = (Vec **) malloc(sizeof(Vec *));
  vectors[0] = vec2;
  for (i=0; i < worker_count; ++i) {
    workers[i] = create_thread(threaded_mult, (void *) vectors);
  }
  for (i=0; i < worker_count; ++i) {
    join_thread(workers[i]);
    cleanup_thread(workers[i]);
  }
  for (int i = 0; i < VECDIM; ++i) {
    EXPECT_DOUBLE_EQ(-2 + 4 * ((worker_count * thread_cycles) % 2) , vec2->components[i].as_double);
  }
  free(vectors);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
