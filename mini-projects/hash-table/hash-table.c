#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义哈希表中的条目（键值对）
typedef struct HashItem {
    char* key;        // 键，这里使用字符串作为键
    void* value;      // 值，使用 void* 可以存储任意类型的值
    struct HashItem* next; // 解决冲突的链表中的下一个条目
} HashItem;

// 定义哈希表结构
typedef struct HashTable {
    int size;           // 哈希表的大小（容量）
    HashItem** table;    // 指向条目指针的指针数组，即哈希表本身
} HashTable;

// 创建哈希表
HashTable* createHashTable(int size) {
    HashTable* hashTable = (HashTable*)malloc(sizeof(HashTable));
    if (hashTable == NULL) {
        perror("Failed to allocate memory for hash table");
        return NULL;
    }

    hashTable->size = size;
    hashTable->table = (HashItem**)calloc(size, sizeof(HashItem*));
    if (hashTable->table == NULL) {
        perror("Failed to allocate memory for hash table entries");
        free(hashTable);
        return NULL;
    }

    return hashTable;
}

// 哈希函数（简单的字符串哈希）
unsigned int hash(char* key, int size) {
    unsigned int hashVal = 0;
    for (; *key != '\0'; key++) {
        hashVal = (hashVal * 31) + *key;
    }
    return hashVal % size;
}

// 插入键值对
void insert(HashTable* hashTable, char* key, void* value) {
    int index = hash(key, hashTable->size);

    // 创建新的哈希条目
    HashItem* newItem = (HashItem*)malloc(sizeof(HashItem));
    if (newItem == NULL) {
        perror("Failed to allocate memory for hash item");
        return;
    }

    newItem->key = strdup(key);
    if (newItem->key == NULL) {
        perror("Failed to allocate memory for key");
        free(newItem);
        return;
    }
    newItem->value = value;
    newItem->next = NULL;

    // 处理冲突（链式地址法）
    if (hashTable->table[index] == NULL) {
        // 该索引处为空，直接插入
        hashTable->table[index] = newItem;
    } else {
        // 该索引处已经有元素，将新元素添加到链表的头部
        newItem->next = hashTable->table[index];
        hashTable->table[index] = newItem;
    }
}

// 查找键对应的值
void* find(HashTable* hashTable, char* key) {
    int index = hash(key, hashTable->size);

    HashItem* current = hashTable->table[index];

    // 遍历链表，查找键
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // 找到了键
            return current->value;
        }
        current = current->next; // 移动到链表中的下一个条目
    }

    // 没有找到键
    return NULL;
}

// 删除键值对
void delete(HashTable* hashTable, char* key) {
    int index = hash(key, hashTable->size);

    HashItem* current = hashTable->table[index];
    HashItem* prev = NULL;

    // 遍历链表，查找要删除的键
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // 找到了要删除的键

            if (prev == NULL) {
                // 要删除的是链表的头部
                hashTable->table[index] = current->next;
            } else {
                // 要删除的不是链表的头部
                prev->next = current->next;
            }

            free(current->key); // 释放键字符串的内存
            free(current);       // 释放条目的内存
            return;
        }

        prev = current;
        current = current->next;
    }
}

// 释放哈希表
void freeHashTable(HashTable* hashTable) {
    if (hashTable == NULL) return;

    for (int i = 0; i < hashTable->size; i++) {
        HashItem* current = hashTable->table[i];
        while (current != NULL) {
            HashItem* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }

    free(hashTable->table);
    free(hashTable);
}

int main() {
    // 创建一个大小为 10 的哈希表
    HashTable* hashTable = createHashTable(10);

    // 插入一些键值对
    insert(hashTable, "name", "John");
    insert(hashTable, "age", "30");
    insert(hashTable, "city", "New York");

    // 查找键对应的值
    char* name = (char*)find(hashTable, "name");
    char* age = (char*)find(hashTable, "age");
    char* city = (char*)find(hashTable, "city");

    printf("Name: %s\n", name);
    printf("Age: %s\n", age);
    printf("City: %s\n", city);

    // 删除键值对
    delete(hashTable, "age");

    // 再次查找 age，应该返回 NULL
    age = (char*)find(hashTable, "age");
    printf("Age after deletion: %s\n", age == NULL ? "NULL" : age);

    // 释放哈希表
    freeHashTable(hashTable);

    return 0;
}
