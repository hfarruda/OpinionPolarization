//Developed by Henrique Ferraz de Arruda
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define UNDIRECTED
#define COS_X_2 0
#define COS_X 1
#define EQUAL_TRANSMISSION 2
#define COS_X_CUT 3
#define MIXED_TRANSMISSION 4

#define PI 3.141592653589793

#define MAXCHAR 10000
#define ARRAY_STEP 100
#define NEIGHBORS_STEP 100


struct Network{
    unsigned int vCount;
    unsigned int adjlistSize;
    unsigned int **adjlist;
    unsigned int *numberOfNeighbors;
    unsigned int *maxNumberOfNeighbors;
};

struct DynamicsVariables{
    double *average_similarity;
    unsigned int *repulsion_count;
    unsigned int *attraction_count;
    double *b;
    unsigned int b_size, result_size;
};


__inline__ void cleanNetwork(Network &network){
    for (unsigned int i=0; i < network.adjlistSize; i++)
        free(network.adjlist[i]);
    free(network.adjlist);
    free(network.numberOfNeighbors);
    free(network.maxNumberOfNeighbors);
}

__inline__ unsigned int selectARandomNeighbor(Network &network, unsigned int node){
    return network.adjlist[node][(unsigned int) lrand48() % network.numberOfNeighbors[node]];
}

__inline__ unsigned int selectARandomNode(Network &network){
    return (unsigned int) lrand48() % network.vCount;
}

__inline__ bool itemInList(unsigned int item, unsigned int *list, unsigned int list_size){
    for (unsigned int i=0; i<list_size; i++){
        if (list[i] == item){
            return true;
        }
    }
    return false;
}

__inline__ unsigned int argwhere(unsigned int item, unsigned int *list, unsigned int list_size){
    for (unsigned int i=0; i<list_size; i++){
        if (list[i] == item){
            return i;
        }
    }
    printf("Error: item not in list!\n");
    return -1;
}

void rewireAConnectionToRandom(Network &network, unsigned int source, unsigned int target){
    unsigned int new_target = lrand48() % network.vCount;
    while (new_target == source or new_target == target or itemInList(new_target, network.adjlist[source], network.numberOfNeighbors[source])){
        new_target = lrand48() % network.vCount;
    }

    //Change target in source list
    unsigned int pos = argwhere(target, network.adjlist[source], network.numberOfNeighbors[source]);
    network.adjlist[source][pos] = new_target;

    #ifdef UNDIRECTED //Change source in target list
        //remove connection in the old target list
        pos = argwhere(source, network.adjlist[target], network.numberOfNeighbors[target]);
        network.adjlist[target][pos] = network.adjlist[target][network.numberOfNeighbors[target]-1];//the latter element is put in the element position
        network.numberOfNeighbors[target]--;

        if (network.numberOfNeighbors[new_target] >= network.maxNumberOfNeighbors[new_target]){//Only if the array is smaller than the necessary size
            network.maxNumberOfNeighbors[new_target] += NEIGHBORS_STEP;
            network.adjlist[new_target] = (unsigned int *) realloc(network.adjlist[new_target], network.maxNumberOfNeighbors[new_target] * sizeof(unsigned int));
        }

        network.adjlist[new_target][network.numberOfNeighbors[new_target]] = source;
        network.numberOfNeighbors[new_target] ++;

    #endif 

}

__inline__ bool shallRewire(double x){
    double value = 0;
    if (x > 1.){
        value = pow(cos(PI/2. * x),2.);
        if (value < drand48()) //[0,1))
            return true;
        else
            return false;
    }
    return false;
}

