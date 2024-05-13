#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PID_SIZE 10005

struct Node{
    int pid;
    int childs_num;
    __pid_t childs[PID_SIZE];
}nodes[PID_SIZE];

int _root;
int pid_num = 0;
int sparse[PID_SIZE];

__pid_t getPpid(__pid_t pid) {
    char filename[50];
    sprintf(filename, "/proc/%d/status", pid);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Can't open %s", filename);
        return 0;
    }

    __pid_t ppid = 0;
        char line[30];
    while(fgets(line, sizeof(line), fp)) {
        if (strstr(line, "PPid:") == line) {
            sscanf(line, "PPid:\t%d", &ppid);
            break;
        }
    }

    fclose(fp);
    return ppid;
}

int sparsePid(int pid) {
    int _sparse = sparse[pid];
    if (_sparse == -1) {
        sparse[pid] = ++pid_num;
        return pid_num;
    }
    return _sparse;
}

void createTree() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");

    if (dir == NULL) {
        printf("Can't open dir proc");
        return;
    }

    memset(sparse, -1, sizeof(sparse));
    while((entry = readdir(dir)) != NULL) {
        int pid = 0;
        if ((pid = atoi(entry->d_name)) != 0) {
            __pid_t ppid = getPpid(pid);
            // printf("PID: %s, PPID %d\n", entry->d_name, ppid);
            // 离散化
            int _pid = sparsePid(pid);
            int _ppid = sparsePid(ppid);
            int _num = nodes[_ppid].childs_num;
            nodes[_pid].pid = pid;
            nodes[_ppid].pid = ppid;
            nodes[_ppid].childs[_num] = _pid;
            nodes[_ppid].childs_num = _num + 1;
            // root pid = 0
            if (ppid == 0) {
                _root = _ppid;
            }
        }

        //if (pid_num > 4) break;
    }

    // debug tree
    // for (int i = 1; i <= pid_num; i++) {
    //     for (int j = 0; j < nodes[i].childs_num; j++) {
    //         int _child = nodes[i].childs[j];
    //         printf("parent: %d, child: %d\n", nodes[i].pid, nodes[_child].pid);
    //     }
    // }

    closedir(dir);
}

void dfs(int pid, int depth) {
    if (pid == 0) {
        return;
    }
    for (int i = 0; i < depth; i++) {
        printf("\t");
    }
    printf("PID: %d\n", nodes[pid].pid);
    for (int i = 0; i < nodes[pid].childs_num; i++) {
        int _pid = nodes[pid].childs[i];
        dfs(_pid, depth + 1);
    }
}

// pid = 0 pstree
int main(int argc, char *argv[]) {
    createTree();
    dfs(_root, 0);
    return 0;
}
