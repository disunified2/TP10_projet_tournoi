#include "algo.h"

#include <stdlib.h>

/*
 * Update positions of either cops or robbers and return false if the
 * moves are invalid. If both a cop and a robber are on the same
 * vertex, the robber must be removed.
 */
bool board_update_position (board * self, size_t *position, enum role r)
{
  if (r == COPS)
    for (size_t i = 0; i < self->cops_len; i++)
      self->cops[i] = position[i];
  else
    for (size_t i = 0; i < self->robbers_len; i++)
      self->robbers[i] = position[i];
  return true;
}

/*
 * Returns the initial positions of either the cops or the robbers
 */
size_t *board_initial_position (board * self, enum role r)
{
  if (r == COPS)
    for (size_t i = 0; i < self->cops_len; i++)
      self->cops[i] = i;
  else
    for (size_t i = 0; i < self->robbers_len; i++)
      self->robbers[i] = self->size - i - 1;
  return r == COPS ? self->cops : self->robbers;
}

/*
 * Returns the next positions of either the cops or the robbers
 */
size_t *board_next_position (board * self, enum role r)
{
  return r == COPS ? self->cops : self->robbers;
}

void print_positions (size_t *pos, size_t len)
{
  for (size_t i = 0; i < len; i++)
    printf ("%zu\n", pos[i]);
}

size_t *read_positions (size_t len)
{
  size_t *pos = calloc (len, sizeof (*pos));
  for (size_t i = 0; i < len; i++)
    {
      char buffer[100];
      fgets (buffer, sizeof buffer, stdin);
      sscanf (buffer, "%zu", &pos[i]);
    }
  return pos;
}

int main (int argc, const char *argv[])
{
  // Initialize data structures
  if (argc != 3)
    {
      fprintf (stderr,
               "Incorrect number of arguments: ./game filename 0/1\n");
      exit (-1);
    }
  board b;
  FILE *file = fopen (argv[1], "r");
  if (file == NULL)
    {
      fprintf (stderr, "Error opening input file");
      exit (-1);
    }
  bool success = board_read_from (&b, file);
  fclose (file);
  if (!success)
    {
      fprintf (stderr, "Error parsing input file");
      exit (-1);
    }
  enum role curr = atoi (argv[2]);

  // Determine initial positions
  fprintf (stderr, "Initial positions:\n");
  size_t *pos;
  if (curr == COPS)
    {
      pos = board_initial_position (&b, curr);
      print_positions (pos, b.cops_len);
      pos = read_positions (b.robbers_len);
      board_update_position (&b, pos, ROBBERS);
      free (pos);
    }
  else
    {
      pos = read_positions (b.cops_len);
      board_update_position (&b, pos, COPS);
      free (pos);
      pos = board_initial_position (&b, curr);
      print_positions (pos, b.robbers_len);
    }

  // Play each turn
  enum role turn = COPS;
  while (b.robbers_len != 0 && b.remaining_turn != 0)
    {
      fprintf (stderr, "Turn for %s (remaining: %zu)\n",
               turn == COPS ? "cops" : "robbers", b.remaining_turn);
      if (turn == curr)
        {
          pos = board_next_position (&b, curr);
          print_positions (pos, curr == COPS ? b.cops_len : b.robbers_len);
        }
      else
        {
          pos = read_positions (curr != COPS ? b.cops_len : b.robbers_len);
          board_update_position (&b, pos, turn);
          free (pos);
        }
      turn = turn == COPS ? ROBBERS : COPS;
      b.remaining_turn--;
    }

  // Finalization
  if (b.robbers_len != 0)
    fprintf (stderr, "Robbers win !\n");
  else
    fprintf (stderr, "Cops win !\n");
  board_destroy (&b);
}