Network loadNetwork(char *fileName, bool verbose){
    unsigned int i = 0;
    unsigned int j = 0;
    char str[MAXCHAR], strAux[MAXCHAR];
    Network network;
    network.vCount = 0;
    network.adjlistSize = 0;

    //Starting arrays
    network.adjlistSize = ARRAY_STEP;
    network.adjlist = (unsigned int **) malloc(network.adjlistSize * sizeof(unsigned int*));
    
    network.numberOfNeighbors = (unsigned int *) malloc(network.adjlistSize * sizeof(unsigned int));
    for (i=0; i<network.adjlistSize ;i++)
        network.numberOfNeighbors[i] = 0;

    network.maxNumberOfNeighbors = (unsigned int *) malloc(network.adjlistSize * sizeof(unsigned int));
    for (i=0; i<network.adjlistSize ;i++){
        network.maxNumberOfNeighbors[i] = NEIGHBORS_STEP;
        network.adjlist[i] = (unsigned int *) malloc(NEIGHBORS_STEP * sizeof(unsigned int));
    }
    //Reading adjlist
    FILE *fp;
    fp = fopen(fileName, "r"); // read mode

    if (fp == NULL)
    {
        printf("Error: file not found!\n %s\n", fileName);
        exit(0);
    }else{
        if (verbose) printf("File name: %s\n", fileName);
    }

    unsigned int actualNode = 0;
    unsigned int actualNeighbor = 0;
    if (verbose) printf("Reading file...\n");
    while (!feof(fp)){
        fgets (str, MAXCHAR, fp);
        if (str[0] != '#'){//Skip comment line
            i = 0;
            //Finding node
            while (str[i] != ' ' && str[i] != '\n'){
                strAux[i] = str[i];
                i++;
            }
            strAux[i] = '\0';
            actualNode = atoi(strAux); 
            while (actualNode >= network.adjlistSize){
                network.adjlist = (unsigned int **) realloc(network.adjlist, (network.adjlistSize+ARRAY_STEP) * sizeof(unsigned int*));
                network.numberOfNeighbors = (unsigned int *) realloc(network.numberOfNeighbors, (network.adjlistSize+ARRAY_STEP) * sizeof(unsigned int));
                network.maxNumberOfNeighbors = (unsigned int *) realloc(network.maxNumberOfNeighbors, (network.adjlistSize+ARRAY_STEP) * sizeof(unsigned int));
                for (unsigned int k=network.adjlistSize; k<network.adjlistSize+ARRAY_STEP; k++){
                    network.numberOfNeighbors[k] = 0;
                    network.maxNumberOfNeighbors[k] = NEIGHBORS_STEP;
                    network.adjlist[k] = (unsigned int *) malloc(NEIGHBORS_STEP * sizeof(unsigned int));
                }
                network.adjlistSize += ARRAY_STEP;
            }

            while (str[i] == ' ')
                i++;
            j = 0;

            network.numberOfNeighbors[actualNode] = 0;

            for (; i < strlen(str); i++){
                while (str[i] != ' ' && str[i] != '\n'){
                    strAux[j] = str[i];
                    i++;
                    j++;
                }

                strAux[j] = '\0';
                j = 0;
                if (strcmp("",strAux) != 0){
                    actualNeighbor = atoi(strAux);

                    if (network.numberOfNeighbors[actualNode] >= network.maxNumberOfNeighbors[actualNode]){
                        network.maxNumberOfNeighbors[actualNode] += NEIGHBORS_STEP;
                        network.adjlist[actualNode] = (unsigned int *) realloc(network.adjlist[actualNode], network.maxNumberOfNeighbors[actualNode] * sizeof(unsigned int));
                    }
                    
                    network.adjlist[actualNode][network.numberOfNeighbors[actualNode]] = actualNeighbor;
                    network.numberOfNeighbors[actualNode] ++;
                }
            }
            network.vCount = actualNode+1;
        }
    }

    fclose(fp);
    return network;
}


__inline__ double function_probability(double x, double phi, char transmission_type){
    double out = 1.;
    switch (transmission_type){
        case COS_X_2: out = pow(cos((x*PI)/4. + phi),2.);
        break;
        case COS_X: out = pow(cos(PI/2. * x + phi),2.);
        break;
        case EQUAL_TRANSMISSION: out = 1.;
        break;
        case COS_X_CUT: if (x < 1.) out = pow(cos(PI/2. * x),2.); else out = 0.;
        break;
        default: printf("ERROR: function_probability(double x, double phi, char transmission_type), unknown transmission_type\n"); 
        break;
    }
    return out;
}

__inline__ double transmission_probability(double theta, double b_i, double phi, char transmission_type){
    double x = (double) fabs(theta - b_i);
    return function_probability(x,phi,transmission_type);
}

__inline__ double reception_probability(double b_i, double neighbor_b, double phi, char reception_type){
    double x =  (double) fabs(b_i - neighbor_b);
    return function_probability(x,phi,reception_type);
}

__inline__ double set_initial_b(double min_value, double max_value){
    double b_out;
    b_out = drand48(); //[0,1)
    b_out *= (max_value - min_value);
    b_out += min_value;
    return b_out;
}

