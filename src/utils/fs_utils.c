#include "fs_utils.h"
#include "kmalloc.h"
#include "string.h"
#include "print.h"

void dprint_sb(StringBuilder *sb, dprint_verbosity_t verb);
 
/* converts the string into malloc'd file path pointer list */
FilePath *split_fpath(char *str, char delim) {
    int idx = 0, curr_len = 0;
    FilePath *first = NULL, *last = NULL, *tmp = NULL;
    if (str == NULL) {
        return NULL;
    }
    while(str[idx] != '\0') {
        if (str[idx] == delim) {
            if (curr_len > 0) {
                tmp = kmalloc(sizeof(FilePath));
                tmp->name = kmalloc(curr_len+1);
                memcpy(tmp->name, (char *) (str+idx-curr_len), curr_len);
                tmp->name[curr_len] = '\0';
                if (first == NULL) {
                    first=tmp;
                    last=tmp;
                } else {
                    last->next=tmp;
                    last=tmp;
                }
            }
            curr_len=-1;
        }
        curr_len++;
        idx++;
    }
    return first;
}

/* 
inserts new_val into pile such that prev->idx <= new_val->idx <= next->idx 
 returns value of new pile 
 assumes pile is correctly formatted 
 */
StringBuilder *insert_sb(StringBuilder *pile, char *new_val, int idx) {
    StringBuilder *curr=NULL, *new_node=NULL, *prev=NULL;

    if (new_val == NULL) {
        printk("insert_sb: new_val is NULL\n");
        return pile;
    } 
    // initialize node for new_val
    new_node = kmalloc(sizeof(StringBuilder));
    new_node->tot_char_ct = strlen(new_val);
    new_node->segment_len = new_node->tot_char_ct;
    new_node->idx = idx;
    new_node->next = NULL;

    new_node->segment = kmalloc(new_node->tot_char_ct + 1);
    new_node->segment[new_node->tot_char_ct] = '\0';
    strcpy(new_node->segment, new_val);

    // insert new node into pile
    if (pile == NULL) {
        pile = new_node;
    } else {
        curr = pile;
        while (curr != NULL) {
            if (curr->idx > new_node->idx) {
                // update and return
                new_node->next = curr;
                new_node->tot_char_ct += new_node->next->tot_char_ct;
                if (curr == pile) {
                    pile = new_node;
                } else {
                    prev->next = new_node;
                }   
                return pile;
            } else {
                curr->tot_char_ct += new_node->tot_char_ct;
                prev = curr;
                curr = curr->next;
            }
        }
        // if at the end, append
        prev->next = new_node;
    }
    return pile;
}

// debug print for string builder
void dprint_sb(StringBuilder *sb, dprint_verbosity_t verb) {
    dprintk(verb, "<<< stringbuilder debug >>>\n");
    while (sb != NULL) {
        dprintk(verb, "... seg #%d: %s len: %ld, tot: %ld\n", sb->idx, sb->segment, sb->segment_len, sb->tot_char_ct);
        sb = sb->next;
    }
}

/* return the malloc'd string represented by sb and free sb */
char *build_string(StringBuilder *sb) {
    char *new_string = NULL;
    StringBuilder *temp_sb;
    uint64_t idx = 0;
    if (sb == NULL) {
        return NULL;
    }
    new_string = kmalloc(sb->tot_char_ct + 1);
    new_string[sb->tot_char_ct] = '\0';
    while (sb != NULL) {
        // build string
        memcpy(new_string+idx, sb->segment, sb->segment_len);
        idx += sb->segment_len;
        
        // increment and free structs 
        temp_sb=sb;
        sb=sb->next;
        kfree(temp_sb->segment);
        kfree(temp_sb);
    }
    return new_string;
}