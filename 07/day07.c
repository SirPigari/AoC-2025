#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_GRID_SIZE 1000
#define MAX_SIGNATURE_LEN 10000
#define HASH_TABLE_SIZE 1048576  // 2^20 for hash table

typedef struct {
    int row;
    int col;
    int sig_len;
    char signature[MAX_SIGNATURE_LEN];
} Path;

typedef struct {
    Path *data;
    int size;
    int capacity;
} PathList;

typedef struct HashNode {
    char *signature;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode **buckets;
    int size;
} HashSet;

char grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
int height = 0;
int width = 0;

// Fast hash function
static inline unsigned int hash_string(const char *str, int len) {
    unsigned int hash = 5381;
    for (int i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + str[i];
    }
    return hash % HASH_TABLE_SIZE;
}

HashSet* create_hash_set() {
    HashSet *set = malloc(sizeof(HashSet));
    set->buckets = calloc(HASH_TABLE_SIZE, sizeof(HashNode*));
    set->size = 0;
    return set;
}

bool hash_set_contains(HashSet *set, const char *str, int len) {
    unsigned int hash = hash_string(str, len);
    HashNode *node = set->buckets[hash];
    
    while (node != NULL) {
        if (memcmp(node->signature, str, len) == 0 && node->signature[len] == '\0') {
            return true;
        }
        node = node->next;
    }
    return false;
}

bool hash_set_add(HashSet *set, const char *str, int len) {
    unsigned int hash = hash_string(str, len);
    HashNode *node = set->buckets[hash];
    
    // Check if already exists
    while (node != NULL) {
        if (memcmp(node->signature, str, len) == 0 && node->signature[len] == '\0') {
            return false;
        }
        node = node->next;
    }
    
    // Add new node
    HashNode *new_node = malloc(sizeof(HashNode));
    new_node->signature = malloc(len + 1);
    memcpy(new_node->signature, str, len);
    new_node->signature[len] = '\0';
    new_node->next = set->buckets[hash];
    set->buckets[hash] = new_node;
    set->size++;
    return true;
}

void free_hash_set(HashSet *set) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode *node = set->buckets[i];
        while (node != NULL) {
            HashNode *next = node->next;
            free(node->signature);
            free(node);
            node = next;
        }
    }
    free(set->buckets);
    free(set);
}

PathList* create_path_list(int initial_capacity) {
    PathList *list = malloc(sizeof(PathList));
    list->data = malloc(sizeof(Path) * initial_capacity);
    list->size = 0;
    list->capacity = initial_capacity;
    return list;
}

static inline void add_path(PathList *list, int row, int col, const char *signature, int sig_len) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->data = realloc(list->data, sizeof(Path) * list->capacity);
    }
    Path *p = &list->data[list->size];
    p->row = row;
    p->col = col;
    p->sig_len = sig_len;
    memcpy(p->signature, signature, sig_len);
    p->signature[sig_len] = '\0';
    list->size++;
}

void free_path_list(PathList *list) {
    free(list->data);
    free(list);
}

int part2(int start_col) {
    PathList *paths = create_path_list(1000);
    add_path(paths, 0, start_col, "", 0);
    
    int timeline_count = 0;
    HashSet *final_states = create_hash_set();
    
    while (paths->size > 0) {
        PathList *new_paths = create_path_list(paths->size * 2);
        
        for (int i = 0; i < paths->size; i++) {
            Path *p = &paths->data[i];
            int next_row = p->row + 1;

            if (p->row % 10 == 0)
                printf("At position (%d, %d) with signature '%s'\n", p->row, p->col, p->signature);
            
            if (next_row >= height) {
                if (hash_set_add(final_states, p->signature, p->sig_len)) {
                    timeline_count++;
                }
                continue;
            }
            
            char next_cell = grid[next_row][p->col];
            
            if (next_cell == '^') {
                char new_sig[MAX_SIGNATURE_LEN];
                int new_len = p->sig_len + 1;
                memcpy(new_sig, p->signature, p->sig_len);
                
                if (p->col > 0) {
                    new_sig[p->sig_len] = 'L';
                    add_path(new_paths, next_row, p->col - 1, new_sig, new_len);
                }
                
                if (p->col < width - 1) {
                    new_sig[p->sig_len] = 'R';
                    add_path(new_paths, next_row, p->col + 1, new_sig, new_len);
                }
            } else {
                char new_sig[MAX_SIGNATURE_LEN];
                memcpy(new_sig, p->signature, p->sig_len);
                new_sig[p->sig_len] = 'D';
                add_path(new_paths, next_row, p->col, new_sig, p->sig_len + 1);
            }
        }
        
        free_path_list(paths);
        paths = new_paths;
    }
    
    free_path_list(paths);
    free_hash_set(final_states);
    
    return timeline_count;
}

int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        printf("Error opening file\n");
        return 1;
    }
    
    char line[MAX_GRID_SIZE];
    while (fgets(line, sizeof(line), file)) {
        int len = strlen(line);
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len == 0) continue;
        
        memcpy(grid[height], line, len + 1);
        if (width == 0) width = len;
        height++;
    }
    fclose(file);
    
    int start_col = 0;
    for (int col = 0; col < width; col++) {
        if (grid[0][col] == 'S') {
            start_col = col;
            break;
        }
    }
    
    int result = part2(start_col);
    printf("Part 2: %d\n", result);
    
    return 0;
}