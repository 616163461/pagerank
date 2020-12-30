#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <math.h>
#include <err.h>
#include <stdbool.h>
#include <sysexits.h>
#include <sys/types.h>
#include "pagerank.h"
#define MAX 100


//
int main(int argc, char *argv[])
{
    PageRankW(atof(argv[1]),  atof(argv[2]),  atof(argv[3]));
    return 0;
}
// part1 main function
void PageRankW(float d, float diffPR, float maxIterations){
	// create url list
	FileList urllist = getfilelist();
	// get the num of files
	int num_of_files = numforfiles(urllist);
	// create graph
	Graph gurl = generategraph(urllist,num_of_files);
	// calculations
	float *i = calPR(gurl,d,diffPR,maxIterations);
	PRList pr_list = createPRList(gurl,i);
	// print out the pr list
	print_out_PR(pr_list);
	return;
}



int numforfiles(FileList l){
	int count = 0;
	FileList curr = l;
	while(curr != NULL){
		curr = curr->next;
		count++;
	}
	return count;
}


FileList getfilelist(){
	FILE *f;
	char savenamefile[MAX];
	FileList l = NULL;
	if ((f = fopen("collection.txt","r")) == NULL) {
        fprintf(stderr, "Can't open file collection.txt\n");
        return NULL;
	}
    while(fscanf(f, "%s", savenamefile) != EOF){
        char * filename = malloc((strlen(savenamefile)+1)*sizeof(char));
        strcpy(filename, savenamefile);
        strcat(filename, "\0");
        l = addtofilelist(l, filename);
    }
    fclose(f);
    return l;
}


FileList addtofilelist(FileList l, char *filesname){
	FileList new = newFileNode(filesname);
	if(l == NULL){
		l = new;
		return l;
	}
	else{
		FileList curr = l;
		while(curr->next != NULL){
			curr = curr->next;
		}
		curr->next = new;
	}
	return l;
}

char *filedir(char *filecollection){
	char *file_dir = malloc((strlen(filecollection)+5)*sizeof(char));
    strcpy(file_dir, filecollection);
    strcat(file_dir, ".txt");
    strcat(file_dir, "\0");
    return file_dir;
}


FileList newFileNode(char *filesname){
 	FileList new = malloc(sizeof(struct FileListNode));
  	assert(new != NULL);
  	new->filename = malloc(strlen(filesname)*sizeof(char) + 1);
  	strcpy(new->filename, filesname);
  	new->next = NULL;
  	return new;
}





//


Graph generategraph(FileList l, int numoffiles){
    Graph newgraph = newGraph((size_t)numoffiles);
    FILE *f;
    char infileword[MAX];
    FileList curr = l;
    while(curr != NULL){
        if ((f = fopen(filedir(curr->filename),"r")) == NULL) {
            fprintf(stderr, "Can't open file %s\n", curr->filename);
            return NULL;
    	}
        rewind(f);
        int i = 2;
        while(fscanf(f, "%s", infileword) != EOF && i>0){
            if((strcmp(infileword, "#end") == 0) || (strcmp(infileword, "#start") == 0)){
           
            }
            else if(strcmp(infileword, "Section-1") == 0){
                i--;
            }
            else{
                if(strcmp(curr->filename, infileword) != 0 ){
                    addEdge(newgraph, curr->filename, infileword);
                }
            }
        }
        fclose(f);
        curr = curr->next;
    }
    return newgraph;
}


Graph newGraph (size_t V){
	Graph new = malloc (sizeof *new);
	if (new == NULL)
		err (EX_OSERR, "couldn't allocate Graph");
	*new = (GraphRep){.nV = 0, .V = V};
	new->vertex = calloc (V, sizeof (char *));
	if (new->vertex == NULL)
		err (EX_OSERR, "calloc");
	new->edges = calloc (V, sizeof (unsigned char *));
	if (new->edges == NULL)
		err (EX_OSERR, "calloc");
	for (size_t i = 0; i < V; i++) {
		new->edges[i] = calloc (V, sizeof (unsigned char));
		if (new->edges[i] == NULL)
			err (EX_OSERR, "calloc");
		for (size_t j = 0; j < V; j++)
			new->edges[i][j] = 0;
	}
	return new;
}


void addEdge (Graph g, char *src, char *dest){
	assert(g!=NULL);
	ssize_t v = getvertex (src, g->vertex, g->nV);
	if (v < 0) {
		if (g->nV >= g->V){
			return;
		}
		v = (ssize_t) addVertex (src, g->vertex, g->nV);
		g->nV++;
	}
	ssize_t w = getvertex (dest, g->vertex, g->nV);
	if (w < 0) {
		if (g->nV >= g->V){
			return;
		}
		w = (ssize_t) addVertex (dest, g->vertex, g->nV);
		g->nV++;
	}
	g->edges[v][w] = 1;
	return;
}

// return 1 if they are connected
bool connect (Graph g, char *src, char *dest){
	assert (g != NULL);
	ssize_t i = getvertex (src, g->vertex, g->nV);
	ssize_t j = getvertex (dest, g->vertex, g->nV);
	if (i < 0 || j < 0){
		return 0;
	}
	return g->edges[i][j] == 1;
}


size_t nVertices (Graph g){
	assert (g != NULL);
	return g->nV;
}