__inline__ double new_post(double min_value, double max_value){
    double value;
    value = drand48(); //[0,1)
    value *= (max_value - min_value);
    value += min_value;
    return value;
}

__inline__ double attraction(double b_i, double theta, double step){
    double b_out;
    b_out = b_i;
    if (b_i > theta){
        b_out -= step;
        if (b_out < theta)
            b_out = theta;
    }
    else if (b_i < theta){ //if neighbor_b == theta: do nothing
        b_out += step;
        if (b_out > theta)
            b_out = theta;
    }
    return b_out;
}

__inline__ double repulsion(double b_i, double theta, double step, double min_value, double max_value){
    double b_out;
    b_out = b_i;
    if (b_i > theta){
        b_out += step;
        if (b_out > max_value)
            b_out = max_value;
    }
    else if (b_i < theta){ //if neighbor_b == theta: do nothing
        b_out -= step; 
        if (b_out < min_value)
            b_out = min_value;
    }
    return b_out;
}

__inline__ double csi(double theta, double neighbor_b, double values_interval){
    return (double) fabs(theta - neighbor_b)/(values_interval); //probability of divergence
}

__inline__ double average_similarity_between_agents(Network &g, double *b, double max_value){
    double similarity_sum = 0.;
    unsigned int n_edges = 0, e_0 = 0, e_1 = 0;
    for (unsigned int i=0; i < g.vCount; i++)
        n_edges += g.numberOfNeighbors[i];
    
    for (unsigned int e_0=0; e_0 < g.vCount; e_0++){ 
        for (unsigned int i=0; i<g.numberOfNeighbors[e_0]; i++){
            e_1 = g.adjlist[e_0][i];
            similarity_sum += (double) fabs(b[e_0] - b[e_1]);
        }
    }
    return similarity_sum / (double) n_edges;
}

__inline__ char random_transmission_type(){
    char out;
    unsigned int selected;
    selected = (unsigned int) lrand48() % 3;
    switch (selected){
        case 0: out = COS_X;
        break;
        case 1: out = EQUAL_TRANSMISSION;
        break;
        case 2: out = COS_X_CUT;
        break;
        default: printf("ERROR: unknown transmission_type in random_transmission_type(). %d\n", selected); 
        break;
    }
    return out; 
}

