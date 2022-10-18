#include <unistd.h>

int main(int argc, char **argv)
{
    write(1, "writer_test\n", sizeof("writer_test\n") - 1);
}

