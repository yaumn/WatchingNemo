#ifndef MOBILITY_H
#define MOBILITY_H
#include <time.h>
#include "tools.h"


//Time in ms between each update
#define DT 100 //in ms
#define RANDOM_TIME_MAX 5000 //in ms
typedef long time_ms; 

struct fish; 
struct aquarium;

enum mobility_function {
			RANDOM_WAY_POINT,
			HORIZONTAL_WAY_POINT,
			CHAOTIC,
			NO_MOBILITY
};

void mobility_init(struct aquarium *);
void mobility_finalize();

int mobility_from_name(char *name);
void call_mobility_function(struct fish*);
int setup_mobility(struct fish* fish, char *mobility);

#endif
