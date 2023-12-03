#include "AssignmentApp.h"

int main(int argc, char *argv[]) {
  AssignmentApp application("assignment", "1.0");

  application.Init();
  application.Run();

  return 0;
}
