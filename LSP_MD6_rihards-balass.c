#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 126

typedef struct memoryBuf{
	int free;
	int size;
	int used;
	struct memoryBuf *next;
	struct memoryBuf *prev;
}memoryBuf;

struct memoryBuf *first;
struct memoryBuf *p;

void help();
void makeMemoryBuf(FILE *data);
void printMemory();
void wipeMemory();
int firstFit(FILE *data);
int nextFit(FILE *data);

//------------------------------------------
//  Galvenā funkcija
//------------------------------------------
int main(int argc, char *argv[]){
	FILE *chunks;
	FILE *sizes;
	int err;

	if(argc < 5 || argc > 5){
		if(argc < 5){
			printf("par maz parametru!\n");
		}else{
			printf("par daudz parametru!\n");
		}
		return -1;
	}

	if(!strcmp(argv[1], "-c")){ //tiek padots chunks fails
		if((chunks = fopen(argv[2], "r")) != NULL){
			printf("chunks atvērts!\n\n");
		}else{
			printf("chunks netika atvērts!\n");
			return -1;
		}
		if(!strcmp(argv[3], "-s")){ //tiek padots fails sizes
			if((sizes = fopen(argv[4], "r")) != NULL){
				printf("sizes atverts!\n\n");
			}else{
				printf("sizes netika atvērts!\n");
				return -1;
			}
		}
	}else if(!strcmp(argv[1], "-s")){ //tiek padots sizes fails
		if((sizes = fopen(argv[2], "r")) != NULL){
			printf("sizes atverts!\n\n");
		}else{
			printf("sizes netika atvērts!\n");
			return -1;
		}
		if(!strcmp(argv[3], "-c")){ //tiek padots fails chunks
			if((chunks = fopen(argv[4], "r")) != NULL){
				printf("chunks atvērts!\n\n");
			}else{
				printf("chunks netika atvērts!\n");
				return -1;
			}
		}
	}
	makeMemoryBuf(chunks);
	err = firstFit(sizes);
	if(err){
//		printMemory();
//		return -1;
	}
	printMemory();
	wipeMemory();
	printMemory();
	rewind(sizes);
	err = nextFit(sizes);
	if(err){
//		printMemory();
//		return -1;
	}
	printMemory();
	
	fclose(chunks);
	fclose(sizes);
	return 0;
}

//------------------------------------------
//  Palīdzības funkcija
//------------------------------------------
void help(){
	printf("HELP!\n-------------------------------------------------------\n");
	//instrukcijas
	printf("\t-h | --help :");
		printf("\t Palīdzība.\n");
	printf("\t-c <filename>:");
		printf("\t\t Definē sarakstu ar brīvo atmiņas bloku lielumiem\n");
	printf("\t-s <filename>:");
		printf("\t\t Definē atmiņas pieprasījumus baitos\n");
	printf("-------------------------------------------------------\n");
}

//------------------------------------------
//  Funkcija saraksta izveidošanai
//------------------------------------------
void makeMemoryBuf(FILE *data){
	printf("padots fails, taisu sarakstu ar atmiņu\n");
	//struct memoryBuf *p;
	p = malloc(sizeof(*p));
	first = p;
	char line[MAX];
	int error = 0;
	
	if(fgets(line, sizeof line, data) != NULL){
		//printf("first line %s \n", line);
		p->free = 1;
		p->size = atoi(line);
		//printf("p->size =  %i, %i\n", p->size, atoi(line));
		p->used = 0;
		p->prev = NULL;
		p->next = malloc(sizeof(*p));
		p->next->prev = p;
		p = p->next;
	}else{
		printf("Fails ir tukšs!\n");
		error = 1;
	}
	if(!error){
		while(fgets(line, sizeof line, data) != NULL){ /* read a line */
			//printf("next line %s\n", line);
			p->free = 1;
			p->size = atoi(line);
			//printf("p->size =  %i, %i\n", p->size, atoi(line));
			p->used = 0;
			p->next = malloc(sizeof(*p));
			p->next->prev = p;
			p = p->next;
		}
	}
}

//------------------------------------------
//  Funkcija kas izdrukā visus atmiņas 
//	apgabalus, un to cik daudz tie ir aizņemti
//------------------------------------------
void printMemory(){
	for(p = first; p->next != NULL; p = p->next){
		printf("Memory buffer size - %i, used - %i\n", p->size, p->used);
	}
}

//------------------------------------------
//  Funkcija kas izdzēš visu atmiņu
//------------------------------------------
void wipeMemory(){
	for(p = first; p->next != NULL; p = p->next){
		p->free = 1;
		p->used = 0;
	}
}

//------------------------------------------
//  First Fit algoritms
//------------------------------------------
int firstFit(FILE *data){
	printf("Allocēju atmiņu izmantojot First Fit algoritmu\n");
	p = first;
	char line[MAX];
	int error = 0;
	int alloc;
	
	while(fgets(line, sizeof line, data) != NULL){
		//printf("first line %s \n", line);
		alloc = atoi(line);
		for(p = first; p != NULL; p = p->next){
			if(p->size >= alloc && p->free == 1){ //atrasts atmiņas apgabals kurš ir vai nu vienāds vai lielāks par pieprasīto un brīvs
				p->free = 0;
				p->used = alloc;
				break;
			}else if(p->next == NULL){
				printf("Nepietiek atmiņas!!!\n");
				error = 1;
				break;
			}
		}
		if(error){
			break;
		}
	}
	return error;
}

int nextFit(FILE *data){
	printf("Allocēju atmiņu izmantojot Next Fit algoritmu\n");
	p = first;
	struct memoryBuf *temp;
	char line[MAX];
	int error = 0;
	int alloc;
	
	while(fgets(line, sizeof line, data) != NULL){
		//printf("first line %s \n", line);
		alloc = atoi(line);
		for(p = first; p != NULL; p = p->next){
			if(p->size >= alloc && p->free == 1){ 	//atrasts atmiņas apgabals kurš ir vai nu vienāds vai lielāks par pieprasīto un brīvs
													//pēc algoritma ir jāmeklē nākamais atmiņas apgabals
				for(temp = p->next; temp != NULL; temp = temp->next){
					if(temp->size >= alloc && temp->free == 1){
						temp->free = 0;
						temp->used = alloc;
						break;
					}else if(temp->next == NULL){
						printf("Nepietiek atmiņas!!!\n");
						error = 1;
						break;
					}
				}
				break;
			}else if(p->next == NULL){
				printf("Nepietiek atmiņas!!!\n");
				error = 1;
				break;
			}
		}
		if(error){
			break;
		}
	}
	return error;
}