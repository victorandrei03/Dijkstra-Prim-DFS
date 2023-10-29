/* PREDA Victor-Andrei - 311CC */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct node{
	char *obj_name;
	struct node *next;
	int index;
	int weight;
	int island;
	int depth;
	float score;
}node;

typedef struct node_pq{
	struct node_pq *next;
	int min;
	int index;
	float score;
	int depth;
}node_pq;

typedef struct PriorityQueue{
	struct node_pq *head;
}PriorityQueue;

typedef struct Tlist{
	node *head;
	node *tail;
	int number_elements;
}Tlist;

typedef struct graph{
	Tlist **list;
	int number_nodes;
}graph;

typedef struct node_path{
	int index;
	struct node_path *next;
	struct node_path *prev;
}node_path;

typedef struct List_path{
	node_path *head;
	node_path *tail;
}List_path;

//	function in which I create a new node for the graph with the name of the
//	objective
node *create_node(char *str){
	node *new_node = (node *) calloc(1, sizeof(node));
	new_node->obj_name = (char *) calloc(strlen(str) + 1, sizeof(char));
	strcpy(new_node->obj_name, str);
	new_node->next = NULL;
	return new_node;
}

//	function in which I store in a priority queue a node at its increasing
//	position compared to the others (this function is made for the distance
//	between nodes)
void enqueue_min(PriorityQueue **pq, int min, int index) {
	node_pq *new_node = (node_pq *)calloc(1, sizeof(node_pq));
	new_node->index = index;
	new_node->min = min;

	if ((*pq)->head == NULL) {
		(*pq)->head = new_node;
		return;
	}

	node_pq *prev_node_pq = NULL;
	node_pq *curr_node_pq = (*pq)->head;

	while (curr_node_pq != NULL && min > curr_node_pq->min) {
		prev_node_pq = curr_node_pq;
		curr_node_pq = curr_node_pq->next;
	}

	if (curr_node_pq == (*pq)->head) {
		new_node->next = (*pq)->head;
		(*pq)->head = new_node;
	}
	else {
		new_node->next = curr_node_pq;
		prev_node_pq->next = new_node;
	}
}

//	this function has the same principle as above, except that it depends on the
//	score
void enqueue_score(PriorityQueue **pq, float score, int index,
				   int weight, int depth) {
	node_pq *new_node = (node_pq *)calloc(1, sizeof(node_pq));
	new_node->index = index;
	new_node->score = score;
	new_node->min = weight;
	new_node->depth = depth;

	if ((*pq)->head == NULL) {
		(*pq)->head = new_node;
		return;
	}

	node_pq *prev_node_pq = NULL;
	node_pq *curr_node_pq = (*pq)->head;

	while (curr_node_pq != NULL && score >  curr_node_pq->score) {
		prev_node_pq = curr_node_pq;
		curr_node_pq = curr_node_pq->next;
	}

	if (curr_node_pq == (*pq)->head) {
		new_node->next = (*pq)->head;
		(*pq)->head = new_node;
	}
	else {
		new_node->next = curr_node_pq;
		prev_node_pq->next = new_node;
	}
}

//	function in which I return the first element from the priority queue, the
//	first being the minimum
node_pq *dequeue(PriorityQueue **pq){
	if ((*pq)->head == NULL){
		return NULL;
	}
	node_pq *removed_node = (*pq)->head;
	(*pq)->head = (*pq)->head->next;
	return removed_node;
}

//	function in which I look for the index for a certain node in the graph by
//	name (I go through the vector of lists and check on the first element)
int find_node(graph *map, char *str, int count){
	for (int i = 0; i < count; i++){
		if (strcmp(map->list[i]->head->obj_name, str) == 0){
			return i;
		}
	}
	return -1;
}

