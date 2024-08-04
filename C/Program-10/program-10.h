#pragma once

#include <errno.h>
#include <fuse.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define UNUSED(x) (void)(x)

#define MAX_FILE_NAME 256
#define MAX_FILE_CONTENTS 256
#define MAX_DIR_NAME MAX_FILE_NAME * 2

enum fs_type { FS_DIRECTORY, FS_FILE };

struct fs_instance {
  enum fs_type type;
  union {
    struct {
      char name[MAX_DIR_NAME];
    } dir;
    struct {
      char name[MAX_FILE_NAME];
      char contents[MAX_FILE_CONTENTS];
    } file;
  } data;
};

struct fs_node {
  struct fs_instance item;
  struct fs_node *next;
};

static struct fs_node *head = NULL;

struct fs_node *fs_create_fs_node(const enum fs_type type, const char *name,
                                  const char *contents) {
  struct fs_node *item = (struct fs_node *)malloc(sizeof(struct fs_node));
  item->item.type = type;
  if (type == FS_DIRECTORY) {
    strncpy(item->item.data.dir.name, name, MAX_DIR_NAME);
  } else {
    strncpy(item->item.data.file.name, name, MAX_FILE_NAME);
    strncpy(item->item.data.file.contents, contents, MAX_FILE_CONTENTS);
  }
  item->next = NULL;
  return item;
}

void fs_add_fs_node(const enum fs_type type, const char *name,
                    const char *contents) {
  static struct fs_node *current = NULL;

  struct fs_node *node = fs_create_fs_node(type, name, contents);

  if (head == NULL) {
    head = node;
    current = head;
  } else if (current->next == NULL) {
    current->next = node;
    current = current->next;
  }
}

struct fs_node *get_fs_file(const char *path) {
  for (struct fs_node *current = head; current != NULL; current = current->next)
    if (current->item.type == FS_FILE &&
        strcmp(path, current->item.data.file.name) == 0) {
      return current;
    }
  return NULL;
}

struct fs_node *get_fs_dir(const char *path) {
  for (struct fs_node *current = head; current != NULL; current = current->next)
    if (current->item.type == FS_DIRECTORY &&
        strcmp(path, current->item.data.dir.name) == 0) {
      return current;
    }
  return NULL;
}

void add_dir(const char *dir_name) {
  fs_add_fs_node(FS_DIRECTORY, dir_name, NULL);
}

void add_file(const char *filename) { fs_add_fs_node(FS_FILE, filename, ""); }

int is_dir(const char *path) { return (get_fs_dir(++path) != NULL) ? 1 : 0; }

int is_file(const char *path) { return (get_fs_file(++path) != NULL) ? 1 : 0; }

void write_to_file(const char *path, const char *new_content) {
  struct fs_node *file = get_fs_file(++path);
  if (file != NULL) {
    strncpy(file->item.data.file.contents, new_content, MAX_FILE_CONTENTS);
  }
}