DynamicsVariables simulate(double *b, 
                           Network *g, 
                           double phi_transmission, 
                           double phi_reception, 
                           double max_value, 
                           double min_value, 
                           unsigned int n_iterations, 
                           unsigned int step_iterations, 
                           double step, char transmission_type, 
                           char reception_type, 
                           bool rewire_dynamics,
                           char *name_out, 
                           bool save_network_out){

    double *neighbor_b;
    double values_interval, b_i, theta, transmission_probability_value;
    unsigned int result_size, repulsion_count_aux, attraction_count_aux, selected_node, position, neighbor_index;
    char *mixed_transmission_types;
    DynamicsVariables out;
    position = 0;

    result_size = (unsigned int) n_iterations / step_iterations;
    out.average_similarity = (double *) malloc(result_size * sizeof(double));
    values_interval = max_value - min_value; // used to compute csi
    repulsion_count_aux = 0;
    attraction_count_aux = 0;

    out.repulsion_count = (unsigned int *) malloc(result_size * sizeof(unsigned int));
    for (unsigned int i=0; i < result_size; i++)
        out.repulsion_count[i] = 0;
    
    out.attraction_count = (unsigned int *) malloc(result_size * sizeof(unsigned int));
    for (unsigned int i=0; i < result_size; i++)
        out.attraction_count[i] = 0;

    if (transmission_type == MIXED_TRANSMISSION){
        //Define here a vector of mixed_trasmission_type with the same probability for each of the cases
        mixed_transmission_types = (char *) malloc(g->vCount * sizeof(char));
        for (unsigned int i=0; i < g->vCount; i++){
            mixed_transmission_types[i] = random_transmission_type();
        }
    }

    for (unsigned int iteration = 1; iteration < n_iterations+1; iteration++){
        selected_node = selectARandomNode(*g);
        b_i = b[selected_node]; //agent opinion
        theta = new_post(min_value, max_value);
        
        if (transmission_type == MIXED_TRANSMISSION){
            transmission_probability_value = transmission_probability(theta, b_i, phi_transmission, mixed_transmission_types[selected_node]);
        }
        else{    
            transmission_probability_value = transmission_probability(theta, b_i, phi_transmission, transmission_type);
        }

        if (drand48() < transmission_probability_value){
            for (unsigned int neighbor_pos=0; neighbor_pos < g->numberOfNeighbors[selected_node]; neighbor_pos++){
                neighbor_index = g->adjlist[selected_node][neighbor_pos];
                neighbor_b = &b[neighbor_index];
                if (drand48() < reception_probability(b_i, *neighbor_b, phi_reception, reception_type)){
                    if (drand48() < csi(theta, *neighbor_b, values_interval)){
                        repulsion_count_aux ++;
                        *neighbor_b = repulsion(*neighbor_b, theta, step, min_value, max_value);
                        if (rewire_dynamics){
                            if (shallRewire(abs(b_i - *neighbor_b))){
                                rewireAConnectionToRandom(*g, selected_node, neighbor_index);
                            }
                        }
                    }
                    else{
                        *neighbor_b = attraction(*neighbor_b, theta, step);
                        attraction_count_aux += 1;
                    }
                } 
            }
        }
        
        //Saving results
        if (iteration % step_iterations == 0){
            out.average_similarity[position] = average_similarity_between_agents(*g,b,max_value);
            out.repulsion_count[position] = repulsion_count_aux;
            out.attraction_count[position] = attraction_count_aux;
            repulsion_count_aux = 0;
            attraction_count_aux = 0;
            position++;
        }
    }
    if (transmission_type == MIXED_TRANSMISSION){
        free(mixed_transmission_types);
    }

    out.b = b;
    out.b_size = g->vCount;
    out.result_size = result_size;

    if (save_network_out){
        FILE * fp;
        char file_name[100];
        strcpy(file_name, name_out);
        strcat(file_name, ".adjlist");
        fp = fopen (file_name, "w");
        for (unsigned int k=0; k<g->vCount; k++){
            fprintf(fp, "%u ",k);
            for (unsigned int l=0; l<g->numberOfNeighbors[k]; l++){
                fprintf(fp, "%u ", g->adjlist[k][l]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }

    return out;
}

void write_files(DynamicsVariables out, char *nameOut, bool verbose, bool only_b){
    FILE * fp;
    char file_name[100];
    if (!only_b){
        strcpy(file_name, nameOut);
        strcat(file_name, "average_similarity.bin");
        fp = fopen (file_name, "wb");
        if (verbose){
            printf("average_similarity: \n");
            printf("  Saving as: %s\n", file_name);
        } 
        fwrite(out.average_similarity, sizeof(double), out.result_size, fp);
        
        if (verbose) printf("\n");
        fclose(fp);

        strcpy(file_name, nameOut);
        strcat(file_name, "repulsion_count.bin");
        fp = fopen (file_name, "wb");
        if (verbose){
            printf("repulsion_count: \n");
            printf("  Saving as: %s\n", file_name);
        }

        fwrite(out.repulsion_count, sizeof(unsigned int), out.result_size, fp);
        
        fclose(fp);

        strcpy(file_name, nameOut);
        strcat(file_name, "attraction_count.bin");
        fp = fopen (file_name, "wb");
        if (verbose){
            printf("attraction_count: \n");
            printf("  Saving as: %s\n", file_name);
        }
        fwrite(out.attraction_count, sizeof(unsigned int), out.result_size, fp);
        
        fclose(fp);
    }

    strcpy(file_name, nameOut);
    strcat(file_name, ".bin");
    fp = fopen (file_name, "wb");
    if (verbose){
        printf("b: \n");
        printf("  Saving as: %s\n", file_name);
    }
    fwrite(out.b, sizeof(double), out.b_size, fp);

    if (verbose) printf("\n");
    fclose(fp);
}

int main(int argc, char *argv[]){
    srand48((long int) time(NULL));
    Network network;
    double *b;
    bool variablesVerbose = true;
    bool verbose = false; //true; //
    if (verbose == true) variablesVerbose = true;
    bool only_b = true;//To save only the variable considered/described in the paper
    ///////////////////////////////
    char *fileName;
    char *nameOut;
    double phi_transmission;
    double phi_reception;
    double min_value;
    double max_value;
    unsigned int n_iterations;
    unsigned int step_iterations; //The measurements obtained with this variable were not considered/described in the paper
    double step;
    char transmission_type;
    char reception_type;
    unsigned int n_separated_files;
    DynamicsVariables out;
    bool rewire_dynamics = true;
    //////////////////////////////

    phi_transmission = 0.;//Fixed value

   if (argc < 2){
        FILE *fp;
        fp = fopen("parameters.txt", "r");
        if (fp != NULL){
            char str[MAXCHAR];
            fgets (str, MAXCHAR, fp);
            fileName = (char *) malloc(strlen(str) + 1);
            strcpy(fileName, str);
            fileName[strlen(fileName)-1] = '\0'; 
            if (variablesVerbose) printf("Network name: %s \n", fileName);
            fgets (str, MAXCHAR, fp);
            nameOut = (char *) malloc(strlen(str) + 1);
            strcpy(nameOut, str);
            nameOut[strlen(nameOut)-1] = '\0'; 
            if (variablesVerbose) printf("Out filename: %s \n", nameOut);
            fgets (str, MAXCHAR, fp);
            phi_reception = atof(str);
            if (variablesVerbose) printf("phi (reception): %lf \n", phi_reception);
            fgets (str, MAXCHAR, fp);
            min_value = atof(str);
            if (variablesVerbose) printf("Min opinion value: %lf \n", min_value);
            fgets (str, MAXCHAR, fp);
            max_value = atof(str);
            if (variablesVerbose) printf("Max opinion value: %lf \n", max_value);
            fgets (str, MAXCHAR, fp);
            n_iterations = (unsigned int) atoi(str);
            if (variablesVerbose) printf("Total number of iterations: %u \n", n_iterations);
            fgets (str, MAXCHAR, fp);
            step_iterations = (unsigned int) atoi(str);
            if (variablesVerbose) printf("Saves for each %u steps \n", step_iterations);
            fgets (str, MAXCHAR, fp);
            step = atof(str);
            if (variablesVerbose) printf("Opinion change step %lf \n", step);
            //Transmission type
            fgets (str, MAXCHAR, fp);
            str[strlen(str)-1] = '\0'; 
            if (strcmp("COS_X_2", str) == 0) transmission_type = COS_X_2;
            else if (strcmp("COS_X", str) == 0) transmission_type = COS_X;
            else if (strcmp("EQUAL_TRANSMISSION", str) == 0) transmission_type = EQUAL_TRANSMISSION;
            else if (strcmp("COS_X_CUT", str) == 0) transmission_type = COS_X_CUT;
            else if (strcmp("MIXED_TRANSMISSION", str) == 0) transmission_type = MIXED_TRANSMISSION;
            else{
                printf ("Error: undefined transmission type.");
                fclose(fp);
                return 1;
            }
            if (variablesVerbose) printf("Transmission type %s %d\n", str, transmission_type);
            //Reception type
            fgets (str, MAXCHAR, fp);
            str[strlen(str)-1] = '\0'; 
            if (strcmp("COS_X_2", str) == 0) reception_type = COS_X_2;
            else if (strcmp("COS_X", str) == 0) reception_type = COS_X;
            else if (strcmp("EQUAL_TRANSMISSION", str) == 0) reception_type = EQUAL_TRANSMISSION;
            else{
                printf ("Error: undefined reception type.");
                fclose(fp);
                return 1;
            }
            if (variablesVerbose) printf("Reception type %s %d\n", str, reception_type);
            
            fgets (str, MAXCHAR, fp);
            n_separated_files = (unsigned int) atoi(str);
            if (variablesVerbose){
                if (n_separated_files == 1)
                    printf("Generates %u separated file.\n", n_separated_files);
                else
                    printf("Generates %u separated files.\n", n_separated_files);
            }
            fgets (str, MAXCHAR, fp);
            rewire_dynamics = (bool) atoi(str);
            if (rewire_dynamics == true)
                printf("Dynamics with rewiring.\n");
            else
                printf("Dynamics without rewiring.\n");
            fclose(fp);
        }
        else{
            printf("ERROR! \"parameters.txt\" was not found. \n");
            return 1;
        }
    }else{
        fileName = argv[1];
        if (variablesVerbose) printf("Network name: %s \n", fileName);
        nameOut = argv[2];
        if (variablesVerbose) printf("Out filename: %s \n", nameOut);
        phi_reception = atof(argv[3]);
        if (variablesVerbose) printf("phi (reception): %lf \n", phi_reception);
        min_value = atof(argv[4]);
        if (variablesVerbose) printf("Min opinion value: %lf \n", min_value);
        max_value = atof(argv[5]);
        if (variablesVerbose) printf("Max opinion value: %lf \n", max_value);
        n_iterations = (unsigned int) atoi(argv[6]);
        if (variablesVerbose) printf("Total number of iterations: %u \n", n_iterations);
        step_iterations = (unsigned int) atoi(argv[7]);
        if (variablesVerbose) printf("Saves for each %u steps \n", step_iterations);
        step = atof(argv[8]);
        if (variablesVerbose) printf("Opinion change step %lf \n", step);
        //Transmission type
        if (strcmp("COS_X_2",argv[9]) == 0) transmission_type = COS_X_2;
        else if (strcmp("COS_X",argv[9]) == 0) transmission_type = COS_X;
        else if (strcmp("EQUAL_TRANSMISSION",argv[9]) == 0) transmission_type = EQUAL_TRANSMISSION;
        else if (strcmp("COS_X_CUT",argv[9]) == 0) transmission_type = COS_X_CUT;
        else if (strcmp("MIXED_TRANSMISSION",argv[9]) == 0) transmission_type = MIXED_TRANSMISSION;
        else{
            printf ("Error: undefined transmission type.");
            return 1;
        }
        if (variablesVerbose) printf("Transmission type %s %d\n", argv[9], transmission_type);
        //Reception type
        if (strcmp("COS_X_2",argv[10]) == 0) reception_type = COS_X_2;
        else if (strcmp("COS_X",argv[10]) == 0) reception_type = COS_X;
        else if (strcmp("EQUAL_TRANSMISSION",argv[10]) == 0) reception_type = EQUAL_TRANSMISSION;
        else{
            printf ("Error: undefined reception type.");
            return 1;
        }
        if (variablesVerbose) printf("Reception type %s %d\n", argv[10], reception_type);
        n_separated_files = (unsigned int) atoi(argv[11]);
        if (variablesVerbose){
            if (n_separated_files == 1)
                printf("Generates %u separated file.\n", n_separated_files);
            else
                printf("Generates %u separated files.\n", n_separated_files);
        }
        rewire_dynamics = (bool) atoi(argv[12]);
        if (rewire_dynamics == true)
            printf("Dynamics with rewiring.\n");
        else
            printf("Dynamics without rewiring.\n");
    }
    
    network = loadNetwork(fileName, verbose);
    
    if (verbose) printf("vcount = %u\n", network.vCount);
    for (unsigned int k=0; k<network.vCount; k++){
        if (verbose) printf("%u: ",k);
        for (unsigned int l=0; l<network.numberOfNeighbors[k]; l++){
            if (verbose) printf("%u ", network.adjlist[k][l]);
        }
        if (verbose) printf("\n");
    }
    
    if (verbose) printf("Executing dynamics...\n");

    //b
    b = (double *) malloc(network.vCount * sizeof(double));
    for (unsigned int i=0; i < network.vCount; i++){
        b[i] = set_initial_b(min_value, max_value);
    }

    char network_name_out[150];
    if (n_separated_files <= 1){
        strcpy(network_name_out, nameOut);
        out = simulate(b,&network, phi_transmission, phi_reception, max_value, min_value, n_iterations, step_iterations, step, transmission_type, reception_type, rewire_dynamics,network_name_out,true);
        if (verbose) printf("Writing files...\n");
        write_files(out, nameOut, verbose, only_b);
        free(out.average_similarity);
        free(out.repulsion_count);
        free(out.attraction_count);
    }else{
        char aux[50];
        char nameOut_aux[150];
        for (int i=0; i<n_separated_files; i++){
            strcpy(nameOut_aux, nameOut);
            snprintf(aux, 50, "%d", i);
            strcat(nameOut_aux, aux);

            strcpy(network_name_out, nameOut);
            strcat(network_name_out, aux);

            // if (verbose) printf("%s, phi(reception) = %lf \n", nameOut_aux, phi_reception);
            out = simulate(b,&network, phi_transmission, phi_reception, max_value, min_value, n_iterations, step_iterations, step, transmission_type, reception_type, rewire_dynamics,network_name_out,true);
            if (verbose) printf("Writing files...\n");
            write_files(out, nameOut_aux, verbose, only_b);

            if (verbose) printf("Cleanning memory...\n");
            free(out.average_similarity);
            free(out.repulsion_count);
            free(out.attraction_count);
        }
    }
    free(out.b);
    cleanNetwork(network);
    return 0;
}