//
//function in which I read each link from the file in order, assign an index to
// each read node and to the respective position in the vector, add the first
// element to the list for the second position and vice versa
graph* initialize_graph(int objectives, int roads, FILE *fp){
	graph *map = (graph *) calloc(1, sizeof(graph));
	map->number_nodes = objectives;
	map->list = (Tlist **) calloc(objectives, sizeof(Tlist *));
	for (int i = 0; i < objectives; i++){
		map->list[i] = (Tlist *) calloc(1, sizeof(Tlist));
		map->list[i]->head = NULL;
		map->list[i]->number_elements = 1;
		map->list[i]->tail = NULL;
	}

	char obj1[15];
	char obj2[15];
	int weight;

	int count = 0;
	for (int i = 0; i < roads; i++){
		fscanf(fp, "%s %s %d\n", obj1, obj2, &weight);

		node *first_obj = create_node(obj1);
		node *second_obj = create_node(obj2);

		int index_first = find_node(map, obj1, count);
		int index_second = find_node(map, obj2, count);
		second_obj->weight = weight;
		first_obj->weight = weight;
		second_obj->island = 0;
		first_obj->island = 0;

		if (index_first == -1){
			map->list[count]->head = create_node(obj1);
			map->list[count]->head->index = count;
			map->list[count]->tail = map->list[count]->head;
			map->list[count]->head->island = 0;
			map->list[count]->head->weight = weight;
			index_first = count;
			count++;
		}

		if (index_second == -1){
			map->list[count]->head = create_node(obj2);
			map->list[count]->head->index = count;
			map->list[count]->tail = map->list[count]->head;
			map->list[count]->head->island = 0;
			map->list[count]->head->weight = weight;
			index_second = count;
			count++;
		}

		second_obj->index = index_second;
		map->list[index_first]->tail->next = second_obj;
		map->list[index_first]->tail = second_obj;
		map->list[index_first]->number_elements += 1;

		first_obj->index = index_first;
		map->list[index_second]->tail->next = first_obj;
		map->list[index_second]->tail = first_obj;
		map->list[index_second]->number_elements += 1;


	}
	return map;
}

//	function in which I implement the dfs algorithm
void dfs(graph *map, int position, int *visited, int *components, int *count){
	visited[position] = 1;
	components[position] = *count;
	node *curr_node = map->list[position]->head;
	while (curr_node != NULL){
		if (visited[curr_node->index] == 0){
			dfs(map, curr_node->index, visited, components, count);
		}
		curr_node = curr_node->next;
	}
}

//	function in which I apply the dfs algorithm to find out how many nodes are
//	not connected to each other
int *find_isles(graph *map, int *count, int *visited){
	int *components = (int *) calloc(map->number_nodes, sizeof(int));
	for (int i = 0; i < map->number_nodes; i++){
		if (visited[i] == 0){
			*count = *count + 1;
			dfs(map, i, visited, components, count);
		}
	}
	return components;
}

//	function in which I insert in my priority queue all the nodes that have not
//	been visited depending on the node I am at with the current iteration
PriorityQueue *addEdge(graph *isles, PriorityQueue *pq, int index, int *visited){
	node *curr_node = isles->list[index]->head;
	visited[index] = 1;
	while (curr_node != NULL){
		if (visited[curr_node->index] == 0) {
			enqueue_min(&pq, curr_node->weight, curr_node->index);
		}
		curr_node = curr_node->next;
	}
	return pq;
}

//	function to free the queue
void free_queue(PriorityQueue **queue){
	node_pq *node_current = (*queue)->head;
	while (node_current != NULL) {
		node_pq *aux = node_current;
		node_current = node_current->next;
		free(aux);
	}
	free(*queue);
}

//	function to free a list
void free_list(List_path **list_path){
	node_path *node_current = (*list_path)->head;
	while (node_current != NULL) {
		node_path *aux = node_current;
		node_current = node_current->next;
		free(aux);
	}
	free(*list_path);
}

//	function the free the graph
void free_graph(graph **map){
	for (int i = 0; i < (*map)->number_nodes; i++){
		node *curr_node = (*map)->list[i]->head;
		while (curr_node != NULL){
			node *aux = curr_node;
			curr_node = curr_node->next;
			free(aux->obj_name);
			free(aux);
		}
		free((*map)->list[i]);
	}
	free((*map)->list);
	free(*map);
}

