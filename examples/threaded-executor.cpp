#include <xzero/executor/ThreadedExecutor.h>

int main() {
  xzero::ThreadedExecutor executor;

  executor.execute([]{printf("task 1\n");});
  executor.execute([]{printf("task 2\n");});
  executor.execute([]{printf("task 3\n");});

  executor.joinAll();

  return 0;
}






















