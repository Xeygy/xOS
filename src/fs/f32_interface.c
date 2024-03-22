#include "fat32.h"
#include "fs_utils.h"
#include "string.h"
#include "kmalloc.h"
#include "print.h"
#include "md5.h"

#include <stdint.h>
#include <stddef.h>

int open_cb(char *name, FATDirent *dirent, void *fp_and_file);

typedef struct two_args {
    void *arg1, *arg2;
} two_args;


/* 
opens the file at pathname for reading
and returns it, null if it doesn't exist 
*/
File *ropen(const char *pathname) {
    FilePath *fp = NULL;
    File *f = NULL;
    char *cpy = NULL;
    two_args fp_and_file = {};

    cpy = kmalloc(strlen(pathname));
    strcpy(cpy, pathname);
    fp = split_fpath(cpy, '/');
    kfree(cpy);

    fp_and_file.arg1 = fp;
    fp_and_file.arg2 = &f;

    fat32_readdir(FAT_ROOT_CLUSTER, open_cb, &fp_and_file);
    free_fpath(fp);
    return f;
}

/* helper callback for ropen */
int open_cb(char *name, FATDirent *dirent, void *fp_and_file) {
    FilePath *next = NULL, *curr=NULL;
    File *tgt = NULL;
    File **ret_val = NULL;
    two_args *args = fp_and_file, *new_args=NULL;
    if (args == NULL || args->arg1 == NULL || args->arg2 == NULL) {
        return -1;
    }
    curr = ((FilePath *) args->arg1);
    ret_val = ((File **) args->arg2);
    if (curr->next == NULL) {
        // look for file
        if (!(dirent->attr & FAT_ATTR_DIRECTORY) && 
        strcmp(name, curr->name) == 0) {  
            tgt = open(dirent);
            *ret_val = tgt;
        }
    } else if ((dirent->attr & FAT_ATTR_DIRECTORY) && 
                strcmp(name, curr->name) == 0) {
            // traverse directories
            next = curr->next;
            new_args = kmalloc(sizeof(two_args));
            new_args->arg1 = next;
            new_args->arg2 = args->arg2;
            fat32_readdir((dirent->cluster_hi << 16) + dirent->cluster_lo, open_cb, new_args);
            kfree(new_args);
    } 
    return 1;
}

/* read file at filepath */
void f32_print_contents(const char *pathname, int num_bytes) {
    char *contents = NULL;
    File *f = NULL;
    if (pathname == NULL) {
        printk("f32_print_contents: pathname null\n");
        return;
    }
    f = ropen(pathname);
    if (f == NULL) {
        printk("file not found: %s\n", pathname);
        return;
    }

    contents = kmalloc(num_bytes+1);
    memset(contents, 0, num_bytes+1);
    printk("read %d bytes\n", f->read(f, contents, num_bytes));
    f->close(&f);
    printk("%s contents: %s", pathname, contents);
    kfree(contents);
}

/* 
prints the md5 hash of the file at pathname.
assumes file contents is one long string (i.e. no zero values)
*/
void f32_md5(const char *pathname) {
    char *contents = NULL, *full = NULL;
    uint8_t hash[16];
    File *f = ropen(pathname);
    int idx = 0;
    
    if (pathname == NULL) {
        printk("f32_md5: pathname null\n");
        return;
    }
    f = ropen(pathname);
    if (f == NULL) {
        printk("file not found: %s\n", pathname);
        return;
    }

    // calculate md5
    md5File(f, hash);
    for (idx = 0; idx<16; idx++) {
        if (hash[idx] < 0x10)
            printk("0");
        printk("%x", hash[idx]);
    }

    kfree(full);
    kfree(contents);
}