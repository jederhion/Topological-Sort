#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef struct vertex_
{
    int value;   
    struct vertex_* next;
    struct vertex_* prev;
} vertex, * Pvertex;

typedef struct graph
{
    Pvertex* vertex_head;
} graph, * Pgraph;

static graph g1, g2;
static Pgraph head, tail;
static int num_vertices;
//static int* indegree;


void insert(Pvertex* head, Pvertex* tail, int data)
{       
    Pvertex new_node = (vertex*)malloc(sizeof(vertex));

    if (!new_node)
    {
        perror("Error");
        return;
    }

    new_node->value = data;
    new_node->next = NULL;    

    if (!*head) 
        *head = new_node;
    else 
        (*tail)->next = new_node;
    
    new_node->prev = *tail;
    *tail = new_node;    
}


void init_graph()
{
    int i;

    g1.vertex_head = (Pvertex*)malloc(sizeof(Pvertex) * num_vertices);
    g2.vertex_head = (Pvertex*)malloc(sizeof(Pvertex) * num_vertices);

    if (!g1.vertex_head || !g2.vertex_head)
    {
        fprintf(stderr, "error: unable to allocate resources\n");
        exit(-1);
    }

    for (i = 0; i < num_vertices; i++)
    {
        g1.vertex_head[i] = NULL;
        g2.vertex_head[i] = NULL;
    }       

    head = &g1;
    tail = &g2;
}


void create_graph(char* input_file)
{    
    char *token, str[1000];
    int i = 0, j, integer;
    
   
    FILE* file = fopen(input_file, "r");  // Open the input file for reading
   
    if (!file)
    {
        perror("Error");
        exit(-1);
    }
   
    while (fgets(str, sizeof(str), file))
    {        
        token = strtok(str, " \n");
        while (token)
        {
            integer = atoi(token);
            insert(&head->vertex_head[i], &tail->vertex_head[i], integer);
          
            token = strtok(NULL, " \n"); // get the next token
        }
        i++;
    }

   
    fclose(file);  // Close the input file
}

void print_graph()
{
    int j;
    Pvertex current;

    for (j = 0; j < num_vertices; j++)
    {      
        current = head->vertex_head[j];
        while (current)
        {
            printf("%d", current->value);
            current = current->next;
            if (current)
                printf(" ");
        }
        printf("\n");
    }
}


void topological_sort()
{
    int next_vertex = 0, front = 0, back = 0, i, j;
    int* sorted_vertices = malloc(sizeof(int) * num_vertices);
    int* queue = malloc(sizeof(int) * num_vertices);
    int* indegree = (int*)malloc(sizeof(int) * num_vertices);
    Pvertex cursor;    

    memset(indegree, 0, sizeof(int) * (num_vertices+1));

    int vertex_sum = 0, edge_sum = 0; // counting
    int* vertex_ops = malloc(sizeof(int) * num_vertices);  // counting
    int** edge_ops = malloc(sizeof(int*) * num_vertices); // counting

    if (!sorted_vertices || !queue || !vertex_ops || !edge_ops)
    {
        fprintf(stderr, "error: unable to allocate resources\n");
        exit(-1);
    }

    // for loop to initialize the counting arrays
    for (i = 0; i < num_vertices; i++)
    {
        vertex_ops[i] = 0;
        edge_ops[i] = malloc(sizeof(int) * num_vertices);

        if (!edge_ops[i])
        {
            fprintf(stderr, "error: unable to allocate resources\n");
            exit(-1);
        }

        for (j = 0; j < num_vertices; j++)
        {
            edge_ops[i][j] = 0;
        }
    }

    for (j = 0; j < num_vertices; j++)
    {
        cursor = head->vertex_head[j];
        vertex_ops[j] += 2;                 // counting
        while (cursor->next)
        {
            cursor = cursor->next;
            indegree[cursor->value]++;           
            edge_ops[j][cursor->value - 1] += 3;        // counting
        }
    }

    //int sum = 0;

    //for (i = 0; i < num_vertices; i++)          //to get |E|
    //{
    //    printf("%d:  %d\n", i + 1, indegree[i + 1]);
    //    sum += indegree[i + 1];
    //}

    //printf("%d Edges\n\n", sum);
        


    for (i = 1; i <= num_vertices; i++)   // Initialize the queue with all vertices that have indegree 0 
    {        
        if (indegree[i] == 0)
        {
            queue[back++] = i;
            vertex_ops[i - 1]++;        // counting         
        } 
        vertex_ops[i - 1] += 2;           // counting
    }

    

    while (front < back)
    {
        int u = queue[front];           // counting
        
        sorted_vertices[next_vertex++] = queue[front];        
        cursor = head->vertex_head[queue[front++] - 1];  

        vertex_ops[u - 1] += 3;         // counting

        while (cursor->next)
        {
            cursor = cursor->next;

            indegree[cursor->value]--;

            int v = cursor->value;      // counting

            if (indegree[cursor->value] == 0)
            {
                queue[back++] = cursor->value;
                vertex_ops[v - 1]++;        // counting
            }  
           
            edge_ops[u - 1][v - 1] += 4;  // counting         
        }
    }

    if (front == num_vertices)
    {
        for (i = 0; i < num_vertices; i++)
        {
            printf("%d ", sorted_vertices[i]);  
            vertex_ops[i] += 2;             // counting
        }          
        
        printf("\n\n");
    }
    else
        printf("This graph has a cycle! \n");    
    

    // the remaining codes are for counting operations
    for (i = 0; i < num_vertices; i++)
    {
        if (num_vertices == 15)
            printf("Vertex %d: %d operations charged to Vertex %d.\n", i + 1, vertex_ops[i], i + 1);
        vertex_sum += vertex_ops[i];        
    } 
    printf("\n");


    for (i = 0; i < num_vertices; i++)
    {
        for (j = 0; j < num_vertices; j++)
        {
            edge_sum += edge_ops[i][j];
            if (edge_ops[i][j] && num_vertices == 15)
                printf("Edge from Vertex number %d to Vertex number %d: %d operations charged to this edge.\n", i+1, j+1, edge_ops[i][j]);
        }
    }

    printf("Total number of operations charged to all vertices is: %d \n", vertex_sum);
    printf("Total number of operations charged to all edges is: %d\n", edge_sum);
    printf("Total number of operations is : %d\n", edge_sum + vertex_sum);
}

void sigsegv_handler(int sig)
{   
    fprintf(stderr, "error: incorrect number of vertices entered for this file\n");   
    exit(-1);
}

int main(int argc, char* argv[])
{  

    signal(SIGSEGV, sigsegv_handler);

    if (argc < 2)
    {
        printf("usage:  graph <options> <graph file>\n");
        printf("\t-v <v>: \tset vertices number to <v>\n");
        exit(-1);
    }

    int arg_index = 1;

    if (!strcmp(argv[arg_index], "-v"))    
        num_vertices = atoi(argv[arg_index + 1]);    
    else
    {
        printf("error:  unrecognized flag %s\n", argv[arg_index]);
        exit(-1);
    }

    init_graph();
    create_graph(argv[arg_index + 2]);  

    topological_sort();

    //print_graph();
  
    return 0;
}
