#include "algo.h"

#include <stdlib.h>

void board_create (board * self)
{
  if (self == NULL)
    {
      return;
    }
  self->size = 0;
  self->cops = 0;
  self->robbers = 0;
  self->max_turn = 0;

  self->vertices = NULL;
  self->dist = NULL;
  self->next = NULL;
}

/*
 * Auxiliary function to factorize code
 */
void board_add_edge_uni (board_vertex * source, board_vertex * destination)
{
  source->degree++;
  source->neighbors =
    realloc (source->neighbors,
             source->degree * sizeof (*(source->neighbors)));
  source->neighbors[source->degree - 1] = destination;
}

bool board_read_from (board * self, FILE * file)
{
  char line[128];
  fgets (line, sizeof (line), file);
  sscanf (line, "Cops: %zu", &(self->cops));
  fgets (line, sizeof (line), file);
  sscanf (line, "Robbers: %zu", &(self->robbers));
  fgets (line, sizeof (line), file);
  sscanf (line, "Max turn: %zu", &(self->max_turn));
  fgets (line, sizeof (line), file);
  sscanf (line, "Vertices: %zu", &(self->size));
  self->vertices = calloc (self->size, sizeof (*self->vertices));
  for (size_t i = 0; i < self->size; i++)
    {
      self->vertices[i] = calloc (1, sizeof (**self->vertices));
      self->vertices[i]->index = i;
      self->vertices[i]->degree = 0;
      self->vertices[i]->neighbors = NULL;
      fgets (line, sizeof (line), file);
    }
  size_t edges = 0;
  fgets (line, sizeof (line), file);
  sscanf (line, "Edges: %zu", &edges);
  for (size_t i = 0; i < edges; i++)
    {
      size_t v1, v2;
      fgets (line, sizeof (line), file);
      sscanf (line, "%zu %zu", &v1, &v2);
      board_add_edge_uni (self->vertices[v1], self->vertices[v2]);
      board_add_edge_uni (self->vertices[v2], self->vertices[v1]);
    }
  return true;
}

void board_vertex_destroy (board_vertex * self)
{
  free (self->neighbors);
  free (self->optim);
  self->neighbors = NULL;
  self->optim = NULL;
}

void board_destroy (board * self)
{
  if (self == NULL)
    {
      return;
    }

  for (size_t i = 0; i < self->size; i++)
    {
      board_vertex_destroy (self->vertices[i]);
      free (self->vertices[i]);
    }
  free (self->dist);
  free (self->next);
  free (self->vertices);
  self->dist = NULL;
  self->next = NULL;
  self->vertices = NULL;
}

bool board_is_valid_move (board * self, size_t source, size_t dest)
{
  //TODO dummy instruction that MUST be changed
  return self->size == source + dest;
}

void board_Floyd_Warshall (board * self)
{
  //TODO dummy instruction that MUST be changed
  if (self->size)
    return;
}

size_t board_dist (board * self, size_t source, size_t dest)
{
  //TODO dummy instruction that MUST be changed
  return self->size + source + dest;
}

size_t board_next (board * self, size_t source, size_t dest)
{
  //TODO dummy instruction that MUST be changed
  return self->size + source + dest;
}
