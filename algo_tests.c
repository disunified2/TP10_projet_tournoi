#include "algo.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { const char *message = test(); \
    if (message) printf ("Test %d failed: %s\n", tests_index, message); \
    else { tests_pass++; } tests_run++; } while (0)
int tests_pass, tests_run, tests_index;

static char *test_board_Floyd_Warshall_chain ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 0\n0 0\n" "Edges: 2\n0 1\n1 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error, incorrect distance", board_dist (&b, 0, 0) == 0
             && board_dist (&b, 0, 1) == 1 && board_dist (&b, 0, 2) == 2
             && board_dist (&b, 1, 2) == 1);
  mu_assert ("error, incorrect next vertex", board_next (&b, 0, 0) == 0
             && board_next (&b, 0, 1) == 1 && board_next (&b, 0, 2) == 1
             && board_next (&b, 1, 2) == 2);

  board_destroy (&b);

  return NULL;
}

static char *test_board_is_valid_move_null ()
{
  board *b = NULL;

  mu_assert ("error, valid move with null board should return false",
             board_is_valid_move (b, 1, 2) == false);
  return NULL;
}

static char *test_board_is_valid_invalid ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 1\n1 0\n" "Edges: 2\n0 1\n1 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error, expected false with invalid source and destination",
             board_is_valid_move (&b, 10, 10) == false);

  board_destroy (&b);
  return NULL;
}

static char *test_board_is_valid_identical_vertex ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 1\n1 0\n" "Edges: 2\n0 1\n1 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error, staying on the same vertex is a valid move",
             board_is_valid_move (&b, 0, 0) == true);

  board_destroy (&b);
  return NULL;
}

static char *test_board_is_valid_false ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 1\n1 0\n" "Edges: 2\n0 1\n1 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error with inexistant edge",
             board_is_valid_move (&b, 0, 2) == false);

  board_destroy (&b);
  return NULL;
}

static char *test_board_is_valid_true ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 1\n1 0\n" "Edges: 2\n0 1\n1 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error with valid move", board_is_valid_move (&b, 0, 1) == true);

  board_destroy (&b);
  return NULL;
}

char *(*tests_functions[]) () = {
  //test_board_Floyd_Warshall_chain,
  test_board_is_valid_move_null,
  test_board_is_valid_invalid,
  test_board_is_valid_identical_vertex,
  test_board_is_valid_false,
  test_board_is_valid_true,
};

int main (int argc, const char *argv[])
{
  size_t n = sizeof (tests_functions) / sizeof (tests_functions[0]);
  if (argc == 1)
    {
      for (tests_index = 0; (size_t) tests_index < n; tests_index++)
        mu_run_test (tests_functions[tests_index]);
      if (tests_run == tests_pass)
        printf ("All %d tests passed\n", tests_run);
      else
        printf ("Tests passed/run: %d/%d\n", tests_pass, tests_run);
    }
  else
    {
      tests_index = atoi (argv[1]);
      if (tests_index < 0)
        printf ("%zu\n", n);
      else if ((size_t) tests_index < n)
        {
          mu_run_test (tests_functions[tests_index]);
          if (tests_run == tests_pass)
            printf ("Test %d passed\n", tests_index);
        }
    }
}