size_t addVertex (char *str, char **arr, size_t num){
    arr[num] = malloc(strlen(str)+1);
    assert(arr[num]!=NULL);
    strcpy(arr[num], str);
    return num;
}

ssize_t getvertex (char *str, char **arr, size_t num){
	for (size_t i = 0; i < num; i++)
		if(strcmp(str,arr[i]) == 0){
			return (ssize_t) i;
		}
	return -1;
}


void freeGraph(Graph g){
	if( g == NULL){
		return;
	}
	size_t i = 0;
	size_t j = 0;
	while(i < g->nV){
		free(g->vertex[i]);
		i++;
	}
	while(j < g->V){
		free(g->edges[j]);
		j++;
	}
	free(g->vertex);
	free(g->edges);
	free(g);
}



//

float *setPR(Graph g){
    float *set =  malloc((g->nV)*sizeof(float));
    int i = 0;
    while(i<g->nV){
        set[i] = 1/((float)(g->nV));
        i++;
    }
    return set;
}

// calculate Win
float calwin(Graph g, int src, int dest){
	float win = 0;
	float m = 0;
	int i = 0;
	while(i<g->nV){
		if(connect(g,g->vertex[src],g->vertex[i])){
			m = m + calindegree(g,i);
		}
		i++;
	}
	float n = calindegree(g,dest);
	return win = n/m;
}

// calculate Wout
float calwout(Graph g, int src, int dest){
	float wout = 0;
	float n = caloutdegree(g,dest);
	if(n == 0){
		n = 0.5;
	}
	float m = 0;
	int i = 0;
	while(i < g->nV){
		if(connect(g,g->vertex[src],g->vertex[i])){
			if(caloutdegree(g,i)!=0){
				m = m + caloutdegree(g,i);
			}
			else{
				m = 0.5 + m;
			}
		}
		i++;
	}
	return wout = n/m;
}

// calculate in degree
int calindegree(Graph g, int i){
	int m = 0;
	int counter = 0;
	while(m<g->nV){
		if(g->edges[m][i] == 1){
			counter++;
		}
		m++;
	}
	return counter;
}

// calculate out degree
int caloutdegree(Graph g, int i){
	int m = 0;
	int counter = 0;
	while(m<g->nV){
		if(g->edges[i][m] == 1){
			counter++;
		}
		m++;
	}
	return counter;
}

// use pr fomula
float PRfomula(Graph g, float m, float *n, int prnum){
	float firstpart = (1-m)/((float)(g->nV));
	int i = 0;
	float secondpart = 0;
	while(i<g->nV){
		if(connect(g,g->vertex[i],g->vertex[prnum])){
			secondpart = n[i]*calwin(g,i,prnum)*calwout(g,i,prnum) + secondpart;
		}
		i++;
	}
	return firstpart + secondpart*m;
}

// calculate diff
float caldiff(Graph g, float *n, float *curr){
	float diff = 0;
	int i = 0;
	// compare with prev
	while(i<g->nV){
		if((curr[i]-n[i])<0){
            diff = diff - (curr[i]-n[i]);
        }
        else if((curr[i]-n[i])>=0){
            diff = diff + (curr[i]-n[i]);
        }
        i++;
	}
	return diff;
}

//calcaulate pr
float *calPR(Graph g, float d, float diffPR, float maxIterations){
    float iteration = 0;
    float diff = diffPR;
	float *prev =  setPR(g);
    float *curr =  setPR(g);
    while(iteration < maxIterations && diff >= diffPR){
        int i = 0;
        while(i<g->nV){
            curr[i] = PRfomula(g, d, prev, i);
            i++;
        }
        diff = caldiff(g, prev, curr);
        int q = 0;
        while(q<g->nV){
            prev[q] = curr[q];
            q++;
        }
        iteration++;
    }
    return curr;
}




//

// new pr node
PRList newPRNode(char *file_name, int od, float id){
	PRList new = malloc(sizeof(struct PRNode));
	assert(new != NULL);
	new->filename = malloc(strlen(file_name)*sizeof(char) + 1);
	strcpy(new->filename, file_name);
	new->outdegree = od;
	new->pr = id;
	new->next = NULL;
	return new;
}

// create pr list
PRList createPRList(Graph g, float *pr){
	PRList l = NULL;
	int m = 0;
	while(m < g->nV){
		l = insertPRNode(l,g,pr,m);
		m++;
	}
	return l;
}

// descending order pr list
PRList insertPRNode(PRList l, Graph g, float *pr, int index){
	int i = caloutdegree(g,index);
	PRList new = newPRNode(g->vertex[index],i,pr[index]);
	PRList curr = l;
	if(curr == NULL){
		l = new;
	}
	else{
		if(new->pr > curr->pr){
			l = new;
			l->next = curr;
		}
		else{
			while(curr != NULL){
				if(curr->next == NULL){
					curr->next = new;
					break;
				}
				else if(new->pr > curr->next->pr){
					new->next = curr->next;
					curr->next = new;
					break;
				}
				curr = curr->next;
			}
		}
	}
	return l;
}

// print out pagerank list to file pageranklist.txt
void print_out_PR(PRList l){
	if(l == NULL){
		return;
	}
	// write to the document
	FILE *output;
	output = fopen("pagerankList.txt","w");
	// loop through prlist
	while(l != NULL){
		fprintf(output, "%s, %d, %.7f\n", l->filename, l->outdegree, l->pr);
		l = l->next;
	}
	fclose(output);
	return;
}	
