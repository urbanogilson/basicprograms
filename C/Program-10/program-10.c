#define FUSE_USE_VERSION 31

#include "program-10.h"

#include <errno.h>
#include <fuse.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static int do_getattr(const char *path, struct stat *st) {
  st->st_uid = getuid();
  st->st_gid = getgid();
  st->st_atime = time(NULL);
  st->st_mtime = time(NULL);

  if (strcmp(path, "/") == 0 || is_dir(path) == 1) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2;  // Why "two" hardlinks instead of "one"? The answer is
                       // here: http://unix.stackexchange.com/a/101536
  } else if (is_file(path) == 1) {
    st->st_mode = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size = 1024;
  } else {
    return -ENOENT;
  }

  return 0;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi) {
  UNUSED(offset);
  UNUSED(fi);

  filler(buffer, ".", NULL, 0);
  filler(buffer, "..", NULL, 0);

  if (strcmp(path, "/") == 0) {
    for (struct fs_node *current = head; current != NULL;
         current = current->next) {
      if (current->item.type == FS_DIRECTORY) {
        filler(buffer, current->item.data.dir.name, NULL, 0);
      } else if (current->item.type == FS_FILE) {
        filler(buffer, current->item.data.file.name, NULL, 0);
      }
    }
  }

  return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset,
                   struct fuse_file_info *fi) {
  UNUSED(fi);

  struct fs_node *file = get_fs_file(++path);

  if (file == NULL) return -1;

  char *content = file->item.data.file.contents;

  memcpy(buffer, content + offset, size);

  return strlen(content) - offset;
}

static int do_mkdir(const char *path, mode_t mode) {
  UNUSED(mode);
  add_dir(++path);
  return 0;
}

static int do_mknod(const char *path, mode_t mode, dev_t rdev) {
  UNUSED(mode);
  UNUSED(rdev);
  add_file(++path);
  return 0;
}

static int do_write(const char *path, const char *buffer, size_t size,
                    off_t offset, struct fuse_file_info *info) {
  UNUSED(offset);
  UNUSED(info);
  write_to_file(path, buffer);
  return size;
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,
    .read = do_read,
    .mkdir = do_mkdir,
    .mknod = do_mknod,
    .write = do_write,
};

int main(int argc, char *argv[]) {
  return fuse_main(argc, argv, &operations, NULL);
}