#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>


struct PRNode {
    char *filename;
	float pr;
    int outdegree;
	struct PRNode *next;
};
typedef struct PRNode *PRList;

struct FileListNode {
    char *filename;
	struct FileListNode *next;
};
typedef struct FileListNode *FileList;

typedef struct GraphRep {
	size_t nV;
	size_t V;
	char **vertex;
	unsigned char **edges;
} GraphRep;
typedef struct GraphRep *Graph;


// functions for graph
Graph newGraph (size_t V);
void freeGraph (Graph g);
void addEdge (Graph g, char *src, char *dest);
bool connect (Graph g, char *src, char *dest);
size_t nVertices (Graph g);
ssize_t getvertex (char *str, char **arr, size_t num);
size_t addVertex (char *, char **, size_t);
Graph generategraph(FileList l, int numoffiles);



// functions for filelist
char * filedir(char *collection);
FileList getfilelist(); // from collection file
FileList newFileNode(char *filesname);
FileList addtofilelist(FileList l, char *filesname);
int numforfiles(FileList l);


// functions for PR
PRList newPRNode(char *file_name, int od, float id);
PRList createPRList(Graph g, float *pr_indexes);
PRList insertPRNode(PRList l, Graph g, float *pr, int index);




// part1 main function
void PageRankW(float d, float diffPR, float maxIterations);
// print out result to file
void print_out_PR(PRList l);
// calculations
int calindegree(Graph g, int i);
int caloutdegree(Graph g, int i);
float *setPR(Graph g);
float calwin(Graph g, int src, int dest);
float calwout(Graph g, int src, int dest);
float PRfomula(Graph g, float m, float *n, int prnum);
float caldiff(Graph g, float *n, float *curr);
float *calPR(Graph g, float d, float diffPR, float maxIterations);





