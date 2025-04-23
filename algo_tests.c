#include "algo.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { const char *message = test(); \
    if (message) printf ("Test %d failed: %s\n", tests_index, message); \
    else { tests_pass++; } tests_run++; } while (0)
int tests_pass, tests_run, tests_index;

// Auxiliary functions to view boards

void print_board_vertices_index (board * self)
{
  for (size_t i = 0; i < self->size; i++)
    {
      printf ("%lu\n", self->vertices[i]->index);
    }
}

void print_board_edges (board * self)
{
  for (size_t i = 0; i < self->size; i++)
    {
      printf ("Edges for %lu\n", self->vertices[i]->index);
      board_vertex *v = self->vertices[i];
      for (size_t j = 0; j < v->degree; j++)
        {
          printf ("%lu %lu\n", v->index, v->neighbors[j]->index);
        }
    }
}

void print_board_distances (board * self)
{
  for (size_t i = 0; i < self->size; i++)
    {
      for (size_t j = 0; j < self->size; j++)
        {
          printf ("%d ", self->dist[i][j]);
        }
      printf ("\n");
    }
}

static char *test_board_read_from_null_file ()
{
  board b;
  board_create (&b);

  bool read = board_read_from (&b, NULL);

  mu_assert ("error, null file cannot be read", read == false);
  mu_assert ("error, board is not correctly initialized with null file",
             b.vertices == NULL);

  board_destroy (&b);
  return NULL;
}

static char *test_board_read_from_null_board ()
{
  board *b = NULL;

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 0\n0 0\n" "Edges: 2\n0 1\n1 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (b, file);

  mu_assert ("error, null board cannot be initialized", read == false);

  return NULL;
}

static char *test_board_read_from_invalid_edges ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 3\n0 0\n0 0\n0 0\n" "Edges: 2\n0 3\n3 2\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, read should be false with invalid edges", read == false);

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

static char *test_board_is_valid_move_invalid ()
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

static char *test_board_is_valid_move_identical_vertex ()
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

static char *test_board_is_valid_move_false ()
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

static char *test_board_is_valid_move_true ()
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

static char *test_board_is_valid_move_multiple_edges ()
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
  mu_assert ("error with valid move",
             board_is_valid_move (&b, 0, 2) == false);

  board_destroy (&b);
  return NULL;
}

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

static char *test_board_Floyd_Warshall_single ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 1\n0 0\n" "Edges: 0\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error, the distance is incorrect", board_dist (&b, 0, 0) == 0);
  mu_assert ("error, the next is incorrect", board_next (&b, 0, 0) == 0);

  board_destroy (&b);
  return NULL;
}

static char *test_board_Floyd_Warshall_square ()
{
  board b;
  board_create (&b);

  char data[] = "Cops: 1\nRobbers: 1\nMax turn: 1\n"
    "Vertices: 4\n0 0\n0 0\n0 0\n0 0\n" "Edges: 4\n0 1\n1 2\n2 3\n3 0\n";
  FILE *file = tmpfile ();
  fputs (data, file);
  rewind (file);

  bool read = board_read_from (&b, file);

  mu_assert ("error, failure reading board", read == true);
  mu_assert ("error, incorrect distance", board_dist (&b, 0, 0) == 0
             && board_dist (&b, 0, 3) == 1);

  board_destroy (&b);
  return NULL;
}

char *(*tests_functions[]) () = {
  test_board_read_from_null_file,
  test_board_read_from_null_board,
  test_board_read_from_invalid_edges,
  test_board_is_valid_move_null,
  test_board_is_valid_move_invalid,
  test_board_is_valid_move_identical_vertex,
  test_board_is_valid_move_false,
  test_board_is_valid_move_true,
  test_board_is_valid_move_multiple_edges,
  test_board_Floyd_Warshall_chain,
  test_board_Floyd_Warshall_single,
  test_board_Floyd_Warshall_square,
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