// function in which I implement the Prim algorithm, using the addEdge function
// described above; this function is called on a vector of
// graphs (each island) and returns the minimum path for each island,
// being stored in a vector
int *min_costs_isles(graph *isles, int isles_count){
	int *min_cost_isles = (int *) calloc(isles_count, sizeof(int));

	for (int i = 0; i < isles_count; i++) {
		PriorityQueue *pq = (PriorityQueue *) calloc(1, sizeof(PriorityQueue));
		pq->head = NULL;
		int *visited = (int *) calloc(isles[i].number_nodes, sizeof(int));
		int size = isles[i].number_nodes;
		visited[0] = 1;
		int index = 0;
		int edge_count = 0;
		pq = addEdge(&isles[i], pq, index, visited);
		node_pq *curr_node = NULL;

		while (pq->head != NULL && edge_count < size - 1) {

			curr_node = dequeue(&pq);
			index = curr_node->index;
			if (visited[index]){
				free(curr_node);
				continue;
			}
			min_cost_isles[i] += curr_node->min;

			pq = addEdge(&isles[i], pq, index, visited);
			edge_count++;
			free(curr_node);
		}
		free_queue(&pq);
		free(visited);
	}
	return min_cost_isles;
}

//	function in which I create my vector of graphs, for each graph the nodes
//  will be indexed from 0
graph *build_isles(graph *map, int count_isles, int *connected_obj){
	graph *isles = (graph *) calloc(count_isles, sizeof(graph));
	for (int i = 0; i < count_isles; i++){
		isles[i].list = (Tlist **) calloc (connected_obj[i], sizeof(Tlist *));
		isles[i].number_nodes = connected_obj[i];
		for (int j = 0; j < connected_obj[i]; j++){
			isles[i].list[j] = (Tlist *) calloc (1, sizeof(Tlist));
		}
		int count = 0;
		for (int j = 0; j < map->number_nodes; j++){
			if (map->list[j]->head->island - 1 == i){
				isles[i].list[count]->head = map->list[j]->head;
				isles[i].list[count]->tail = map->list[j]->tail;
				isles[i].list[count]->head->index = count;
				count++;
			}
		}
		for (int j = 0; j < isles[i].number_nodes; j++){
			node *curr_node = isles[i].list[j]->head;
			while (curr_node != NULL){
				for (int k = 0; k < isles[i].number_nodes; k++){
					if (strcmp(curr_node->obj_name,
							   isles[i].list[k]->head->obj_name) == 0){
						curr_node->index = isles[i].list[k]->head->index;
					}
				}
				curr_node = curr_node->next;
			}
		}
	}
	return isles;
}

// similar function for creating the first graph, only that I no longer add the
// second element to the list, the first element being a "directed graph"; I
// also go through each node separately to assign it the respective score and
// depth
graph* initialize_graph_t2(int objectives, int roads, FILE *fp,
						   int *total_weight, int *island_node, int *ship_node){

	graph *map = (graph *) calloc(1, sizeof(graph));
	map->number_nodes = objectives;
	map->list = (Tlist **) calloc(objectives, sizeof(Tlist *));
	for (int i = 0; i < objectives; i++){
		map->list[i] = (Tlist *) calloc(1, sizeof(Tlist));
		map->list[i]->head = NULL;
		map->list[i]->number_elements = 1;
		map->list[i]->tail = NULL;
	}

	char obj1[15];
	char obj2[15];
	int weight;

	int count = 0;
	for (int i = 0; i < roads; i++){
		fscanf(fp, "%s %s %d\n", obj1, obj2, &weight);

		node *second_obj = create_node(obj2);

		int index_first = find_node(map, obj1, count);
		int index_second = find_node(map, obj2, count);
		second_obj->weight = weight;
		second_obj->island = 0;

		if (index_first == -1){
			map->list[count]->head = create_node(obj1);
			map->list[count]->head->index = count;
			map->list[count]->head->weight = weight;
			map->list[count]->tail = map->list[count]->head;
			map->list[count]->head->island = 0;
			index_first = count;
			count++;
		}

		if (index_second == -1){
			map->list[count]->head = create_node(obj2);
			map->list[count]->head->index = count;
			map->list[count]->head->weight = weight;
			map->list[count]->tail = map->list[count]->head;
			map->list[count]->head->island = 0;
			index_second = count;
			count++;
		}

		second_obj->index = index_second;
		map->list[index_first]->tail->next = second_obj;
		map->list[index_first]->tail = second_obj;
		map->list[index_first]->number_elements += 1;

	}
	for (int i = 0; i < objectives; i++){
		int depth, index = 0;
		if (strcmp(map->list[i]->head->obj_name, "Insula") == 0){
			*island_node = map->list[i]->head->index;
		}
		if (strcmp(map->list[i]->head->obj_name, "Corabie") == 0){
			*ship_node = map->list[i]->head->index;
		}
		fscanf(fp, "%s %d\n", obj1, &depth);
		for (int j = 0; j < objectives; j++){
			if (strcmp(obj1, map->list[j]->head->obj_name) == 0){
				index = j;
				break;
			}
		}
		map->list[index]->head->depth = depth;
	}
	for (int i = 0; i < objectives; i++){
		node *curr_node = map->list[i]->head;
		while (curr_node != NULL){
			curr_node->depth = map->list[curr_node->index]->head->depth;
			float curr_weight = (float) curr_node->weight;
			float curr_depth = (float) curr_node->depth;
			curr_node->score = curr_weight / curr_depth;
			curr_node = curr_node->next;
		}
	}
	fscanf(fp, "%d", total_weight);
	return map;
}

