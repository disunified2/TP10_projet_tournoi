#include "algo.h"

#include <stdlib.h>
#include <limits.h>

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
  if (!fgets (line, sizeof (line), file))
    return false;
  sscanf (line, "Cops: %zu", &(self->cops));
  if (!fgets (line, sizeof (line), file))
    return false;
  sscanf (line, "Robbers: %zu", &(self->robbers));
  if (!fgets (line, sizeof (line), file))
    return false;
  sscanf (line, "Max turn: %zu", &(self->max_turn));
  if (!fgets (line, sizeof (line), file))
    return false;
  sscanf (line, "Vertices: %zu", &(self->size));
  self->vertices = calloc (self->size, sizeof (*self->vertices));
  for (size_t i = 0; i < self->size; i++)
    {
      self->vertices[i] = calloc (1, sizeof (**self->vertices));
      self->vertices[i]->index = i;
      self->vertices[i]->degree = 0;
      self->vertices[i]->neighbors = NULL;
      if (!fgets (line, sizeof (line), file))
        return false;
    }
  size_t edges = 0;
  if (!fgets (line, sizeof (line), file))
    return false;
  sscanf (line, "Edges: %zu", &edges);
  for (size_t i = 0; i < edges; i++)
    {
      size_t v1, v2;
      if (!fgets (line, sizeof (line), file))
        return false;
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
      if (self->dist != NULL)
        {
          free (self->dist[i]);
        }
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
  if (self == NULL)
    {
      return false;
    }

  if (source >= self->size || dest >= self->size)
    {
      return false;
    }

  if (source == dest)
    {
      return true;
    }

  if (self->vertices[source] == NULL || self->vertices[dest] == NULL)
    {
      return false;
    }

  for (size_t i = 0; i < self->size; i++)
    {
      if (self->vertices[i]->degree > dest
          && self->vertices[i]->neighbors[dest] != NULL)
        {
          return true;
        }
    }
  return false;
}

void board_Floyd_Warshall (board * self)
{
  if (self == NULL)
    {
      return;
    }

  self->dist = calloc (self->size, sizeof (unsigned int *));
  self->next = calloc (self->size, sizeof(size_t*));
  for (size_t i = 0; i < self->size; i++)
    {
      self->dist[i] = calloc (self->size, sizeof (unsigned int));
      self->next[i] = calloc (self->size, sizeof (size_t));
    }

  for (size_t u = 0; u < self->size; u++)
    {
      for (size_t v = 0; v < self->size; v++)
        {
          self->dist[u][v] = INT_MAX;
          self->next[u][v] = 0;
        }
    }

  for (size_t u = 0; u < self->size; u++) {
    board_vertex *vertex = self->vertices[u];
    for (size_t i = 0; i < self->size; i++) {
      size_t v = vertex->neighbors[i]->index;
      self->dist[u][v] = 1;
      self->next[u][v] = v;
    }
  }

  for (size_t v = 0; v < self->size; v++) {
    self->dist[v][v] = 0;
    self->next[v][v] = v;
  }

  for (size_t w = 0; w < self->size; w++) {
    for (size_t u = 0; u < self->size; u++) {
      for (size_t v = 0; v < self->size; v++) {
        if (self->dist[u][v] > self->dist[u][w] + self->dist[w][v]) {
          self->dist[u][v] = self->dist[u][w] + self->dist[w][v];
          self->next[u][v] = self->next[u][w];
        }
      }
    }
  }
}

size_t board_dist (board * self, size_t source, size_t dest)
{
  if (self == NULL) {
    return 0;
  }

  if (source >= self->size || dest >= self->size)
  {
    return 0;
  }

  if (source == dest)
  {
    return 0;
  }

  if (self->vertices[source] == NULL || self->vertices[dest] == NULL)
  {
    return 0;
  }

  if (self->dist == NULL) {
    return 0;
  }

  if (self->dist[source][dest]) {
    return 0;
  }

  return self->dist[source][dest];
}

size_t board_next (board * self, size_t source, size_t dest)
{
  if (self == NULL) {
    return 0;
  }

  if (source >= self->size || dest >= self->size)
  {
    return 0;
  }

  if (source == dest)
  {
    return 0;
  }

  if (self->vertices[source] == NULL || self->vertices[dest] == NULL)
  {
    return 0;
  }

  if (self->next == NULL) {
    return 0;
  }

  if (self->next[source][dest]) {
    return 0;
  }

  return self->next[source][dest];
}