//	function in which I implement the dijkstra algorithm depending on the score
//	and return a vector that represents the distance from the start node to each
//	individual node; and here I still use the same priority queue
int *dijkstra(graph *map, int start, int *prev){
	PriorityQueue *pq = (PriorityQueue *) calloc (1, sizeof(PriorityQueue));
	int *dist = (int *) calloc(map->number_nodes, sizeof(int));
	float *dist_sc = (float *) calloc(map->number_nodes, sizeof(float));
	int *visited = (int *) calloc(map->number_nodes, sizeof(int));
	for (int i = 0; i < map->number_nodes; i++){
		dist[i] = INT_MAX;
		dist_sc[i] = (float)INT_MAX;
		prev[i] = -1;
		visited[i] = 0;
	}
	dist[start] = 0;
	dist_sc[start] = 0;

	enqueue_score(&pq, map->list[start]->head->score,
				  map->list[start]->head->index,
				  map->list[start]->head->weight,
				  map->list[start]->head->depth);

	while (pq->head != NULL){
		node_pq *curr_node = dequeue(&pq);
		visited[curr_node->index] = 1;
		node *node_iterator = map->list[curr_node->index]->head;

		while (node_iterator != NULL){
			if (visited[node_iterator->index] != 0){
				node_iterator = node_iterator->next;
				continue;
			}
			int new_dist = dist[curr_node->index] + node_iterator->weight;
			float new_dist_sc = dist_sc[curr_node->index] + node_iterator->score;

			if (new_dist_sc < dist_sc[node_iterator->index]){
				dist[node_iterator->index] = new_dist;
				dist_sc[node_iterator->index] = new_dist_sc;
				prev[node_iterator->index] = curr_node->index;
				enqueue_score(&pq, node_iterator->score,
							  node_iterator->index,
							  node_iterator->weight,
							  node_iterator->depth);
			}
			node_iterator = node_iterator->next;
		}
		free(curr_node);
	}
	free(visited);
	free(dist_sc);
	free_queue(&pq);
	return dist;
}

//	function in which I create a list that represents the path from a node to
// another, each node having an index parameter; at the end I rotate the list
List_path *shortest_path(graph *map, int island_node, int ship_node, int *prev){
	int *dist = dijkstra(map, island_node, prev);
	if (dist[ship_node] == INT_MAX){
		return NULL;
	}
	List_path *path_aux = (List_path *) calloc(1, sizeof (List_path));
	path_aux->head = NULL;
	for (int i = ship_node; i != -1; i = prev[i]){
		node_path *new_node = (node_path *) calloc (1, sizeof (node_path));
		new_node->index = i;
		if (path_aux->head == NULL){
			path_aux->head = new_node;
			path_aux->tail = path_aux->head;
			path_aux->head->prev = NULL;
			path_aux->head->next = NULL;
		}
		else{
			path_aux->tail->next = new_node;
			new_node->prev = path_aux->tail;
			path_aux->tail = new_node;
		}
	}
	List_path *path = (List_path *) calloc(1, sizeof (List_path));
	node_path *curr_node = path_aux->tail;
	while (curr_node != NULL){
		node_path *new_node = (node_path *) calloc (1, sizeof (node_path));
		new_node->index = curr_node->index;
		if (path->head == NULL){
			path->head = new_node;
			path->tail = path->head;
			path->head->prev = NULL;
			path->head->next = NULL;
		}
		else{
			path->tail->next = new_node;
			new_node->prev = path->tail;
			path->tail = new_node;
		}
		curr_node = curr_node->prev;
	}
	free_list(&path_aux);
	free(dist);
	return path;
}

int main(int argc, char **argv)
{
	FILE *fp = fopen("tema3.in", "r");
	if (fp == NULL){
		return -1;
	}

	FILE *fp_out = fopen("tema3.out", "w");
	if(fp_out == NULL){
		return -1;
	}

	int objectives, roads;
	graph *map;
	fscanf(fp, "%d %d\n", &objectives, &roads);

	if (strcmp(argv[1], "1") == 0) {
		map = initialize_graph(objectives, roads, fp);

		int *visited = (int *) calloc(map->number_nodes, sizeof(int));
		int count_isles = 0;
		int *components = find_isles(map, &count_isles, visited);
		int *connected_objectives = (int *) calloc(count_isles, sizeof(int));

		for (int i = 0; i < objectives; i++) {
			connected_objectives[components[i] - 1] += 1;
			map->list[i]->head->weight = 0;
			map->list[i]->head->island = components[i];
		}
		fprintf(fp_out, "%d\n", count_isles);

		graph *isles = build_isles(map, count_isles, connected_objectives);
		int *min_cost_isles = min_costs_isles(isles, count_isles);
		for (int i = 0; i < count_isles - 1; i++){
			for (int j = 0; j < count_isles - i - 1; j++){
				if (min_cost_isles[j] > min_cost_isles[j + 1]){
					int aux = min_cost_isles[j];
					min_cost_isles[j] = min_cost_isles[j + 1];
					min_cost_isles[j + 1] = aux;
				}
			}
		}

		for (int i = 0; i < count_isles; i++) {
			fprintf(fp_out, "%d\n", min_cost_isles[i]);
		}

		for (int i = 0; i < count_isles; i++){
			for(int j = 0; j < isles[i].number_nodes; j++){
				free(isles[i].list[j]);
			}
				free(isles[i].list);
		}
		free(visited);
		free(min_cost_isles);
		free(connected_objectives);
		free(components);
		free(isles);
		free_graph(&map);
	}

	else{
		int total_weight;
		int island_node, ship_node;
		map = initialize_graph_t2(objectives, roads, fp, &total_weight,
								  &island_node, &ship_node);

		int min_depth = INT_MAX;
		int *prev = (int *) calloc(map->number_nodes, sizeof(int));
		int *distance = dijkstra(map, island_node, prev);
		int *distance_from_ship = dijkstra(map, ship_node, prev);

		if (distance[ship_node] == INT_MAX){
			fprintf(fp_out, "Echipajul nu poate transporta comoara inapoi la corabie\n");
		}
		else if (distance_from_ship[island_node] == INT_MAX){
			fprintf(fp_out, "Echipajul nu poate ajunge la insula\n");
		}
		else {
			List_path *path = shortest_path(map, island_node, ship_node, prev);
			node_path *curr_node = path->head;
			while (curr_node != NULL) {
				if (curr_node->index != island_node &&
					curr_node->index != ship_node) {
					if (min_depth > map->list[curr_node->index]->head->depth) {
						min_depth = map->list[curr_node->index]->head->depth;
					}
				}
				fprintf(fp_out, "%s ",
						map->list[curr_node->index]->head->obj_name);
				curr_node = curr_node->next;
			}
			int number_paths = total_weight / min_depth;
			fprintf(fp_out, "\n%d\n%d\n%d\n", distance[ship_node], min_depth,
					number_paths);
			free_list(&path);
		}
		free_graph(&map);
		free(distance);
		free(distance_from_ship);
		free(prev);
	}
	fclose(fp);
	fclose(fp_out);
	return 0;
}
