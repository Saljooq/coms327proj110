//#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <endian.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#define minObjects 10

#define MAX_ROOMS 12
#define MIN_ROOMS 6

#define xlenMax 78
#define ylenMax 19


#define minRoomxlen 4
#define minRoomylen 3

#define maxStairs 3
#define minStairs 1

#define BIT_SMART 	0x1
#define TELE 		0x2
#define TUN 		0x4
#define ERAT 		0x8
#define PICKUP 		0x10
#define DESTROY 	0x20
#define UNIQ 		0x40
#define BOSS 		0x80
#define PASS 		0x100

#define WEAPON 		0x1
#define OFFHAND 	0x2
#define RANGED 		0x4
#define ARMOR 		0x8
#define HELMET 		0x10
#define CLOAK 		0x20
#define GLOVES 		0x40
#define BOOTS 		0x80
#define RING 		0x100
#define AMULET 		0x200
#define LIGHT 		0x400
#define SCROLL 		0x800
#define BOOK 		0x1000
#define FLASK 		0x2000
#define GOLD 		0x4000
#define AMMUNITION 	0x8000
#define FOOD 		0x10000
#define WAND 		0x20000
#define CONTAINER 	0x40000


/*the room is a struct to save the relevant coordinates and leghts of a room*/
class room
{
public:
	int xloc;
	int yloc;
	int xlen;
	int ylen;
};
/*node - this contains x and y coordinates and address of next and prev node;*/
class node {
public:
	int xcoor;
	int ycoor;
	node* prev;
	node* next;
};
/*node_heap - this contains the head node address (last node entered), the tail node address (first node entered) and the size of the node_heap*/
class node_heap {
public:
	node* head;
	node* tail;
	int size;
};
/*neighbourhood - this contains an 8x2 array that contains all possible neighour's coordinates (x,y) and the size - num of neighours*/
class neighbourhood{
public:
	int store[8][2];
	int size;
};
/*This struct will contain all the data we will need for pc*/
class PC{
public:
	int dam[3];
	int totDam;
	int hp;
	int speed;
	int x;
	int y;
};
/*This will be used to parse the monster description from a text file*/
class monster_desc
{
public:
	string name;
	string desc;
	vector<int> color;
	int speed[3];
	int abil;
	int hp[3];
	int dam[3];
	char symb;
	int rrty;

};
/*ALL non-pc players data will be stored here*/
class NPC{
public:
	int hp;//roll over from desc
	int character; //turns to abil
	int x;
	int y;
	int speed; //roll over from desc
	int ifPCseen;
	int PCx;
	int PCy;
	monster_desc *desc;//since sym, name, desc, color are unchanged for instances, we can use the desc
	int index;//this will be the precise index of the desc in the vector of descriptions
	int tangle;
};
/*These nodes store all the players and their data needed to put them on the heap*/
class player_node{
public:
	int ifPC;
	PC* pc;
	NPC* npc;
	int next_turn;
	int when_initiated;
	int alive;
	player_node* prev;
	player_node* next;

};
/*This merely contains the head and tail player_node of the heap. It is used to access all the players*/
class player_node_heap {
public:
	player_node* head;
	player_node* tail;
	//int size;
};
/*This will be used to parse the obj_dec description from a text file*/
class obj_desc
{
public:
	string name;
	string desc;
	vector<int> color;
	int hit[3];
	int type;
	int dodge[3];
	int dam[3];
	int def[3];
	int weight[3];
	int speed[3];
	int attr[3];
	int val[3];
	int art;
	int rrty;
};

class obj_node
{
public:
	obj_desc* d;
	int index;
};

int bossFound = 0;
int attackLine = ylenMax;
int radius = 2;
int untangle();

int readyPC();
int wearThis(obj_node *o);
int selectInventory();
int viewEquipment();
int eq_vector = 0;
int ring2 = 0;
vector<obj_node *>equipped;
vector<obj_node *>inventory;
int weaponInventory();
int description_controller(PC* pc, int& x, int& y);

char getObjChar(int type);
int populate_dungeon_objects();
int findMonster();

vector<int>object_used;
vector<int>object_death;
vector<int>monster_used;
vector<int>monster_death;
vector <monster_desc> monsters;
vector <obj_desc> odesc;

int roll_dice(int* d);
int parseObjDesc();
int get_obj_type(string abil);
string int_to_objtype(int abil);
int print_obj_desc();

int print_monster_desc();
int parseMonstersDesc();
string getColorString(int i);
string intToAbil(int abil);
int finddice(int* ar, string s);
int getColorint(string t);
int get_abilities(string abil);


int teleport_controller(PC* pc, int& x, int& y);
int print_teleport(PC* pc, int* x, int* y);
int print_dungeon_limited(PC* pc);

int random_generator(player_node_heap** h, PC** pc, int nummon, room** rooms);
int getmonsterlist(player_node_heap* h);
int getkey(int x,int y, int *nextx, int *nexty, int* ifend, player_node_heap* h);

void init_terminal();
int print_end(int success);
int populate_heap(player_node_heap** h);//This puts all the player_node in a heap
int push_player_node(player_node_heap* h, player_node* p);//used to add players to heap
int kill_all();//This is used to make sure all the players malloc'd get freed
int initialize_pc(PC** p);//this is used to take a pc struct's pointer and create a node and place it in random spot.
int initialize_players(int n, PC* p);//used to create all the npc's as per the specs.
/*These are all the prototypes that we'll use later*/
int getNeighbour(int x, int y, neighbourhood* n);
int push(node_heap* nh, int x, int y);
int pop(node_heap* nh, int* x, int* y);
int print_difficulty(int PCposx, int PCposy, int diff_print[xlenMax][ylenMax]);


int makes_sense(room rooms[], int numRooms);
int not_so_rand_roomsize_resizer(int numRooms);
int print_dungeon();
int djik (int xcoordinate, int ycoordinate, int ifdigger);
int print_hardness();
int print_neighbour_movement(int PCposx, int PCposy, int hardness[xlenMax][ylenMax]);

int next_move(player_node *pn, PC* pc, int* ifend, player_node_heap* h);
int pop_player(player_node_heap* nh, int* ifend, player_node** p);
int if_in_room(PC* pc);//updates all bots that share same room as pc

uint8_t player_init_counter = 0;
int distant_from_pc(PC* p, int x, int y);//creates a giant square centered at PC to make sure no bots are initialised too close

/*the five grids are being saved so all the methods will have access to them*/
char grid[xlenMax][ylenMax];
int hardness[xlenMax][ylenMax];
int difficulty[xlenMax][ylenMax];//this will be used to save data for distance of non-tunnelers
int difficulty_t[xlenMax][ylenMax];//this is to save data for tunnelers
uint8_t shortPathRecord[xlenMax][ylenMax];//this keeps record of what is on the queue and what isn't for djik* algo
int visible;

char remembered[xlenMax][ylenMax];//what pc remembers

obj_node *grid_objects[xlenMax][ylenMax];
player_node *grid_players[xlenMax][ylenMax];//this will be used to store pointers of all the player nodes

int main(int argc, char* argv[])
{

	int i, j, x, y, k; //we declare most of the common variables we'll be using late
	//the grid below will be use to store all the characters for dungeon
	//char grid[xlenMax][ylenMax];
	//int hardness[xlenMax][ylenMax];
	int numRooms, numUpstairs, numDownstairs;
	uint8_t xPCpos, yPCpos;

	visible = 0;

	srand(time(0));
	room *rooms;

	//first we populate the grid with spaces
	for (i = 0; i < xlenMax; i++)
	{
		for (j = 0; j < ylenMax; j++)
		{
			grid[i][j] = ' ';
			remembered[i][j] = ' ';
			grid_players[i][j]=NULL;
			grid_objects[i][j]=NULL;
		}
	}

	//load method goes ther
	for (i = 1; i < argc; i++)
	{
			if (!(strcmp(argv[i], "--load")))
			{
				j = 1;
				break;
			}

	}

	if (j==1)
	{
		//printf("load found\n");

		FILE *f;

		string home = getenv("HOME");
		string gamedir = ".rlg327";
		string savefile = "dungeon";
		string path = home + "/"+gamedir+"/"+savefile;
		//malloc(strlen(home) + strlen(gamedir) + strlen(savefile) + 2 + 1);
		//sprintf(path, "%s/%s/%s", home, gamedir, savefile);

		if( !( f = fopen( path.c_str(), "r"))) {/*printf("Failed to open file\n");*/ return 1;}
		//free(path);


		uint8_t temp8;
		uint16_t temp16;
		uint32_t temp32;

		char filetype[12];
		fread(filetype, sizeof(char), 12, f);

		//dealing with version
		fread(&temp32, sizeof(temp32), 1, f);

		//dealing with filesize - will need to adjust endian if to be used
		fread(&temp32, sizeof(temp32), 1, f);

		//dealing xPCpos and yPCpos
		fread(&xPCpos, sizeof(uint8_t), 1, f);
		fread(&yPCpos, sizeof(uint8_t), 1, f);

		//now we populate the dungeon matrix
		for (j = 0; j < xlenMax + 2; j++)
		{
			fread(&temp8, sizeof(uint8_t), 1, f);
		}

		for (i = 0; i < ylenMax; i++)
		{

			fread(&temp8, sizeof(uint8_t), 1, f);

			for (j = 0; j < xlenMax; j++)
			{
				fread(&temp8, sizeof(uint8_t), 1, f);
				hardness[j][i] = temp8;
			}

			fread(&temp8, sizeof(uint8_t), 1, f);

		}

		for (j = 0; j < xlenMax + 2; j++)
		{
			fread(&temp8, sizeof(uint8_t), 1, f);
		}
		//end of hardness reading

		//number of Rooms are entered here
		fread(&temp16, sizeof(uint16_t), 1, f);
		numRooms = be16toh(temp16);

		//next we fill in the matrix rooms
		rooms = (room*) malloc(numRooms * sizeof(room));

		//here we write the coordinates of the room
		for (i = 0; i < numRooms; i++)
		{
			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].xloc = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].yloc = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].xlen = temp8;

			fread(&temp8, sizeof(uint8_t), 1, f);
			rooms[i].ylen = temp8;

		}

		//next we populate the room area with the downstairs
		for (x = 0; x < numRooms; x++)
		{

			for (i = rooms[x].xloc; i < (rooms[x].xloc + rooms[x].xlen); i++)
			{
				for (j = rooms[x].yloc; j < (rooms[x].yloc + rooms[x].ylen); j++)
				{
					grid[i][j] = '.';
				}
			}
		}

		//next we deal with upstairs
		fread(&temp16, sizeof(uint16_t), 1, f);
		numUpstairs = be16toh(temp16);


		for (i = 0; i < numUpstairs; i++)
		{

			fread(&temp8, sizeof(uint8_t), 1, f);
			x = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			y = temp8 - 1;
			grid[x][y] = '<';

		}

		//next we deal with downstairs
		fread(&temp16, sizeof(uint16_t), 1, f);
		numDownstairs = be16toh(temp16);


		for (i = 0; i < numDownstairs; i++)
		{

			fread(&temp8, sizeof(uint8_t), 1, f);
			x = temp8 - 1;

			fread(&temp8, sizeof(uint8_t), 1, f);
			y = temp8 - 1;
			grid[x][y] = '>';

		}
		//next we populate the corridors wherever hardness is zero and there is no room or stairs (or in this case wherever there is space in the grid)
		for (i = 0; i < ylenMax; i++)
		{
			for (j = 0; j < xlenMax; j++)
			{
				if (hardness[j][i] == 0)
				{
					if (grid[j][i] == ' ') grid[j][i] = '#';
				}
			}
		}

		fclose(f);
		//printf("load found - final - no error while loading\n");

	}
	//this is where we start processing the dungeon if the load was not found
	else
	{

		//we will start out by creating a seed with time-0 to access some randomeness
		srand(time(0));

		//populating the hardness randomly
		for (i = 0; i < ylenMax; i++)
		{
			for (j = 0; j < xlenMax; j++)
			{
				hardness[j][i] = 1 + (rand() % 254);
			}
		}

		numRooms = MIN_ROOMS + (rand() % (MAX_ROOMS - MIN_ROOMS + 1));

		rooms = (room*) malloc(numRooms * sizeof(room));

		int resizer = not_so_rand_roomsize_resizer(numRooms);//we use this function to obtain a denominator to limit the size of the rooms

		//the if conditions used to obtain the max length of the room help avoid the floating point exception (core dump) later when we use it with modulus later
		int maxRoomxlen = xlenMax / resizer;
		if (maxRoomxlen <= minRoomxlen) maxRoomxlen = minRoomxlen + 1;


		int maxRoomylen = ylenMax / resizer;
		if (maxRoomylen <= minRoomylen) maxRoomylen = minRoomylen + 1;

		//printf("num Rooms = %d\n", numRooms); //uncomment to see num of rooms generated

		//this loop keeps going till random coordinates and lengths are obtained from random function that make sense
		while (1)
		{
			for (i = 0; i < numRooms; i++)
			{
				rooms[i].xloc = rand() % xlenMax;
				rooms[i].yloc = rand() % ylenMax;
				rooms[i].xlen = minRoomxlen + rand() % ((maxRoomxlen) - minRoomxlen);
				rooms[i].ylen = minRoomylen + rand() % ((maxRoomylen) - minRoomylen);
			}
			if (makes_sense(rooms, numRooms)) break;
		}


		//Next we populate the grid with '.' as per the randomised coordinates that made sense that we obtained earlier
		for (x = 0; x < numRooms; x++)
		{

			for (i = rooms[x].xloc; i < (rooms[x].xloc + rooms[x].xlen); i++)
			{
				for (j = rooms[x].yloc; j < (rooms[x].yloc + rooms[x].ylen); j++)
				{
					grid[i][j] = '.';
					hardness[i][j] = 0;
				}
			}
		}

		//next we carve out a path between adjacent rooms in which we use the former's x coordinate and latter's y-coordinates to create a mid-point
		for (int x = 0; x < numRooms - 1; x++)
		{
			int middlex = rooms[x].xloc;
			int middley = rooms[x + 1].yloc;
			int i;//i will save the direction of the path

			if (rooms[x].yloc > middley) i = 1;
			else i = -1;

			//first we go from from midpoint to former room
			for ( j = middley; j != rooms[x].yloc; j += i)
			{
				if (grid[middlex][j] != '.')
				{
					grid[middlex][j] = '#';
					hardness[middlex][j] = 0;
				}
			}

			//then we go from midpoint to latter room
			if (rooms[x + 1].xloc > middlex) i = 1;
			else i = -1;

			for ( j = middlex; j != rooms[x + 1].xloc; j += i)
			{
				if (grid[j][middley] != '.')
				{
					grid[j][middley] = '#';
					hardness[j][middley] = 0;
				}
			}

		}


		//here we randomise the upwards and downward staircases and insert them wherever the random coordinates and its horizontal neighbours are part of room
		for (i = 0; i < 2; i++)
		{
			//first iteration adds random number of '<' to the grid, second adds '<'
			char staircase;
			if (i == 0) staircase = '<';
			else staircase = '>';

			int numStairs = minStairs + rand() % ((maxStairs) - minStairs);
			if (i == 0)
			{
				staircase = '<';
				numUpstairs = numStairs;

			}
			else
			{
				staircase = '>';
				numDownstairs = numStairs;
			}

			for (j = 0; j < numStairs; j++)
			{
				//while loops below keeps going till a successfuk coordinate is found
				while (1)
				{
					x = 1 + (rand() % (xlenMax - 2));//this ensures that we're not on the left or the right edge because the condition below checks horizontal neighbours
					y = (rand() % (ylenMax));

					if (grid[x][y] == '.' && grid[x - 1][y] == '.' && grid[x + 1][y] == '.')
					{
						grid[x][y] = staircase;
						break;
					}
				}


			}
		}


		//this is where we try to position PC on the floor, making sure there's floor there
		xPCpos = 0;
		yPCpos = 0;
		for (i = 0; i < ylenMax; i++)
		{
			k = 0;
			for (j = 0; j < xlenMax; j++)
			{
				if (grid[j][i] == '.')
				{
					xPCpos = j+1;
					yPCpos = i+1;
					k=1;
					break;
				}

			}
			if (k) break;
		}

	}
	//this is where processing of the dungeon ends

	/*the method below will help produce the desired result for 1.07*/
	parseMonstersDesc();
	//print_monster_desc();
	parseObjDesc();
	//print_obj_desc();
	//return 0;
	/*Processing for 1.07 ends here*/

	for (i = 0; i < monsters.size(); i++) monster_death.push_back(0);
	for (i = 0; i < monsters.size(); i++) monster_used.push_back(0);
	for (i = 0; i < odesc.size(); i++) object_death.push_back(0);
	for (i = 0; i < odesc.size(); i++) object_used.push_back(0);
	for (i = 0; i < 12; i++) equipped.push_back(NULL);

	//here we get the argument for the number of monsters
	j = 0;
	for (i = 1; i < argc; i++)
	{
			if (!(strcmp(argv[i], "--nummon")))
			{
				j = 1;
				break;
			}

	}

	if (j) j = atoi(argv[i+1]);
	else j = 10;
	//processing for nummon tags beings here
	//cout<<"getting num of mons "<<j<<endl;
	//cout<<"total desc "<<monsters.size()<<endl;
	//cout<<"monsters death ledger: "<<monster_death.size()<<endl;
	//printf("\n\n\n");
	PC* pc;
	initialize_pc(&pc);//this is where pc gets initialised
	//printf("\nPC has been initialised; the coordinates accessible from main are %d, %d\n", pc->x, pc->y);
	initialize_players(j, pc);//this initialises all the bots
	player_node_heap* h;
	populate_heap(&h);//all monsters and pc get loaded on a heap
	populate_dungeon_objects();

	//for (i = 0; i< odesc.size(); i++) cout<<getObjChar(odesc[i].type)<<endl;
	// for (i = 0; i < ylenMax; i++){
	// 	for (j = 0; j < xlenMax; j++){
	// 		if (grid_objects[j][i]==NULL) cout<<" ";
	// 		else cout<<getObjChar(grid_objects[j][i]->d->type);
	// 	}
	// 	cout<<endl;
	// }

	init_terminal();
	print_dungeon();
	i = 0;
	player_node* curr = NULL;
	while (!(i))
	{
		pop_player(h, &i, &curr);//this is where a node 'curr' gets selected to make next move
		if (!(i))//if its not the case that theres only one player left, then game goes on or else pc wins
		{
			next_move(curr, pc ,&i, h);//move prints and adds a 0.25s sleep as well


			/*Below the whole heap can be made visible for debugging*/
			// while(curr2!=NULL)
			// player_node* curr2= h->head;
			// {
			// 	printf("%d-%d->",curr2->next_turn, curr2->ifPC);
			// 	curr2 = curr2->next;
			// }
			// printf("\n");

			//if i=15..random_generator(&h, &pc, j); ->free heap. kill_all, {generate new dungeon} initialise pc. initialise players
		}
		if (i == 15)
		{
			random_generator(&h, &pc, j, &rooms);
			i=0;
		}

	}
	if (i==2){
		print_dungeon();
		usleep(2000000);
		print_end(0);//printf("\n\n\n\n\n\n\n\n\n\n\n\nPC LOST\n\n\n\n\n\n\n\n");
	}
	//else if (i==3)print_end(1);//printf("\n\n\n\n\n\n\n\n\n\n\n\nPC WON\n\n\n\n\n\n\n\n");
	else if (i==5) print_end(5);///prints quit
	else if (i==4) print_end(1);//printf("\n\n\n\n\n\n\n\n\n\n\n\nPC WON\n\n\n\n\n\n\n\n");

	endwin();

	kill_all();
	free(h);


	//we free up all the objects in the grid
	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++){
			if (grid_objects[j][i] != NULL){
				free(grid_objects[j][i]);
				grid_objects[j][i] = NULL;
			}
		}
	}

	object_used.clear();
	object_death.clear();
	monster_used.clear();
	monster_death.clear();
	monsters.clear();
	odesc.clear();

	equipped.clear();
	inventory.clear();


	//Now we check to see if there's a save switch to update the /.rlg327/dungeon
	j = 0;
	for (i = 1; i < argc; i++)
	{
			if (!(strcmp(argv[i], "--save")))
			{
				j = 1;
				break;
			}

	}
	//processing for save tags beings here
	if (j)
	{
		//printf("save found\n");
		FILE *f;

		string home = getenv("HOME");
		string gamedir = ".rlg327";
		string savefile = "dungeon";
		string path = home + "/"+gamedir+"/"+savefile;
		//malloc(strlen(home) + strlen(gamedir) + strlen(savefile) + 2 + 1);
		//sprintf(path, "%s/%s/%s", home, gamedir, savefile);

		if( !( f = fopen( path.c_str(), "w"))) {/*printf("Failed to open file\n");*/ return 1;}
		//free(path);

		string marker = "RLG327-S2021";
		fwrite(marker.c_str(), sizeof(char), 12, f);

		uint32_t version = 0;
		version = htobe32(version);
		fwrite(&version, sizeof(uint32_t), 1, f);

		//calculate the size of the file, meanwhile the size is taken to be zero
		uint32_t size = 1708 + (4 * numRooms) + (2 * (numUpstairs + numDownstairs));
		size = htobe32(size);
		fwrite(&size, sizeof(uint32_t), 1, f);

		//now we enter position of the PC
		fwrite(&xPCpos, sizeof(uint8_t), 1, f);
		fwrite(&yPCpos, sizeof(uint8_t), 1, f);


		//next we write the dungeon matrix - we will have to artificially populate the file with max hardness on border
		uint8_t temp8;

		for (j = 0; j < xlenMax + 2; j++)
		{
			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);
		}

		for (i = 0; i < ylenMax; i++)
		{
			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			for (j = 0; j < xlenMax; j++)
			{
				temp8 = hardness[j][i];
				fwrite(&temp8, sizeof(uint8_t), 1, f);
			}

			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

		}

		for (j = 0; j < xlenMax + 2; j++)
		{
			temp8 = 255;
			fwrite(&temp8, sizeof(uint8_t), 1, f);
		}

		//number of Rooms are entered here
		uint16_t temp16 = numRooms;
		temp16 = htobe16(temp16);

		fwrite(&temp16, sizeof(uint16_t), 1, f);

		//mext we write the coordinates of the room
		for (i = 0; i < numRooms; i++)
		{
			temp8 = 1 + rooms[i].xloc;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			temp8 = 1 + rooms[i].yloc;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			temp8 = rooms[i].xlen;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

			temp8 = rooms[i].ylen;
			fwrite(&temp8, sizeof(uint8_t), 1, f);

		}

		//here we process the number of upstairs
		temp16 = numUpstairs;
		temp16 = htobe16(temp16);
		fwrite(&temp16, sizeof(uint16_t), 1, f);

		//here we enter the coordinates of upstairs
		for (j = 0; j < ylenMax; j++)
		{
			for (k = 0; k < xlenMax; k++)
			{
				if (grid[k][j] == '<')
				{
					temp8 = k + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);


					temp8 = j + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);

				}
			}
		}

		//here we process the number of downstairs
		temp16 = numDownstairs;
		temp16 = htobe16(temp16);
		fwrite(&temp16, sizeof(uint16_t), 1, f);

		//here we enter the coordinates of downstairs
		for (j = 0; j < ylenMax; j++)
		{
			for (k = 0; k < xlenMax; k++)
			{
				if (grid[k][j] == '>')
				{
					temp8 = k + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);

					temp8 = j + 1;
					fwrite(&temp8, sizeof(uint8_t), 1, f);

				}
			}
		}


		fclose(f);
	}
	//processing for save ends here

	free(rooms);
	return 0;
}
/*The first is the makes_sense function that takes the array of rooms and number of Rooms as the argument.
It tries to see if the top edge of one room coincides with area occupied with all the other rooms.
This it does in four ways - first it check if the y coordinates of the top edge are inside
the range+1(1 is added to keep a gap of 1) of the other rooms. If a room is coincides on
 the y-coordinates, then we check the x-coordinates. This we do in three ways, by checking the left corner,
 the right corner and the middle. If any of these indicate intersection then the program ends soonafter because
 it 'doesnt make sense' so the random function will pick some other coordinates. This function also makes sure
 the rooms are within the grid we have and don't exceed.*/
int makes_sense(room rooms[], int numRooms)
{

	int checker = 1;//this essentially marks whether the program makes any sense, 0 indicates it doesnn't

	for (int i = 0; i < numRooms; i++)
	{
		for (int j = 0; j < numRooms; j++)
		{
			if (i != j)
			{
				//first it check if the y coordinates of the top edge are inside the range+1(1 is added to keep a gap of 1) of the other rooms. If a room is coincides on the y-coordinates, then we check the x-coordinates
				if(
				rooms[i].yloc >= rooms[j].yloc &&
				rooms[i].yloc <= (rooms[j].yloc + rooms[j].ylen + 1)
				)
				{
					//If a room is coincides on the y-coordinates, then we check the x-coordinates by checking the left corner, the right corner and the middle.
					if(
					(rooms[i].xloc >= rooms[j].xloc &&//this checks left corner
					rooms[i].xloc <= (rooms[j].xloc + rooms[j].xlen + 1)
					) || (
					rooms[i].xloc + rooms[i].xlen >= rooms[j].xloc &&//this checks right corner
					rooms[i].xloc + rooms[i].xlen <= (rooms[j].xloc + rooms[j].xlen + 1)
					) || (
					rooms[i].xloc < rooms[j].xloc &&//this checks middle
					rooms[i].xloc + rooms[i].xlen > (rooms[j].xloc + rooms[j].xlen + 1)
					)
					) checker = 0;
				}


			}

			if (checker == 0) break;//this helps end the program soon if the coordinates don't make sense

		}

		if (//this condition just makes sure all the coordinates will map the rooms in the available grid
			(rooms[i].xloc + rooms[i].xlen > xlenMax - 1) ||
			(rooms[i].yloc + rooms[i].ylen > ylenMax - 1)
			) checker = 0;

		if (checker == 0) break;//this, along with the break above, ensures we swiftly end the program soon if the coordinates don't make sense
	}

	return checker;
}

/*The second function simply creates a number that we use as a denominator for calculating max_size of the rooms.
The reason for this function was because the program took too long to find coordinates that made sense of number
of rooms greater than 8. This restricts the random function a little bit more - hence the not_so_random part of the name.*/
int not_so_rand_roomsize_resizer(int numRooms)
{
	int roomSizer = (numRooms/2) - 1;

	return roomSizer;
}
/* The print_dungeon method below simply takes the x and y coordinates of the PC to make sure the @ is at the right position*/
int print_dungeon()
{
	int i, j, x;//x will serve as  the top offset
	char npc2;
	x=1;

	//for (i = 0; i < xlenMax; i++) {printf("-");}
	//printf("\n");

	for (i = 0; i < ylenMax; i++)
	{
		//printf("|");
		for (j = 0; j < xlenMax; j++)
		{
			attron(COLOR_PAIR(COLOR_WHITE));
			mvaddch(i+x,j, grid[j][i]);
			//if (grid_players[j][i]==NULL) printf("%c", grid[j][i]);
			if (grid_players[j][i]==NULL) mvaddch( i+x,j, grid[j][i]);
			else
			{
				if (grid_players[j][i]->ifPC==1) mvaddch(i+x,j, '@');//printf("@");
				else
				{
					//char* store = sprintf("%x",(grid_players[j][i]->npc->character));
					char npc = grid_players[j][i]->npc->desc->symb;
					attron(COLOR_PAIR(grid_players[j][i]->npc->desc->color[0]));
					mvaddch(i+x,j, npc);
					// if (npc <= 9){
					// npc2=npc+'0';
					// mvaddch(i+x,j, npc2);
					// }
					// else{
					// 	npc2 = npc+'a'-10;
					// 	mvaddch(i+x,j, npc2);
					// }
				}
			}
		}
		//printf("\n");
	}
	refresh();

	//for (i = 0; i < xlenMax; i++) {printf("-");}
	//printf("\n\n\n");
	return 0;
}
/* This will only print the dungeon close to the PC if visible is false or 0, else, it would print regular dungeon*/
int print_dungeon_limited(PC* pc)
{
	int i, j, x;//x will serve as  the top offset
	char npc2;
	x=1;
	//for (i = 0; i < xlenMax; i++) {printf("-");}
	//printf("\n");
	if (visible)
	{
		for (i = 0; i < ylenMax; i++)
		{
			//printf("|");
			for (j = 0; j < xlenMax; j++)
			{
				attron(COLOR_PAIR(COLOR_WHITE));
				mvaddch(i+x,j, grid[j][i]);
				//if (grid_players[j][i]==NULL) printf("%c", grid[j][i]);
				if (grid_players[j][i]==NULL)
				{
					if (grid_objects[j][i]==NULL)
					mvaddch( i+x,j, grid[j][i]);
					else {
						attron(COLOR_PAIR(grid_objects[j][i]->d->color[0]));
						mvaddch( i+x,j, getObjChar(grid_objects[j][i]->d->type));
					}

				}
				else
				{
					if (grid_players[j][i]->ifPC==1)
					{
						attron(COLOR_PAIR(COLOR_RED));
						mvaddch(i+x,j, '@');

					}//printf("@");
					else
					{
						//char* store = sprintf("%x",(grid_players[j][i]->npc->character));
						//int npc = grid_players[j][i]->npc->character;
						char npc = grid_players[j][i]->npc->desc->symb;
						attron(COLOR_PAIR(grid_players[j][i]->npc->desc->color[0]));
						mvaddch(i+x,j, npc);
						// if (npc <= 9){
						// npc2=npc+'0';
						// mvaddch(i+x,j, npc2);
						// }
						// else{
						// 	npc2 = npc+'a'-10;
						// 	mvaddch(i+x,j, npc2);
						// }
					}
				}
			}
			//printf("\n");
		}
		refresh();
	}
	else
	{
		//first we make sure everything is dark
		for (i = 0; i < ylenMax; i++)
		{
			for (j = 0; j < xlenMax; j++)
			{
				attron(COLOR_PAIR(COLOR_WHITE));
				mvaddch(i+x,j, remembered[j][i]);
			}
		}
		//next we figure out the limits of the 5x5 matrix around the pc
		// int Ymin = (0 < ((pc->y) - 2)) ? (pc->y - 2) : 0;
		// int Xmin = (0 < ((pc->x) - 2)) ? (pc->x - 2) : 0;
		// int Ymax = ((ylenMax-1) > ((pc->y) + 2)) ? (pc->y + 2) : (ylenMax-1);
		// int Xmax = ((xlenMax-1) > ((pc->x) + 2)) ? (pc->x + 2) : (xlenMax-1);

		int Ymin = (0 < ((pc->y) - radius)) ? (pc->y - radius) : 0;
		int Xmin = (0 < ((pc->x) - radius)) ? (pc->x - radius) : 0;
		int Ymax = ((ylenMax-1) > ((pc->y) + radius)) ? (pc->y + radius) : (ylenMax-1);
		int Xmax = ((xlenMax-1) > ((pc->x) + radius)) ? (pc->x + radius) : (xlenMax-1);


		for (i = Ymin; i <= Ymax; i++)
		{
			for (j = Xmin; j <= Xmax; j++)
			{
				int dist_sqr = ((j - pc->x)*(j - pc->x)) + ((i - pc->y)*(i - pc->y));
				if (dist_sqr <= ((radius*radius)+1))
				{
				// if (grid_players[j][i]==NULL)
				// {
				// 	attron(COLOR_PAIR(COLOR_WHITE));
				// 	mvaddch( i+x,j, grid[j][i]);
				// 	remembered[j][i] = grid[j][i];
				// }

					if (grid_players[j][i]==NULL)
					{
						if (grid_objects[j][i]==NULL)
						{
							attron(COLOR_PAIR(COLOR_WHITE));
							mvaddch( i+x,j, grid[j][i]);
							remembered[j][i] = grid[j][i];
						}
						else {
							attron(COLOR_PAIR(grid_objects[j][i]->d->color[0]));
							mvaddch( i+x,j, getObjChar(grid_objects[j][i]->d->type));
						}

					}
					else
					{
						if (grid_players[j][i]->ifPC==1)
						{
							attron(COLOR_PAIR(COLOR_RED));
							mvaddch(i+x,j, '@');//printf("@");
						}
						else
						{
							//char* store = sprintf("%x",(grid_players[j][i]->npc->character));
							// int npc = grid_players[j][i]->npc->character;
							char npc = grid_players[j][i]->npc->desc->symb;
							//char color = grid_players[j][i]->npc->desc.color;
							attron(COLOR_PAIR((char)grid_players[j][i]->npc->desc->color[0]));
							mvaddch(i+x,j, npc);
							// if (npc <= 9){
							// npc2=npc+'0';
							// mvaddch(i+x,j, npc2);
							// }
							// else{
							// 	npc2 = npc+'a'-10;
							// 	mvaddch(i+x,j, npc2);
							// }
						}
						remembered[j][i] = grid[j][i];
					}

				}
			}
		}

		refresh();


	}


	//for (i = 0; i < xlenMax; i++) {printf("-");}
	//printf("\n\n\n");
	return 0;
}

/*This should print the whole dungron alongside printing the estrick where we might want to teleport the PC*/
int print_teleport(PC* pc, int* x, int* y)
{
	int i, j;
	char npc2;
	int offset=1;
	//for (i = 0; i < xlenMax; i++) {printf("-");}
	//printf("\n");
	if (visible)
	{
		for (i = 0; i < ylenMax; i++)
		{
			//printf("|");
			for (j = 0; j < xlenMax; j++)
			{
				attron(COLOR_PAIR(COLOR_WHITE));
				mvaddch(i+offset,j, grid[j][i]);
				//if (grid_players[j][i]==NULL) printf("%c", grid[j][i]);
				if (grid_players[j][i]==NULL)
				{
					if (grid_objects[j][i]==NULL)
					mvaddch( i+offset,j, grid[j][i]);
					else {
						attron(COLOR_PAIR(grid_objects[j][i]->d->color[0]));
						mvaddch( i+offset,j, getObjChar(grid_objects[j][i]->d->type));
					}

				}
				else
				{
					if (grid_players[j][i]->ifPC==1) mvaddch(i+offset,j, '@');//printf("@");
					else
					{
						//char* store = sprintf("%x",(grid_players[j][i]->npc->character));
						char npc = grid_players[j][i]->npc->desc->symb;
						attron(COLOR_PAIR(grid_players[j][i]->npc->desc->color[0]));
						mvaddch(i+offset,j, npc);
						// if (npc <= 9){
						// npc2=npc+'0';
						// mvaddch(i+offset,j, npc2);
						// }
						// else{
						// 	npc2 = npc+'a'-10;
						// 	mvaddch(i+offset,j, npc2);
						// }
					}
				}
			}
			//printf("\n");
		}

	}
	else
	{
		//first we make sure everything is dark
		for (i = 0; i < ylenMax; i++)
		{
			for (j = 0; j < xlenMax; j++)
			{
				mvaddch(i+offset,j, ' ');
			}
		}

		int Ymin = (0 < ((pc->y) - 2)) ? (pc->y - 2) : 0;
		int Xmin = (0 < ((pc->x) - 2)) ? (pc->x - 2) : 0;
		int Ymax = ((ylenMax-1) > ((pc->y) + 2)) ? (pc->y + 2) : (ylenMax-1);
		int Xmax = ((xlenMax-1) > ((pc->x) + 2)) ? (pc->x + 2) : (xlenMax-1);

		for (i = Ymin; i <= Ymax; i++)
		{
			for (j = Xmin; j <= Xmax; j++)
			{
				if (grid_players[j][i]==NULL)
				{
					if (grid_objects[j][i]==NULL)
					{
						attron(COLOR_PAIR(COLOR_WHITE));
						mvaddch( i+offset,j, grid[j][i]);
					}
					else {
						attron(COLOR_PAIR(grid_objects[j][i]->d->color[0]));
						mvaddch( i+offset,j, getObjChar(grid_objects[j][i]->d->type));
					}
				}
				else
				{
					if (grid_players[j][i]->ifPC==1) mvaddch(i+offset,j, '@');//printf("@");
					else
					{
						//char* store = sprintf("%x",(grid_players[j][i]->npc->character));
						// int npc = grid_players[j][i]->npc->character;
						// if (npc <= 9){
						// npc2=npc+'0';
						// mvaddch(i+offset,j, npc2);
						// }
						// else{
						// 	npc2 = npc+'a'-10;
						// 	mvaddch(i+offset,j, npc2);
						// }
						char npc = grid_players[j][i]->npc->desc->symb;
						attron(COLOR_PAIR(grid_players[j][i]->npc->desc->color[0]));
						mvaddch(i+offset,j, npc);
					}
				}
			}
		}

	}

	mvaddch( *y+offset,*x, '*');
	refresh();
	return 0;
}
/*This will be used to control the esterik '*' once we are in the teleport mode after pressing 'g', pressing 'g' again is the only way to leave*/
int teleport_controller(PC* pc, int& x, int& y)
{
	start:
	print_teleport(pc, &x, &y);
	chtype ch = getch();

	if (ch == '8'||ch == 'k')
	{
		if (y!=0) y--;
		goto start;
	}
	else if (ch == '7'||ch == 'y')
	{
		if (y!=0) y--;
		if (x!=0) x--;
		goto start;
	}
	else if (ch == '9'||ch == 'u')
	{
		if (y!=0) y--;
		if (x!=(xlenMax-1)) x++;
		goto start;
	}
	else if (ch == '6'||ch == 'l')
	{
		if (x!=(xlenMax-1)) x++;
		goto start;
	}
	else if (ch == '1'||ch == 'b')
	{
		if (x!=0) x--;
		if (y!=(ylenMax-1)) y++;
		goto start;
	}
	else if (ch == '2'||ch == 'j')
	{
		if (y!=(ylenMax-1)) y++;
		goto start;
	}
	else if (ch == '3'||ch == 'n')
	{
		if (y!=(ylenMax-1)) y++;
		if (x!=(xlenMax-1)) x++;
		goto start;
	}
	else if (ch == '4'||ch == 'h')
	{
		if (x!=0) x--;
		goto start;
	}
	else if (ch == 'g')
	{
		if (grid[x][y]==' ')
		{
			hardness[x][y] = 0;
			grid[x][y] = '#';
		}
		return 0;
	}
	else goto start;

}


/*This will be used to calculate the shortest distance for the monsters
shortPathRecord keeps tab of what is inside the queue
node_heap is used to keep all the coordinates in a priority queue
the while loops keep doing till there's nothing left to process
if the distance from current node is shorter than the one the neighbour cell has then it is checked if the neighbour is already on queue. If it isn't then its pushed on the priority queue
The method takes the coordinates of PC as the argument and whether or not the distance that we're finding for is a digger or not*/
int djik (int xcoordinate, int ycoordinate, int ifdigger)
{
	int i, j, k, x, y;


	//next we calculate shortest distance for non-tunnelers
	if(!(ifdigger))
	{
		for (i = 0; i < ylenMax; i++){
			for(j = 0; j < xlenMax; j++) difficulty[j][i] = INT_MAX;
		}
	}
	else
		{
		for (i = 0; i < ylenMax; i++){
			for(j = 0; j < xlenMax; j++) difficulty_t[j][i] = INT_MAX;
		}
	}


	//first we initialise the set of shortPathRecord to zero so nothing is taken to be processed by djik algo
	for (i = 0; i < ylenMax; i++){
		for (j = 0; j < xlenMax; j++)
		{
			shortPathRecord[j][i] = 0;
		}
	}

	node_heap* newH;
	newH = (node_heap*) malloc(sizeof(node_heap));
	newH->size = 0;
	newH->tail = NULL;
	newH->head = NULL;

	push (newH, xcoordinate, ycoordinate);
	shortPathRecord [xcoordinate][ycoordinate] = 1;
	difficulty[xcoordinate][ycoordinate] = 0;
	difficulty_t[xcoordinate][ycoordinate] = 0;

	while(newH->size > 0)
	{
		pop(newH, &i, &j);
		shortPathRecord [i][j] = 0;
		neighbourhood currN;
		getNeighbour(i, j, &currN);

		int init_diff;
		if (ifdigger)	init_diff	= difficulty_t[i][j];
		else init_diff = difficulty[i][j];

		int diff_curr_block;
		if (grid[i][j]== ' ') diff_curr_block = 1 + (hardness[i][j]/85);
		else diff_curr_block = 1;

		for (k = 0; k < currN.size; k++)
		{
			x = currN.store[k][0];
			y = currN.store[k][1];

			if (!(ifdigger))
			{
				if (grid[x][y] != ' ')
				{
					//this is where we process the diggers

					if (difficulty[x][y] > (init_diff + diff_curr_block))
					{
						difficulty[x][y] = init_diff + diff_curr_block;
						//check to see if it is already on the processed stack
						if (!shortPathRecord[x][y])
						{
							push(newH, x, y);
							shortPathRecord[x][y] = 1;
						}
					}
			}
			}
			else
			{
				//this is where we process the diggers

				if (difficulty_t[x][y] > (init_diff + diff_curr_block))
				{
					difficulty_t[x][y] = init_diff + diff_curr_block;
					//check to see if it is already on the processed stack
					if (!shortPathRecord[x][y])
					{
						push(newH, x, y);
						shortPathRecord[x][y] = 1;
					}
				}

			}

		}


	}
	free(newH);
	return 0;
}
/*This looks up the coordinates of all the cells around the targeted one. The coordinates of the targeted cell and the neighbourgood that we need to populate*/
int getNeighbour(int x, int y, neighbourhood* n){
	int i,j;
	int size = 0;
	int store[8][2]; //= n->store;
	//we start from the left and the move in the clockwise fashion
	if (x > 0) {
		store[size][0] = x - 1;
		store[size][1] = y;
		size++;
	}
	if ( (x > 0) && (y > 0) ){
		store[size][0] = x - 1;
		store[size][1] = y - 1;
		size++;
	}
	if (y > 0) {
		store[size][0] = x;
		store[size][1] = y - 1;
		size++;
	}
	if ( (x < (xlenMax - 1)) && (y > 0) ){
		store[size][0] = x + 1;
		store[size][1] = y - 1;
		size++;
	}
	if (x < (xlenMax - 1)) {
		store[size][0] = x + 1;
		store[size][1] = y;
		size++;
	}
	if ( (x < (xlenMax - 1)) && (y < (ylenMax - 1)) ){
		store[size][0] = x + 1;
		store[size][1] = y + 1;
		size++;
	}
	if (y < (ylenMax - 1)) {
		store[size][0] = x;
		store[size][1] = y + 1;
		size++;
	}
	if ( (x > 0) && (y < (ylenMax - 1)) ){
		store[size][0] = x - 1;
		store[size][1] = y + 1;
		size++;
	}

//finally getting the copy in neighbourhood
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < size; j++){
			n->store[j][i] = store[j][i];
		}
	}
	n->size = size;

	return 0;
}
/*this method takes the coordinates of the cell and the heap that we're suppose to push them into. A new node will be created and pushed into being the head. If the size is zero, the head and tail will be the newNode. If it isnt, then its simply the head*/
int push(node_heap* nh, int x, int y)
{
	node *newNode = (node*) malloc(sizeof(node));
	newNode->xcoor = x;
	newNode->ycoor = y;
	newNode->next = NULL;

	if (!(nh->size)){
		newNode->prev = NULL;
		nh->tail = newNode;
		nh->head = newNode;
		nh->size++;
	}
	else{
		nh->head->next = newNode;
		newNode->prev = (nh->head);
		nh->head = newNode;
		nh->size++;
	}

	return 0;

}
/*This is used to save the results of the node on the tail in the integer addresses provided and then free the node in the tail*/
int pop(node_heap* nh, int* x, int* y)
{
	if (nh->size == 1)
	{
		*x = nh->tail->xcoor;
		*y = nh->tail->ycoor;
		free(nh->tail);
	}
	else
	{
		node* temp;
		*x = nh->tail->xcoor;
		*y = nh->tail->ycoor;
		temp = nh->tail;
		nh->tail = nh->tail->next;
		nh->tail->prev = NULL;
		free(temp);

	}
	nh->size -= 1;
	return 0;

}

/*This method is used to make sure that monsters initialise away from PC and creates a 9x9 square centered at the pc where no npc can intialise*/
int distant_from_pc(PC* p, int x, int y){
	int PCx,PCy;
	//printf("we're in distant method");
	PCx = p->x;
	PCy=p->y;
	//printf("PC is at (%d,%d)", PCx,PCy);
	if ((x > (PCx+4))||(x < (PCx-4))) return 1;
	if ((y > (PCy+4))||(y < (PCy-4))) return 1;
	return 0;
}
/*this is used to take a pc struct's pointer and create a node and place it in random spot.*/
int initialize_pc(PC** pc)
{
	(*pc) = (PC*) malloc(sizeof(PC));
	int i, j, k;
	i = 1;
	while (i)
	{
		k = rand()%xlenMax;
		j = rand()%ylenMax;

		if (grid[k][j] == '.') i = 0;


	}

	(*pc)->x = k;
	(*pc)->y = j;
	(*pc)->hp = 2000;
	(*pc)->totDam = 0;
	(*pc)->dam[0] = 0;
	(*pc)->dam[1] = 1;
	(*pc)->dam[2] = 4;
	(*pc)->speed = 10;


	player_node* pn = (player_node*) malloc(sizeof(player_node));
	pn->ifPC = 1;
	pn->alive = 1;
	pn->pc = (*pc);
	pn->next_turn = 0;
	pn->when_initiated = player_init_counter++;
	//printf("\nx: %d y: %d\n", k,j);
	grid_players[k][j] = pn;

	return 0;
}
/* This is used to create all the npc's as per the specs. There is a distance of atleast 4 from PC*/
int initialize_players(int n, PC* p)
{
	int i, j, k , t;


	for (t = 0; t < n; t++)
	{

		NPC *npc = (NPC*) malloc(sizeof(NPC));

		//printf("good so far\n");

		i= 1;
		while (i)
		{

			k = rand()%xlenMax;
			j = rand()%ylenMax;

			//printf("---random find:: x is %d and y is %d\n", k,j);

			if (grid[k][j] == '.' && grid_players[k][j]==NULL && distant_from_pc(p, k, j)) i = 0;


		}

		//printf("here! after initialisation");

		npc->x = k;
		npc->y = j;


		//printf("x is %d and y is %d\n", k,j);
		npc->tangle = 0;
		npc->index = findMonster();
		monster_desc* desc = &monsters[npc->index];
		npc->desc = desc;

		npc->character = desc->abil;//rand()&0xf;//any character netween 0-15
		if (npc->character & BOSS) bossFound = 1;
		//cout<<"variables found"<<endl;
		npc->speed = roll_dice(desc->speed);//5+ (rand()&0xf);//speed randomly gets selected from 5-20
		//cout<<"variables found"<<endl;
		npc-> ifPCseen = 0;
		npc->hp = roll_dice(desc->hp);
		//cout<<"variables found"<<endl;

		player_node* pn = (player_node*) malloc(sizeof(player_node));
		pn->ifPC = 0;
		pn->alive = 1;
		pn->npc = npc;
		pn->next_turn = 0;
		pn->when_initiated = player_init_counter++;
		//printf("x: %d y: %d\n", k,j);
		grid_players[k][j] = pn;

	}

	return 0;

}
/*This is a clean-up mechanism that makes sure all remaining players(malloc'd) are freed.*/
int kill_all()
{
	int i, j;
	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++)
		{
			if (grid_players[j][i]!=NULL)
			{
				if (grid_players[j][i]->ifPC)
				{
					int tempx = (grid_players[j][i]->pc->x);
					int tempy = (grid_players[j][i]->pc->y);
					free(grid_players[j][i]->pc);
					free(grid_players[j][i]);
					grid_players[tempx][tempy] = NULL;
				}
				else
				{
					int tempx = grid_players[j][i]->npc->x;
					int tempy = grid_players[j][i]->npc->y;
					free(grid_players[j][i]->npc);
					free(grid_players[j][i]);
					grid_players[tempx][tempy] = NULL;
				}
			}
		}
	}
	return 0;

}

/*populates our heap with all the players in the grid_players using the player_push method*/
int populate_heap(player_node_heap** h)
{
	(*h) = (player_node_heap*) malloc(sizeof(player_node_heap));
	(*h)->head = NULL;
	(*h)->tail = NULL;
	for (int i = 0; i < ylenMax; i++)
	{
		for (int j = 0; j < xlenMax; j++)
		{
			if(grid_players[j][i]!=NULL) push_player_node((*h), grid_players[j][i]);
		}
	}
	return 0;
}
/*This is used to push all the new nodes into the heap*/
int push_player_node(player_node_heap* h, player_node* p)
{
	if (h->head==NULL)//nothing in the heap
	{
		h->head = p;
		h->tail = p;
		p->prev = NULL;
		p->next = NULL;
		return 0;
	}
	else
	{
		h->tail->next = p;
		p->prev = h->tail;
		h->tail = p;
		p->next = NULL;
		return 0;
	}

}

/*This is used to select a node in the heap based on the specs (lowest -> next_turn, when_initiated) */
int pop_player(player_node_heap* nh, int* ifend, player_node** output)
{
	player_node* p = nh->head;
	if(p==NULL)
	{
		*ifend = 1;
		return 0;
	}
	if(p->next==NULL)
	{
		//*ifend = 3;
		return 0;
	}
	int min_turn = p->next_turn;
	int min_when_initiated = p->when_initiated;
	player_node* min_node = p;
	p = p->next;
	while(p!=NULL)
	{
		if ((p->next_turn) < min_turn)
		{
			min_node = p;
			min_when_initiated = p->when_initiated;
			min_turn = p->next_turn;
		}
		else if((p->next_turn) == min_turn)
		{
			if ((p->when_initiated) < min_when_initiated)
			{
				min_node = p;
				min_when_initiated = p->when_initiated;
			}
		}
		p = p->next;
	}
	//if p->ifpc then print dungeon
	//next_move(min_node, ifend);
	*output = min_node;
	return 0;

}

/*This is used to kill a player, remove it from the heap and free the malloc'd node*/
int kill_player(player_node* p, player_node_heap* h)
{
	//player_node* previous =
	if ((p->prev)!=NULL)
	{
		if (p->next==NULL) p->prev->next = NULL;
		else p->prev->next = p->next;
	}
	else //this is a head
	{
		if (p->next==NULL)
		h->head = NULL;
		else h->head = p->next;
	}
	if ((p->next)!=NULL)
	{
		p->next->prev = p->prev;
	}
	else //this is tail
	{
		if (p->prev==NULL) h->tail =NULL;
		else h->tail = p->prev;
	}

	if (p->ifPC==1)
	{
		int tempx = p->pc->x;
		int tempy = p->pc->y;
		free(p->pc);
		free(p);
		grid_players[tempx][tempy] = NULL;
	}
	else
	{
		int tempx = p->npc->x;
		int tempy = p->npc->y;
		monster_death[p->npc->index] = 1;
		free(p->npc);
		free(p);
		grid_players[tempx][tempy] = NULL;
	}
	return 0;
}

/*This is used to figure out what to do with the node selected with pop_player. nextx, and nexty are set to current location of the bot and will determine where it moves next.
This method first checks if bot is tele or not. If it is tele then ifPCseen gets updated to 1, and PCx and PCy get updated to current PC location.
If not tele, then if_in_room updates to see if tele can see the PC.
Then we check to see if bot it intelligent. If intelligent and if PC is seen- then we will use djik algo (based on PCx and PCy and if digger or not) then update nextx and nexty.
If not intelligent, then if PC is seen, then bot moves the nextx, and nexty, horizontally and then vertically towards the location known to the bot.
If the bot is erratic, then we check random, and there's a 50 percent chance neigbouring cell will be used for nextx or nexty, other 50percent chance, next coordinates remain unchanged.
Then the nextx and nexty is processed - as per the specs. First we check if there's a bot sitting on the next coordinates. If there is (and it isn't the current one) then we kill it and remove it from the heap and the grid_players. If there is no bot- action will depend on whether it can tunnel and what the hardness is.*/
int next_move(player_node *pn, PC* pc, int* ifend, player_node_heap* h)
{

	if (pn->ifPC==1) {
		//printf("\nPC's turn, score of %d \n", pn->next_turn);
		int nextx, nexty;
		print_dungeon_limited(pn->pc);
		getkey(pn->pc->x, pn->pc->y, &nextx, &nexty, ifend, h);

		//boundary check
		if (nextx < 0 || nextx >= xlenMax) nextx = pn->pc->x;
		if (nexty < 0 || nexty >= ylenMax) nexty = pn->pc->y;

		if ( (pn->pc->x!=nextx) || (pn->pc->y!=nexty) )
		{
			int killed = 1;
			if(grid_players[nextx][nexty]!=NULL)
			{
				//kill_player(grid_players[nextx][nexty], h);
				readyPC();
				PC* p = pn->pc;
				NPC* n = grid_players[nextx][nexty]->npc;
				//kill_player(grid_players[nextx][nexty], h);
				//engaging in a battle
				untangle();
				n->tangle = 1;
				for (int i = 0; i < xlenMax; i++) mvaddch(0,i,' ');
				string temp = "You tangled ";
				temp+= n->desc->name;

				//for (int i = 0; i<temp.size(); i++) mvaddch(0,i,temp[i]);
				temp += " by damage of " + to_string(p->totDam);
				temp+= " with remaing hp-> ";

				n->hp -= p->totDam;
				if (n->hp < 0) temp += "zero";
				else temp +=  to_string (n->hp);
				attron(COLOR_PAIR(COLOR_WHITE));
				for (int i = 0; i<temp.size(); i++) mvaddch(0,i,temp[i]);

				if (n->hp < 0)
				{
					if((n->character) & BOSS) *ifend = 4;
					kill_player(grid_players[nextx][nexty], h);

				}
				else
				{
					nextx = p->x;
					nexty = p->y;
					killed = 0;
				}

			}
			if (killed)
			{
				if(grid_objects[nextx][nexty]!=NULL)
				{
					//do dosomething
					if (inventory.size() < 10)
					{
						inventory.push_back(grid_objects[nextx][nexty]);
						object_death[grid_objects[nextx][nexty]->index] = 1;
						grid_objects[nextx][nexty] = NULL;
					}
				}

				grid_players[pn->pc->x][pn->pc->y] = NULL;
				pn->pc->x = nextx;
				pn->pc->y = nexty;
				grid_players[pn->pc->x][pn->pc->y] = pn;


				//wall check
				if (grid[pn->pc->x][pn->pc->y] == ' ')
				{
					grid[pn->pc->x][pn->pc->y] = '#';
					hardness[pn->pc->x][pn->pc->y] = 0;
				}
			}

		}
		for (int i = ylenMax; i <= attackLine; i++)
		for (int j = 0; j < xlenMax; j++)
		mvaddch(i, j, ' ');

		attackLine = ylenMax;

		pn->next_turn = pn->next_turn + (1000/(pn->pc->speed));
		print_dungeon_limited(pn->pc);
		//usleep(10000);

		return 0;

		// getNeighbour(pn->pc->x, pn->pc->y, n);
		// for (int i = 0; i < n->size; i++)
		// {
		// 	if(grid_players[n->store[i][0]][n->store[i][1]]!=NULL)
		// 	{
		// 		kill_player(grid_players[n->store[i][0]][n->store[i][1]], h);
		// 		//player_node* temp = pn;
		// 		grid_players[pn->pc->x][pn->pc->y] = NULL;
		// 		pn->pc->x = n->store[i][0];
		// 		pn->pc->y = n->store[i][1];
		// 		grid_players[n->store[i][0]][n->store[i][1]] = pn;
		// 		break;
		// 	}
		// }
		//
		//
		// pn->next_turn = pn->next_turn + (1000/(pn->pc->speed));
		// usleep(250000);
		// print_dungeon();
		// free(n);
		// //printf("\n\n\n");
		// return 0;

	}

	//printf("\nNPC with a score of %d at x,y: %d,%d named %x, being moved to  ",pn->next_turn, pn->npc->x,pn->npc->y,pn->npc->character);
	//now we've established that the node is not of PC
	int character = pn->npc->character;
	int x = pn->npc->x;
	int y = pn->npc->y;
	neighbourhood* n;
	n = (neighbourhood*) malloc(sizeof(neighbourhood));
	getNeighbour(x, y, n);
	//dijik(x,y);
	int nextx=x;
	int nexty=y;
	int cost = INT_MAX;

	if (character & TELE)
	{
		pn->npc->ifPCseen = 1;
		pn->npc->PCx = pc->x;
		pn->npc->PCy = pc->y;
	}
	else
	{
		if_in_room(pc);//this updates all the ifPCseen and coordinates in all the bots in the room with PC
	}
	if (character & BIT_SMART)
	{
		if (pn->npc->ifPCseen)
		{
			if (character & TUN)
			{
				djik(pn->npc->PCx, pn->npc->PCy, 1);

				for (int i = 0; i < n->size; i++)
				{
					if (difficulty_t[n->store[i][0]][n->store[i][1]] < cost)
					{
						nextx = n->store[i][0];
						nexty = n->store[i][1];
						cost = difficulty_t[n->store[i][0]][n->store[i][1]];
					}
				}


			}else
			{
				djik(pn->npc->PCx, pn->npc->PCy, 0);

				for (int i = 0; i < n->size; i++)
				{
					if (difficulty_t[n->store[i][0]][n->store[i][1]] < cost)
					{
						nextx = n->store[i][0];
						nexty = n->store[i][1];
						cost = difficulty[n->store[i][0]][n->store[i][1]];
					}
				}
			}
		}

	}
	else //dumb ones just move in a straight line towards pc location
	{
		if (pn->npc->ifPCseen)
		{
			if(x > pn->npc->PCx)
			{
				nextx = x - 1;
			}
			else if (x < pn->npc->PCx)
			{
				nextx = x + 1;
			}
			else if (y > pn->npc->PCy)
			{
				nexty = y - 1;
			}
			else if (y < pn->npc->PCy)
			{
				nexty = y + 1;
			}
			//else do nothing
		}
	}

	if (character & ERAT)
	{
		if (rand()& 0x1)
		{
			//printf(" --sorry randomiser activated--  ");
			int selector = rand() % (n->size);
			nextx = n->store[selector][0];
			nexty = n->store[selector][1];
		}
	}

	if (pn->npc->tangle)
	{
		for (int i = 0; i < n->size; i++)
		{
			if (grid_players[n->store[i][0]][n->store[i][1]] == NULL) continue;
			if ( (grid_players[n->store[i][0]][n->store[i][1]]->ifPC) == 1)
			{
				nextx = n->store[i][0];
				nexty = n->store[i][1];
			}
		}

	}

	//printf("%d, %d\n",nextx, nexty);

	//nextx and nexty will be the positions we will use to determine where the monster goes

	if(grid_players[nextx][nexty]==NULL)//no players to kill
	{
		if(character & TUN)
		{
			if(grid[nextx][nexty]==' ' && hardness[nextx][nexty] > 85 )
			{
				hardness[nextx][nexty] -= 85;
			}
			else
			{
				hardness[nextx][nexty] = 0;
				if(grid[nextx][nexty]==' ') grid[nextx][nexty] = '#';
				//player_node* temp = grid_players[x][y];
				grid_players[x][y] = NULL;
				pn->npc->x = nextx;
				pn->npc->y = nexty;
				grid_players[nextx][nexty] = pn;

			}
		}
		else
		{
			if(grid[nextx][nexty]!=' ')
			{
				//player_node* temp = grid_players[x][y];
				grid_players[x][y] = NULL;
				pn->npc->x = nextx;
				pn->npc->y = nexty;
				grid_players[nextx][nexty] = pn;
			}
		}
	}
	else if (nextx!=x || nexty!=y)//we kill someone
	{
		if (grid_players[nextx][nexty]->ifPC==1)//we kill PC
		{
			// *ifend = 2;
			// kill_player(grid_players[nextx][nexty], h);
			// //player_node* temp = grid_players[x][y];
			// grid_players[x][y] = NULL;
			// pn->npc->x = nextx;
			// pn->npc->y = nexty;
			// grid_players[nextx][nexty] = pn;


			readyPC();
			PC* p = grid_players[nextx][nexty]->pc;
			NPC* n = pn->npc;
			//engaging in a battle
			int damToPC = roll_dice(n->desc->dam);
			p->hp -= damToPC;
			int hpLeft;
			if (p->hp >= 0) hpLeft = p->hp;
			else hpLeft = 0;
			string msg = "PC got damaged with " + to_string(damToPC) ;//+ " points, " + p->hp + " hp left";
			msg += " points, by " + n->desc->name + " ---->  ";
			if (p->hp > 0)
			msg += to_string(p->hp) + " hp left.";
			else
			msg += " zero hp left.";
			attron(COLOR_PAIR(COLOR_RED));
			for (int i = 0; msg[i]!='.'; i++) mvaddch(attackLine, i, msg[i]);
			attron(COLOR_PAIR(COLOR_WHITE));
			attackLine++;
			if (p->hp < 0){
				*ifend = 2;
				kill_player(grid_players[nextx][nexty], h);
				grid_players[x][y] = NULL;
				n->x = nextx;
				n->y = nexty;
				grid_players[nextx][nexty] = pn;
			}
			else{
				//nothing, everyone stays where they are
			}


		}
		else//we kill non-pc
		{
			//kill_player(grid_players[nextx][nexty], h);
			player_node* temp = grid_players[nextx][nexty];
			grid_players[x][y] = temp;
			temp->npc->x = x;
			temp->npc->y = y;
			pn->npc->x = nextx;
			pn->npc->y = nexty;
			grid_players[nextx][nexty] = pn;
		}
	}

	pn->next_turn += (1000/(pn->npc->speed));
	free (n);
	return 0;
}
/*This methods identifies all the non-npc players in the room shared by PC, and updated the ifPCseen to PC's current location PCx and PCy for npc*/
int if_in_room(PC* pc)
{
	int upper = pc->y;
	while(upper+1 < ylenMax && grid[pc->x][upper+1]!=' ') upper++;
	int lower = pc->y;
	while(lower > 0 && grid[pc->x][lower-1]!=' ') lower--;
	int right = pc->x;
	while(right+1 < xlenMax && grid[right+1][pc->y]!=' ') right++;
	int left = pc->x;
	while(left > 0 && grid[left-1][pc->y]!=' ') left--;

	for(int i = lower; i <= upper; i++)
	{
		for(int j = left; j <= right; j++)
		{
			if (grid_players[j][i]!=NULL){
				if(!(grid_players[j][i]->ifPC==1)){
					grid_players[j][i]->npc->ifPCseen = 1;
					grid_players[j][i]->npc->PCx = pc->x;
					grid_players[j][i]->npc->PCy = pc->y;
				}
			}
		}
	}
	return 0;

}
/*This will help initialise the screen so we can have see the game, with contant printf's*/
void init_terminal()
{
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	start_color();
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);

	//black-black should be converted to white-black as well
	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);

}
/*This is simply use to display msgs like 'You lose' or 'You win' or 'You quit'.*/
int print_end(int success)
{
	for (int i = 0; i < ylenMax+1; i++){
		for (int j = 0; j< xlenMax; j++){
			mvaddch(i,j,' ');
		}
	}
	if (success==5)
	{
		string win = "YOU QUIT!!!!!!!!!!!!!.";
		for (int i = 0; win[i]!='.'; i++) mvaddch(10, 30+i, win[i]);
	}

	else if (success==1)
	{
		string win = "YOU WIN!!!!!!!!!!!!!.";
		for (int i = 0; win[i]!='.'; i++) mvaddch(10, 30+i, win[i]);

	}
	else {
		string lose = "YOU LOSE!!!!!!!!!!!!!.";
		for (int i = 0; lose[i]!='.'; i++) mvaddch(10, 30+i, lose[i]);
	}
	refresh();
	usleep(2000000);
	return 0;
}
/*This method was added to the nex_move method, where to determine next move, get key fetches a key and reacts accordingly*/
int getkey(int prevx,int prevy, int *x,int *y, int *endif, player_node_heap* h)
{
	start:
	int i;
	chtype ch = getch();

	/////////////////////////clearing the top msd board in case we need to give a msg after key is pressed//////////
	for (i = 0; i < xlenMax; i++) mvaddch(0,i,' ');

	/////////////////////
	*x = prevx;
	*y = prevy;
	//mvaddch(*y, *x,' ');
	if (ch=='8'||ch=='k') --*y;
	else if (ch=='5'||ch==' '|| ch=='.');
	else if (ch=='6'||ch=='l') ++*x;
	else if (ch=='4'||ch=='h')  --*x;
	else if (ch=='2'||ch=='j') ++*y;
	else if (ch=='1'||ch=='b') {++*y; --*x;}
	else if (ch=='3'||ch=='n') {++*y; ++*x;}
	else if (ch=='7'||ch=='y') {--*x; --*y;}
	else if (ch=='9'||ch=='u') {--*y; ++*x;}
	else if (ch=='q'||ch=='Q') {*endif = 5;}
	else if (ch=='m');
	else if (ch=='v')
	{
		player_node* current = (h->head);
		while (!(current->ifPC)) current=current->next;
		while(1)
		{
			for (i = 0; i < xlenMax; i++) mvaddch(0, i, ' ');
			string inst = "Press b to go back, UP/DOWN to change --> ";
			for (i = 0; i < radius; i++) inst += "+";
			inst += ".";
			refresh();
			for (i = 0; inst[i]!='.'; i++) mvaddch(0, i, inst[i]);
			ch = getch();
			if (ch == 259){if (radius <= 12) radius++;}//UP key 259 (and down is 258)
			else if (ch == 258){if (radius > 1) radius--;}//Down Key
			else if (ch == 'b'|| ch == 27) break;
		}
		for (i = 0; i < xlenMax; i++) mvaddch(0,i,' '); refresh();
		print_dungeon_limited(current->pc);
		goto start;
	}
	else if (ch=='g')
	{
		player_node* current = (h->head);
		while (!(current->ifPC)) current=current->next;
		teleport_controller(current->pc, *x, *y);
		/*dosomething();*/
		/*goto start;*/
	}
	else if (ch=='L')
	{
		player_node* current = (h->head);
		while (!(current->ifPC)) current=current->next;
		description_controller(current->pc, *x, *y);
		goto start;
	}
	else if (ch=='f')
	{
		visible = visible ? 0 : 1;
		player_node* current = (h->head);
		while (!(current->ifPC)) current=current->next;
		print_dungeon_limited(current->pc);
		goto start;
	}
	else if (ch=='<')
	{

		if (grid[prevx][prevy]=='<')
		{
			if (!(bossFound)) *endif=15;
			else
			{
				string lose = "BOSS FOUND IN THIS LEVEL, CANNOT LEAVE TILL BOSS DEFEATED.";
				for (i = 0; lose[i]!='.'; i++) mvaddch(0, i, lose[i]);
				refresh();
				goto start;
			}
		}
	}
	else if (ch=='>')
	{
		if (grid[prevx][prevy]=='>')
		{
			if (!(bossFound)) *endif=15;
			else
			{
				string lose = "BOSS FOUND IN THIS LEVEL, CANNOT LEAVE TILL BOSS DEFEATED.";
				for (i = 0; lose[i]!='.'; i++) mvaddch(0, i, lose[i]);
				refresh();
				goto start;
			}
		}
	}
	else if (ch=='w'||ch=='t'||ch=='d'||ch=='x'||ch=='i'||ch=='e'||ch=='I')
	{
		if 	( (ch == 'w') || (ch == 'd')|| (ch == 'x')|| (ch == 'i')|| (ch == 'I') )		selectInventory();
		else if ( (ch == 'e') || (ch == 't') )		viewEquipment();//weaponInventory();

		player_node* current = (h->head);
		while (!(current->ifPC)) current=current->next;
		print_dungeon_limited(current->pc);

		for (i = 0; i < xlenMax; i++) mvaddch(0,i,' ');
		goto start;
	}
	// else if (ch=='e')
	// {
	// 	viewEquipment();
	// 	player_node* current = (h->head);
	// 	while (!(current->ifPC)) current=current->next;
	// 	print_dungeon_limited(current->pc);
	// 	for (i = 0; i < xlenMax; i++) mvaddch(0,i,' ');
	// 	goto start;
	// }

	//else if (ch=='q') endwin();
	//game(y, x);

	//none of the recongnised keys were selected
	else
	{
		string lose = "THE KEY WAS UNRECOGNISED, TRY AGAIN!.";
		for (i = 0; lose[i]!='.'; i++) mvaddch(0, i, lose[i]);
		refresh();
		goto start;
		//getkey(prevx, prevy, x,y, endif,h);
	}
	//boundary check - pc will get an error if it hits the boundary and and an opportunity to put in another key////
	if (*x < 0 || *x >= xlenMax || *y < 0 || *y >= ylenMax)//boundary check
	{
		*x = prevx;
		*y = prevy;
		string lose = "YOU HAVE HIT THE EDGE, YOU CANNOT GO ANY FURTHER.";
		for (i = 0; lose[i]!='.'; i++) mvaddch(0, i, lose[i]);
		refresh();

		goto start;
		//getkey(prevx, prevy, x,y, endif,h);
	}
	//if we come across a wall and since the player cannot tunnel, we ask for another key///
	if(grid[*x][*y]==' ')
	{
		*x = prevx;
		*y = prevy;
		string lose = "SORRY YOU CAN'T TUNNEL YOUR WAY THROUGH. YOU NEED TO FIND AN EXISTING TUNNEL!!.";
		for (i = 0; lose[i]!='.'; i++) mvaddch(0, i, lose[i]);
		refresh();

		goto start;
		//getkey(prevx, prevy, x,y, endif,h);
	}
	if (ch=='m')
	{
		getmonsterlist(h);
		//getkey(prevx, prevy, x,y, endif,h);
		goto start;
	}

	return 0;

}
/*This method allows for the screen to show the monsters with their names/characters and their position relative to the pc*/
int getmonsterlist(player_node_heap* h)
{
	player_node* current = (h->head);
	player_node* print_node;
	int left_offset = 3;
	int top_offset = 16;

	PC* pc;
	while (!(current->ifPC)) current=current->next;
	pc = current->pc;


	current = (h->head);
	while(1)
	{
		for (int j = left_offset-1; j < ylenMax-left_offset+1; j++)
		{
			for(int k = top_offset-1; k <xlenMax-top_offset; k++)
			mvaddch(j, k, ' ');
		}
		refresh();
		//usleep(2000000);

		int count = 0;
		print_node = current;

		string player_info_2 = "=============================================.";
		int cursor2;
		for ( cursor2 = 0; player_info_2[cursor2]!='.'; cursor2++) mvaddch(left_offset+count, top_offset+cursor2, player_info_2[cursor2]);
		count++;
		player_info_2 = "NPC LIST WITH CHARACTERS AND DISTANCE.";
		for ( cursor2 = 0; player_info_2[cursor2]!='.'; cursor2++) mvaddch(left_offset+count, top_offset+cursor2, player_info_2[cursor2]);
		count++;
		player_info_2 = "=============================================.";
		for ( cursor2 = 0; player_info_2[cursor2]!='.'; cursor2++) mvaddch(left_offset+count, top_offset+cursor2, player_info_2[cursor2]);
		count++;


		int counter2 = 0;//this stores the num of monsters on display

		while(print_node!= NULL && counter2 < 10)
		{
			if (!(print_node->ifPC))
			{
				int cursor;
				string player_info = (print_node->npc->desc->name.substr(0,18)).c_str();//"PLAYER  .";
				for (cursor = 0; player_info[cursor]!='\0'; cursor++) mvaddch(left_offset+count, top_offset+cursor, player_info[cursor]);
				cursor = 20;
				// if (print_node->npc->character<10)
				// mvaddch(left_offset+count, 17+cursor, print_node->npc->character + '0');
				// else mvaddch(left_offset+count, 17+cursor, print_node->npc->character + 'a'-10);
				//attron(COLOR_PAIR(COLOR_WHITE));
				attron(COLOR_PAIR(print_node->npc->desc->color[0]));
				mvaddch(left_offset+count, 17+cursor, print_node->npc->desc->symb);
				attron(COLOR_PAIR(COLOR_WHITE));
				cursor+=5;

				if (pc->y > print_node->npc->y)
				{
					if ((pc->y - print_node->npc->y) < 10)
					{
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' + pc->y - print_node->npc->y));
						cursor++;
					}else{
						mvaddch(left_offset+count, 17+cursor, ('0' + ((pc->y - print_node->npc->y)/10)));
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' + ((pc->y - print_node->npc->y)%10)));
						cursor++;
					}
					mvaddch(left_offset+count, 17+cursor, 'N');
					cursor++;
				}
				else if (pc->y < print_node->npc->y)
				{
					if ((-pc->y + print_node->npc->y) < 10)
					{
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' - pc->y + print_node->npc->y));
						cursor++;
					}else{
						mvaddch(left_offset+count, 17+cursor, ('0' + ((-pc->y + print_node->npc->y)/10)));
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' + ((-pc->y + print_node->npc->y)%10)));
						cursor++;
					}
					mvaddch(left_offset+count, 17+cursor, 'S');
					cursor++;
				}
				else cursor+=3;

				cursor+=4;

				if (pc->x > print_node->npc->x)
				{
					if ((pc->x - print_node->npc->x) < 10)
					{
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' + pc->x - print_node->npc->x));
						cursor++;
					}else{
						mvaddch(left_offset+count, 17+cursor, ('0' + ((pc->x - print_node->npc->x)/10)));
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' + ((pc->x - print_node->npc->x)%10)));
						cursor++;
					}
					mvaddch(left_offset+count, 17+cursor, 'W');
					cursor++;
				}
				else if (pc->x < print_node->npc->x)
				{
					if ((-pc->x + print_node->npc->x) < 10)
					{
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' - pc->x + print_node->npc->x));
						cursor++;
					}else{
						mvaddch(left_offset+count, 17+cursor, ('0' + ((-pc->x + print_node->npc->x)/10)));
						cursor++;
						mvaddch(left_offset+count, 17+cursor, ('0' + ((-pc->x + print_node->npc->x)%10)));
						cursor++;
					}
					mvaddch(left_offset+count, 17+cursor, 'E');
					cursor++;
				}
				else cursor+=3;

				cursor++;
				if (print_node->npc->character & BOSS)
				{
					string boss = "--> BOSS.";
					for (int i = 0; boss[i]!='.'; i++)
					{
						mvaddch(left_offset+count, 17+cursor, boss[i]);
						cursor++;
					}
				}


				count++;
				counter2++;
				print_node = print_node->next;

			}
			else print_node = print_node->next;
		}

		player_info_2 = "=============================================.";
		for ( cursor2 = 0; player_info_2[cursor2]!='.'; cursor2++) mvaddch(left_offset+count, top_offset+cursor2, player_info_2[cursor2]);

		refresh();
		int key = getch();
		if (key==27) {print_dungeon_limited(pc); break;}//if ESC pressed
		else if (key == 259) //if UP key is pressed
		{
			if (current != NULL && current->prev) current = current->prev;
		}
		else if (key == 258)//if DOWN key is pressed
		{
			if (current != NULL && current->next) current = current->next;
		}

	}
	return 0;
}
/*This should ensure a completely random generated dungeon if the player takes the stairs
*/
int random_generator(player_node_heap** h, PC** pc, int nummon, room** rooms)
{
	int i,j,x,y;

	for (i = 0; i < xlenMax; i++)
	{
		for (j = 0; j < ylenMax; j++)
		{
			grid[i][j] = ' ';
			grid_players[i][j]=NULL;
			remembered[i][j] = ' ';
			grid_objects[i][j]=NULL;
		}
	}

	//we will start out by creating a seed with time-0 to access some randomeness
	srand(time(0));

	//populating the hardness randomly
	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++)
		{
			hardness[j][i] = 1 + (rand() % 254);
		}
	}

	int numRooms = MIN_ROOMS + (rand() % (MAX_ROOMS - MIN_ROOMS + 1));

	*rooms = (room*) malloc(numRooms * sizeof(room));

	int resizer = not_so_rand_roomsize_resizer(numRooms);//we use this function to obtain a denominator to limit the size of the rooms

	//the if conditions used to obtain the max length of the room help avoid the floating point exception (core dump) later when we use it with modulus later
	int maxRoomxlen = xlenMax / resizer;
	if (maxRoomxlen <= minRoomxlen) maxRoomxlen = minRoomxlen + 1;


	int maxRoomylen = ylenMax / resizer;
	if (maxRoomylen <= minRoomylen) maxRoomylen = minRoomylen + 1;

	//printf("num Rooms = %d\n", numRooms); //uncomment to see num of rooms generated

	//this loop keeps going till random coordinates and lengths are obtained from random function that make sense
	while (1)
	{
		for (i = 0; i < numRooms; i++)
		{
			(*rooms)[i].xloc = rand() % xlenMax;
			(*rooms)[i].yloc = rand() % ylenMax;
			(*rooms)[i].xlen = minRoomxlen + rand() % ((maxRoomxlen) - minRoomxlen);
			(*rooms)[i].ylen = minRoomylen + rand() % ((maxRoomylen) - minRoomylen);
		}
		if (makes_sense((*rooms), numRooms)) break;
	}


	//Next we populate the grid with '.' as per the randomised coordinates that made sense that we obtained earlier
	for (x = 0; x < numRooms; x++)
	{

		for (i = (*rooms)[x].xloc; i < ((*rooms)[x].xloc + (*rooms)[x].xlen); i++)
		{
			for (j = (*rooms)[x].yloc; j < ((*rooms)[x].yloc + (*rooms)[x].ylen); j++)
			{
				grid[i][j] = '.';
				hardness[i][j] = 0;
			}
		}
	}

	//next we carve out a path between adjacent rooms in which we use the former's x coordinate and latter's y-coordinates to create a mid-point
	for (int x = 0; x < numRooms - 1; x++)
	{
		int middlex = (*rooms)[x].xloc;
		int middley = (*rooms)[x + 1].yloc;
		int i;//i will save the direction of the path

		if ((*rooms)[x].yloc > middley) i = 1;
		else i = -1;

		//first we go from from midpoint to former room
		for ( j = middley; j != (*rooms)[x].yloc; j += i)
		{
			if (grid[middlex][j] != '.')
			{
				grid[middlex][j] = '#';
				hardness[middlex][j] = 0;
			}
		}

		//then we go from midpoint to latter room
		if ((*rooms)[x + 1].xloc > middlex) i = 1;
		else i = -1;

		for ( j = middlex; j != (*rooms)[x + 1].xloc; j += i)
		{
			if (grid[j][middley] != '.')
			{
				grid[j][middley] = '#';
				hardness[j][middley] = 0;
			}
		}

	}


	//here we randomise the upwards and downward staircases and insert them wherever the random coordinates and its horizontal neighbours are part of room
	//int numUpstairs, numDownstairs;
	for (i = 0; i < 2; i++)
	{
		//first iteration adds random number of '<' to the grid, second adds '<'
		char staircase;
		if (i == 0) staircase = '<';
		else staircase = '>';

		int numStairs = minStairs + rand() % ((maxStairs) - minStairs);
		if (i == 0)
		{
			staircase = '<';
			//numUpstairs = numStairs;

		}
		else
		{
			staircase = '>';
			//numDownstairs = numStairs;
		}

		for (j = 0; j < numStairs; j++)
		{
			//while loops below keeps going till a successfuk coordinate is found
			while (1)
			{
				x = 1 + (rand() % (xlenMax - 2));//this ensures that we're not on the left or the right edge because the condition below checks horizontal neighbours
				y = (rand() % (ylenMax));

				if (grid[x][y] == '.' && grid[x - 1][y] == '.' && grid[x + 1][y] == '.')
				{
					grid[x][y] = staircase;
					break;
				}
			}


		}
	}
	monster_used.clear();
	object_used.clear();
	for (i = 0; i < monsters.size(); i++) monster_used.push_back(0);
	for (i = 0; i < odesc.size(); i++) object_used.push_back(0);

	//PC* pc;
	initialize_pc(pc);//this is where pc gets initialised
	//printf("\nPC has been initialised; the coordinates accessible from main are %d, %d\n", pc->x, pc->y);
	initialize_players(nummon, *pc);//this initialises all the bots
	//player_node_heap* h;
	populate_heap(h);//all monsters and pc get loaded on a heap

	populate_dungeon_objects();
	print_dungeon();


	return 0;
}
/*This is used to read into the monster_desc.txt and initialize the monsters,
they will be pushed to the monsters vector*/
int parseMonstersDesc()
{
	string home = getenv("HOME");
	string gamedir = ".rlg327";
	string monst_desc_file = "monster_desc.txt";
	string path = home + "/"+gamedir+"/"+monst_desc_file;
	ifstream f(path);

	string t,x;

        while(getline(f, x))
        {
                if (x.compare("BEGIN MONSTER")==0)
                {
			monster_desc m;
			m.desc = "";

                        //cout<<"\n\n---------Preparing to load the Monster---------\n\n";
                        while (1)
                        {
                                getline(f, x);

				if (x.compare("END")==0) break;

				stringstream stream(x);
				//if (x.compare("\r")!=0)
				//{
					while(1)
					{
					stream>>t;


					if (t.compare("SPEED")==0)
					{
						//cout<<"speed detected\n";
						stream>>t;
						//int ar[3];
						finddice(m.speed, t);
						//cout<<m.speed[0]<<"+"<< m.speed[1]<<"d"<<m.speed[2] <<endl;

					}
					else if (t.compare("DAM")==0)
					{
						//cout<<"damage detected\n";
						stream>>t;
						finddice(m.dam, t);
						//cout<<m.dam[0]<<"+"<< m.dam[1]<<"d"<<m.dam[2] <<endl;

					}
					else if (t.compare("HP")==0)
					{
						//cout<<"hitpoints detected\n";
						stream>>t;
						finddice(m.hp, t);
						//cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
					}
					else if (t.compare("RRTY")==0)
					{
						//cout<<"rarity detected\n";
						stream>>t;
						m.rrty = stoi(t);
						//cout<<m.rrty<<endl;
					}
					else if (t.compare("SYMB")==0)
					{
						//cout<<"symbol detected\n";
						stream>>t;
						m.symb = t[0];
						//cout<<m.symb<<endl;
					}
					else if (t.compare("COLOR")==0)
					{
						//cout<<"color detected\n";
						//stream>>t;
						while (!stream.eof())
						{
							stream>>t;
							m.color.push_back(getColorint(t));
						}

					}
					else if (t.compare("ABIL")==0)
					{
						//cout<<"abilities detected\n";
						m.abil = 0;

						while (!stream.eof())
						{
							stream>>t;
							m.abil = m.abil | get_abilities(t);
						}

					}
					else if (t.compare("NAME")==0)
					{
						//cout<<"name detected\n";
						stream>>t;
						m.name.append(t);
						while (!stream.eof())
						{
							stream>>t;
							m.name.append(" ");
							m.name.append(t);
							//stream>>t;
						}
						//cout<<m.color<<endl;
					}
					else if (t.compare("DESC")==0)
					{
						//cout<<"description detected\n";
						getline(f, x);

						while(x.compare(".")!=0)
						{
							//x = x.substr(0, x.length()-1);
							if (x.length() > 77) x = x.substr(0, 77);
							m.desc.append(x);
							m.desc.append("\n");
							getline(f, x);
						}
						//cout<<m.desc;
					}
					else break;
					//cout<<t<<endl;
					if (stream.eof()) break;
					}
				//}
                                //cout<<x<<endl;
                        }
                        //cout<<"\n\n------------Ending the load-------\n\n";
			monsters.push_back(m);
                }
        }
	f.close();
	return 0;
}
/*This converts the string into 3 int set*/
int finddice(int* ar, string s)
{
	int i,j,k;
	i = 0;
	j = 0;
	int negative_multipe = 1;;

	if(s[0]=='-')
	{
		negative_multipe = -1;
		s = s.substr(1, s.size()-1);
	}

	string temp;

	for (k = 0; k < 3; k++)
	{
	while (s[i]<='9' && s[i]>='0') i++;
	temp = s.substr( j, i-j);
	j = stoi(temp);
	ar[k] = j;
	j = ++i;
	}
	ar[0] *= negative_multipe;
	return 0;



}
/*This is used to get the string color depending on the int as per the ncurses library*/
string getColorString(int i)
{
	if (i == 0) return "BLACK";
	else if (i == 1) return "RED";
	else if (i == 2) return "GREEN";
	else if (i == 3) return "YELLOW";
	else if (i == 4) return "BLUE";
	else if (i == 5) return "MAGENTA";
	else if (i == 6) return "CYAN";
	else return "WHITE";
}
/*This is used to the exact int of the color depending on the string as per the ncurses library*/
int getColorint(string t)
{
	int col;
	if (t.compare("BLACK")==0) col = 0;
	else if (t.compare("RED")==0) col = 1;
	else if (t.compare("GREEN")==0) col = 2;
	else if (t.compare("YELLOW")==0) col = 3;
	else if (t.compare("BLUE")==0) col = 4;
	else if (t.compare("MAGENTA")==0) col = 5;
	else if (t.compare("CYAN")==0) col = 6;
	else if (t.compare("WHITE")==0) col = 7;
	else col = -1;
	return col;
}
/*This should get us the bit from the bitvector of the abilities
and determine which ability it corresponds to and converts those to string*/
int get_abilities(string abil)
{
	if (abil.compare("SMART")==0) return 0x1;
	else if (abil.compare("TELE")==0) return 0x2;
	else if (abil.compare("TUNNEL")==0) return 0x4;
	else if (abil.compare("ERRATIC")==0) return 0x8;
	else if (abil.compare("PICKUP")==0) return 0x10;
	else if (abil.compare("DESTROY")==0) return 0x20;
	else if (abil.compare("UNIQ")==0) return 0x40;
	else if (abil.compare("BOSS")==0) return 0x80;
	else if (abil.compare("PASS")==0) return 0x100;
	else return 0;
}
/*This takes the ability bit-vector and tells all the abilities in string by conversion - via bit-wise-operations*/
string intToAbil(int abil)
{
	string abilities = "";
	if (abil & 0x1) abilities.append("SMART ");
	if (abil & 0x2) abilities.append("TELE ");
	if (abil & 0x4) abilities.append("TUNNEL ");
	if (abil & 0x8) abilities.append("ERRATIC ");
	if (abil & 0x10) abilities.append("PICKUP ");
	if (abil & 0x20) abilities.append("DESTROY ");
	if (abil & 0x40) abilities.append("UNIQ ");
	if (abil & 0x80) abilities.append("BOSS ");
	if (abil & 0x100) abilities.append("PASS ");

	return abilities;
}
/*This method prints all the description of the monsters in the monsters vector*/
int print_monster_desc()
{
	monster_desc m;
	for (int count = 0; count < (int)monsters.size(); count++)
	{
		m = monsters[count];
		//cout<<"Printing monster "<<count<<endl;
		cout<<m.name<<endl;
		cout<<m.desc;
		cout<<m.symb<<endl;
		for (int k = 0; k < (int)m.color.size(); k++)
		{
			cout<<getColorString(m.color[k])<<" ";
		}
		cout<<endl;
		cout<<m.speed[0]<<"+"<< m.speed[1]<<"d"<<m.speed[2] <<endl;
		cout<<intToAbil(m.abil)<<endl;
		cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
		cout<<m.dam[0]<<"+"<< m.dam[1]<<"d"<<m.dam[2] <<endl;
		cout<<m.rrty<<endl;
		cout<<endl;
		//cout<<"ending print monster "<<count<<"\n\n";
	}
	return 0;
}
/*This will be used to parse object descriptions from the text file into obj_desc instamces
and into the odescs vector*/

int parseObjDesc()
{
	string home = getenv("HOME");
	string gamedir = ".rlg327";
	string monst_desc_file = "object_desc.txt";
	string path = home + "/"+gamedir+"/"+monst_desc_file;
	ifstream f(path);
	//cout<<"file found\n";
	string t,x;

        while(getline(f, x))
        {
		//cout<<x<<endl;
                if (x.compare("BEGIN OBJECT")==0)
                {
			obj_desc m;
			m.desc = "";

                        //cout<<"\n\n---------Preparing to load the Monster---------\n\n";
                        while (1)
                        {
                                getline(f, x);

				if (x.compare("END")==0) break;

				stringstream stream(x);
				//if (x.compare("\r")!=0)
				//{
					while(1)
					{
					stream>>t;


					if (t.compare("SPEED")==0)
					{
						//cout<<"speed detected\n";
						stream>>t;
						//int ar[3];
						finddice(m.speed, t);
						//cout<<m.speed[0]<<"+"<< m.speed[1]<<"d"<<m.speed[2] <<endl;

					}
					else if (t.compare("DAM")==0)
					{
						//cout<<"damage detected\n";
						stream>>t;
						finddice(m.dam, t);
						//cout<<m.dam[0]<<"+"<< m.dam[1]<<"d"<<m.dam[2] <<endl;

					}
					else if (t.compare("HIT")==0)
					{
						//cout<<"hitpoints detected\n";
						stream>>t;
						finddice(m.hit, t);
						//cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
					}
					else if (t.compare("DODGE")==0)
					{
						//cout<<"hitpoints detected\n";
						stream>>t;
						finddice(m.dodge, t);
						//cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
					}
					else if (t.compare("DEF")==0)
					{
						//cout<<"hitpoints detected\n";
						stream>>t;
						finddice(m.def, t);
						//cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
					}
					else if (t.compare("WEIGHT")==0)
					{
						//cout<<"hitpoints detected\n";
						stream>>t;
						finddice(m.weight, t);
						//cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
					}
					else if (t.compare("ATTR")==0)
					{
						//cout<<"hitpoints detected\n";
						stream>>t;
						finddice(m.attr, t);
						//cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
					}
					else if (t.compare("VAL")==0)
					{
						//cout<<"hitpoints detected\n";
						stream>>t;
						finddice(m.val, t);
						//cout<<m.hp[0]<<"+"<< m.hp[1]<<"d"<<m.hp[2] <<endl;
					}
					else if (t.compare("RRTY")==0)
					{
						//cout<<"rarity detected\n";
						stream>>t;
						m.rrty = stoi(t);
						//cout<<m.rrty<<endl;
					}
					else if (t.compare("ART")==0)
					{
						//cout<<"rarity detected\n";
						stream>>t;
						if (t=="TRUE") m.art = 1;
						else m.art = 0;
						//cout<<m.rrty<<endl;
					}

					else if (t.compare("COLOR")==0)
					{
						//cout<<"color detected\n";
						//stream>>t;
						while (!stream.eof())
						{
							stream>>t;
							m.color.push_back(getColorint(t));
						}

					}
					else if (t.compare("TYPE")==0)
					{
						//cout<<"abilities detected\n";
						m.type = 0;

						while (!stream.eof())
						{
							stream>>t;
							m.type += get_obj_type(t);
						}

					}
					else if (t.compare("NAME")==0)
					{
						//cout<<"name detected\n";
						stream>>t;
						m.name.append(t);
						while (!stream.eof())
						{
							stream>>t;
							m.name.append(" ");
							m.name.append(t);
							//stream>>t;
						}
						//cout<<m.color<<endl;
					}
					else if (t.compare("DESC")==0)
					{
						//cout<<"description detected\n";
						getline(f, x);

						while(x.compare(".")!=0)
						{
							//x = x.substr(0, x.length()-1);
							if (x.length() > 77) x = x.substr(0, 77);
							m.desc.append(x);
							m.desc.append("\n");
							getline(f, x);
						}
						//cout<<m.desc;
					}
					else break;
					//cout<<t<<endl;
					if (stream.eof()) break;
					}
				//}
                                //cout<<x<<endl;
                        }
                        //cout<<"\n\n------------Ending the load-------\n\n";
			odesc.push_back(m);
                }
        }
	f.close();
	return 0;
}

/*This should get us the bit from the bitvector of the abilities
and determine which ability it corresponds to and converts those to string*/
int get_obj_type(string abil)
{
	if (abil.compare("WEAPON")==0) return WEAPON;
	else if (abil.compare("OFFHAND")==0) return OFFHAND;
	else if (abil.compare("RANGED")==0) return RANGED;
	else if (abil.compare("ARMOR")==0) return ARMOR;
	else if (abil.compare("HELMET")==0) return HELMET;
	else if (abil.compare("CLOAK")==0) return CLOAK;
	else if (abil.compare("GLOVES")==0) return GLOVES;
	else if (abil.compare("BOOTS")==0) return BOOTS;
	else if (abil.compare("RING")==0) return RING;
	else if (abil.compare("AMULET")==0) return AMULET;
	else if (abil.compare("LIGHT")==0) return LIGHT;
	else if (abil.compare("SCROLL")==0) return SCROLL;
	else if (abil.compare("BOOK")==0) return BOOK;
	else if (abil.compare("FLASK")==0) return FLASK;
	else if (abil.compare("GOLD")==0) return GOLD;
	else if (abil.compare("AMMUNITION")==0) return AMMUNITION;
	else if (abil.compare("FOOD")==0) return FOOD;
	else if (abil.compare("WAND")==0) return WAND;
	else if (abil.compare("CONTAINER")==0) return CONTAINER;
	else return 0;

}
/*This takes the ability bit-vector and tells all the abilities in string by conversion - via bit-wise-operations*/
string int_to_objtype(int abil)
{
	string abilities = "";
	if (abil & WEAPON) abilities.append("WEAPON ");
	if (abil & OFFHAND) abilities.append("OFFHAND ");
	if (abil & RANGED) abilities.append("RANGED ");
	if (abil & ARMOR) abilities.append("ARMOR ");
	if (abil & HELMET) abilities.append("HELMET ");
	if (abil & CLOAK) abilities.append("CLOAK ");
	if (abil & GLOVES) abilities.append("GLOVES ");
	if (abil & BOOTS) abilities.append("BOOTS ");
	if (abil & RING) abilities.append("RING ");
	if (abil & AMULET) abilities.append("AMULET ");
	if (abil & LIGHT) abilities.append("LIGHT ");
	if (abil & SCROLL) abilities.append("SCROLL ");
	if (abil & BOOK) abilities.append("BOOK ");
	if (abil & FLASK) abilities.append("FLASK ");
	if (abil & GOLD) abilities.append("GOLD ");
	if (abil & AMMUNITION) abilities.append("AMMUNITION ");
	if (abil & FOOD) abilities.append("FOOD ");
	if (abil & WAND) abilities.append("WAND ");
	if (abil & CONTAINER) abilities.append("CONTAINER ");

	return abilities;

}
/*This method prints all the description of the object_desc in the odescs vector*/
int print_obj_desc()
{
	obj_desc m;
	for (int count = 0; count < (int)odesc.size(); count++)
	{
		m = odesc[count];
		//cout<<"Printing monster "<<count<<endl;
		cout<<m.name<<endl;
		cout<<m.desc;
		for (int k = 0; k < (int)m.color.size(); k++)
		{
			cout<<getColorString(m.color[k])<<" ";
		}
		cout<<endl;
		cout<<m.speed[0]<<"+"<< m.speed[1]<<"d"<<m.speed[2] <<endl;
		cout<<int_to_objtype(m.type)<<endl;
		cout<<m.hit[0]<<"+"<< m.hit[1]<<"d"<<m.hit[2] <<endl;
		cout<<m.dam[0]<<"+"<< m.dam[1]<<"d"<<m.dam[2] <<endl;
		cout<<m.rrty<<endl;
		cout<<endl;
		//cout<<"ending print monster "<<count<<"\n\n";
	}
	return 0;
}
/*This outputs an int from an array of 3 ints.*/
int roll_dice(int* d){
	int base = d[0];
	int number = d[1];
	int sides = d[2];

	int total = base;

        if (sides) {
          for (int i = 0; i < number; i++) {
            total += 1+ (rand()%sides);
          }
        }

        return total;
}
/*Finds an index of monster in vector of monster description as per the specs*/
int findMonster(){
	int i, rare_val;

	//cout<<"finding monsters\n";

	find_monster:

	i = rand()%(monsters.size());
	rare_val = rand()%100;

	if (monster_used[i]) goto find_monster;

	if (monsters[i].abil & UNIQ) //if monster is unique we go in
	{
		//cout<<"monster is unique\n";
		if (monster_death[i]) //check to see if uniq monster has died
		{
			goto find_monster;
		}
		else			//uniq monster hasn't died, we need to check if its rare
		{
			if (monsters[i].rrty > rare_val) {
				//cout<<"getting mons "<<monsters[i].name<<endl;
				monster_used[i] = 1;
				return i;
			}
			else goto find_monster;
		}
	}
	else		//if monster isn't unique
	{
		//cout<<"monster not unique\n";
		//cout<<"rare_val: "<< rare_val <<endl;
		//cout<<"monster rarity: "<< monsters[i].rrty  <<endl;
		if (monsters[i].rrty > rare_val) {
			//cout<<"getting mons "<<monsters[i].name<<endl;
			monster_used[i] = 1;
			return i;
		}
		else goto find_monster;
	}

}
/*Finds an index of obj in vector of object description as per the specs*/
int findObj(){
	int i, rare_val;

	//cout<<"finding monsters\n";

	find_obj:

	i = rand()%(odesc.size());
	rare_val = rand()%100;

	if (object_used[i]) goto find_obj;

	//cout<<"monster is unique\n";
	if (object_death[i]) //check to see if object has died
	{
		goto find_obj;
	}
	else			//uniq monster hasn't died, we need to check if its rare
	{
		if (odesc[i].rrty > rare_val) {
			//cout<<"getting mons "<<monsters[i].name<<endl;
			object_used[i] = 1;
			return i;
		}
		else goto find_obj;
	}

}
/*This empties the dungeon of any objects and finds spot with room and no existing object and no pc to place the object*/
int populate_dungeon_objects()
{
	int i,j,k;
	int minimum;

	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++){
			if (grid_objects[j][i] != NULL){
				free(grid_objects[j][i]);
				grid_objects[j][i] = NULL;
			}
		}
	}
	//cout<<"got here\n";
	//number of objects

	i = 0;
	for (j = 0; j < object_death.size(); j++){
		if (!(object_death[j])) i++;
	}

	if (i < minObjects) minimum = i;
	else minimum = minObjects;
	//cout<<"got here\n";
	//find out pc
	PC* pc;

	for (i = 0; i < ylenMax; i++)
	{
		for (j = 0; j < xlenMax; j++){
			if (grid_players[j][i]!=NULL)
			{
				if (grid_players[j][i]->ifPC){
					pc = grid_players[j][i]->pc;
					break;
				}
			}
		}
	}
	//cout<<"got here\n";

	//we populate the dungeon where there is room and no pc
	//cout<< "PC pos: "<<pc->x<<"  "<<pc->y<<endl;
	//cout<< "minimum: "<<minimum<<endl;
	for (int k = 0; k < minimum; k++){
		int end = 1;
		while(end)
		{
			i = rand()%xlenMax;
			j = rand()%ylenMax;
			//cout<<i<<"  "<<j<<endl;
			if ( (i!=pc->x) || (j!=pc->y) ) {
				//cout<<"not same position as pc"<<endl;
				if (grid_objects[i][j]==NULL) {

					 if (grid[i][j]=='.') {
						 end = 0;
						 //if (k > minimum-2)cout<<"found pos with room "<<k<<"out of "<<minimum<<endl;
					 }
				 }
			}

		}
		//this is where we place the object
		//cout<<"got to k: "<<k<<endl;
		obj_node* o = (obj_node*) malloc(sizeof(obj_node));
		o->index = findObj();
		o->d = &odesc[o->index];

		grid_objects[i][j] = o;

	}
	//cout<<"got here\n";
	return 0;
}
/*Takes a type int of objects and returns the respective character (if no match, it gets '*')*/
char getObjChar(int type){


	if (type & WEAPON) 	return '|';
	if (type & OFFHAND) 	return ')';
	if (type & RANGED) 	return '}';
	if (type & ARMOR) 	return '[';
	if (type & HELMET) 	return ']';
	if (type & CLOAK) 	return '(';
	if (type & GLOVES) 	return '{';
	if (type & BOOTS) 	return '\'';
	if (type & RING) 	return '=';
	if (type & AMULET) 	return '"';
	if (type & LIGHT) 	return '_';
	if (type & SCROLL) 	return '~';
	if (type & BOOK) 	return '?';
	if (type & FLASK) 	return '!';
	if (type & GOLD) 	return '$';
	if (type & AMMUNITION) 	return '/';
	if (type & FOOD) 	return ',';
	if (type & WAND) 	return '-';
	if (type & CONTAINER) 	return '%';
	else 			return '*';

}

/*This method (activated with L) would allow the user to navigate the dungeon with numpad and will look like
an esterick and press t when a monster is selected to see its description*/

int description_controller(PC* pc, int& x, int& y)
{
	start:
	print_teleport(pc, &x, &y);
	chtype ch = getch();

	if (ch == '8'||ch == 'k')
	{
		if (y!=0) y--;
		goto start;
	}
	else if (ch == '7'||ch == 'y')
	{
		if (y!=0) y--;
		if (x!=0) x--;
		goto start;
	}
	else if (ch == '9'||ch == 'u')
	{
		if (y!=0) y--;
		if (x!=(xlenMax-1)) x++;
		goto start;
	}
	else if (ch == '6'||ch == 'l')
	{
		if (x!=(xlenMax-1)) x++;
		goto start;
	}
	else if (ch == '1'||ch == 'b')
	{
		if (x!=0) x--;
		if (y!=(ylenMax-1)) y++;
		goto start;
	}
	else if (ch == '2'||ch == 'j')
	{
		if (y!=(ylenMax-1)) y++;
		goto start;
	}
	else if (ch == '3'||ch == 'n')
	{
		if (y!=(ylenMax-1)) y++;
		if (x!=(xlenMax-1)) x++;
		goto start;
	}
	else if (ch == '4'||ch == 'h')
	{
		if (x!=0) x--;
		goto start;
	}
	else if (ch == 't')
	{
		if (grid_players[x][y]!=NULL)
		{
			if (!(grid_players[x][y]->ifPC))
			{
				while(1)
				{
					string description = grid_players[x][y]->npc->desc->desc;
					for (int i = 0; i < ylenMax+1; i++)
					{
						for (int j = 0; j< xlenMax; j++)
						{
							mvaddch(i,j,' ');
						}
					}
					///////
					int i = 0;
					int j = 0;
					int k = 0;

					string d = "Press ESC to go back.";
					for (i = 0; d[i]!='.'; i++) mvaddch(k, i, d[i]);
					k+=2;

					d = "NAME   .";
					for (i = 0; d[i]!='.'; i++) mvaddch(k, i, d[i]);
					d = grid_players[x][y]->npc->desc->name;
					for (j = 0; j < d.size(); j++) mvaddch(k, i++, d[j]);

					k++;
					//HP, speed, DAM
					d = "HP   .";
					for (i = 0; d[i]!='.'; i++) mvaddch(k, i, d[i]);
					d = to_string(grid_players[x][y]->npc->hp);
					for (j = 0; j < d.size(); j++) mvaddch(k, i++, d[j]);

					k++;
					//HP, speed, DAM
					d = "SPEED   .";
					for (i = 0; d[i]!='.'; i++) mvaddch(k, i, d[i]);
					d = to_string(grid_players[x][y]->npc->speed);
					for (j = 0; j < d.size(); j++) mvaddch(k, i++, d[j]);
					k++;
					//HP, speed, DAM
					d = "DAMAGE   .";
					for (i = 0; d[i]!='.'; i++) mvaddch(k, i, d[i]);
					d = to_string(grid_players[x][y]->npc->desc->dam[0])+"+";
					d = d + to_string(grid_players[x][y]->npc->desc->dam[1]) + "d";
					d = d + to_string(grid_players[x][y]->npc->desc->dam[2]);
					for (j = 0; j < d.size(); j++) mvaddch(k, i++, d[j]);

					d = "DESCRIPTION.";
					for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(6, cursor, d[cursor]);

					i = 0;
					j = 0;
					k = 0;

					while (1)
					{
						if (description[k]=='\n')//(i >= 40)
						{
							j++;
							i = 0;
						}
						if (j==ylenMax-8) break;
						mvaddch(8+j, 0+i, description[k]);
						k++;
						i++;
						if (k==description.size()) break;
					}


					ch = getch();
					if (ch == 27) {
						for (j = 0; j< xlenMax; j++)
						{
							mvaddch(0,j,' ');
						}
						goto start;//leaves the screen of description
					}
				}
			}
		}

		goto start;
	}
	else if (ch == 27)//press ESC
	{
		for (int j = 0; j< xlenMax; j++)
		{
			mvaddch(0,j,' ');
		}
		print_dungeon_limited(pc);
		return 0;
	}
	else goto start;

}



/*View all the items that the PC holds in the inventory with this method*/
int weaponInventory()
{
	int i, j, line;
	start:
	line = 0;
	for (i = 0; i < ylenMax+1; i++)
	{
		for (j = 0; j< xlenMax; j++)
		{
			mvaddch(i,j,' ');
		}
	}
	string d = "Press ESC or 'b' to go back.";
	for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line, cursor, d[cursor]);
	line+=2;
	d = "WEAPONS LIST.";
	for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line, cursor, d[cursor]);
	line++;



	for (i = 0; i < inventory.size(); i++){
		int cursor;
		d = inventory[i]->d->name;
		mvaddch(line, 4, i+'0');
		for (cursor = 0; cursor < d.size(); cursor++) mvaddch(line, cursor+6, d[cursor]);
		cursor+=9;
		d = "TYPE:.";
		for (j = 0; d[j]!='.'; j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+1;
		d = int_to_objtype(inventory[i]->d->type);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+3;
		d = "DAM:.";
		for (j = 0; d[j]!='.'; j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+1;
		d = to_string(inventory[i]->d->dam[0])+"+";
		d = d + to_string(inventory[i]->d->dam[1]) + "d";
		d = d + to_string(inventory[i]->d->dam[2]);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+3;
		line++;
	}



	chtype ch = getch();
	if ((ch == 27) || (ch=='b'))
	{
		return 0;
	}
	else goto start;
}


/*View all the items that the PC is eqipped with this method*/
int viewEquipment()
{
	int i, j, line, k;
	k = 0;
	int warning = 0;
	start:
	int typeID = 1;
	line = 0;
	for (i = 0; i < ylenMax+1; i++)
	{
		for (j = 0; j< xlenMax; j++)
		{
			mvaddch(i,j,' ');
		}
	}
	string d = "Press ESC or 'b' to go back, 't' to take off equipment                        .";
	for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line, cursor, d[cursor]);
	if (warning == 2)
	{
		d = "There doesn't seem to be any space in your inventory                            .";
		for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line+1, cursor, d[cursor]);
		warning = 0;
	}
	else if (warning ==3)
	{
		d = "There isn't anything in this slot                                       .";
		for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line+1, cursor, d[cursor]);
		warning = 0;
	}
	line+=2;
	d = "EQUIPMENT.";
	for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line, cursor, d[cursor]);
	line++;

	for (i = 0; i < equipped.size(); i++)
	{
		if (i == k)
		{
			d = "-->.";
			for (j = 0; d[j]!='.'; j++) mvaddch(line, j, d[j]);
		}
		int cursor;
		mvaddch(line, 4, i+'a');
		cursor = 6;

		d= int_to_objtype(typeID);
		if (i==11) d = int_to_objtype(RING);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+=j+3;

		typeID*=2;
		if (equipped[i]==NULL)
		{
			line++;
			continue;
		}
		d = equipped[i]->d->name;
		for (j = 0; j < d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+=j+3;
		//USE THIS FOR SPEED
		if (cursor < 45) cursor = 45;

		d = "SPEED:.";
		for (j = 0; d[j]!='.'; j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+1;
		d = to_string(equipped[i]->d->speed[0])+"+";
		d = d + to_string(equipped[i]->d->speed[1]) + "d";
		d = d + to_string(equipped[i]->d->speed[2]);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+3;

		if (cursor < 55) cursor = 55;

		d = "DAM:.";
		for (j = 0; d[j]!='.'; j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+1;
		d = to_string(equipped[i]->d->dam[0])+"+";
		d = d + to_string(equipped[i]->d->dam[1]) + "d";
		d = d + to_string(equipped[i]->d->dam[2]);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+3;
		line++;
	}


	chtype ch = getch();
	if ((ch == 27) || (ch=='b'))
	{
		return 0;
	}
	else if (ch == 259) //if UP key
	{
		if (k > 0) k--;
		else {
			k--;
			k+=equipped.size();
		}
		goto start;
	}
	else if (ch == 258) //if DOWN key
	{
		if (k < equipped.size()-1) k++;
		else{
			k++;
			k-=equipped.size();
		}
		goto start;
	}
	else if (ch == 't') //if t is pressed to take off an item
	{
		if (inventory.size()==12)
		{
			warning = 2;
			goto start;
		}
		else if (equipped[k]==NULL)
		{
			warning = 3;
			goto start;
		}
		else
		{
			if (k==11){
				inventory.push_back(equipped[k]);
				equipped[k] = NULL;
				ring2 = 0;
				goto start;
			}
			else
			{
				inventory.push_back(equipped[k]);
				eq_vector -= equipped[k]->d->type;
				equipped[k] = NULL;
				goto start;
			}
		}
	}
	else if (ch == 'w')
	{
		return selectInventory();
	}
	else goto start;

}
/*SELECT AN ITEM FROM THE WEAPONS INVENTORY*/
int selectInventory()
{
	int warning = 0;
	int i, j, line, k;
	k = 0;
	start:
	line = 0;
	for (i = 0; i < ylenMax+1; i++)
	{
		for (j = 0; j< xlenMax; j++)
		{
			mvaddch(i,j,' ');
		}
	}
	string d = "Press ESC/b to esc, UP/DOWN to navigate, w to wear, d to drop, x to expunge.";
	for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line, cursor, d[cursor]);
	if (warning!=0)
	{
		string d = "CANNOT WEAR THIS WEAPON                                                        .";
		for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line, cursor, d[cursor]);
		warning = 0;
	}
	line+=2;
	d = "WEAPONS LIST.";
	for (int cursor = 0; d[cursor]!='.'; cursor++) mvaddch(line, cursor, d[cursor]);
	line++;


	for (i = 0; i < inventory.size(); i++){
		if (i == k)
		{
			d = "-->.";
			for (j = 0; d[j]!='.'; j++) mvaddch(line, j, d[j]);
		}
		int cursor;
		d = inventory[i]->d->name;
		mvaddch(line, 4, i+'0');
		for (cursor = 0; cursor < d.size(); cursor++) mvaddch(line, cursor+6, d[cursor]);
		cursor+=9;
		if (cursor < 30) cursor = 30;
		d = "TYPE:.";
		for (j = 0; d[j]!='.'; j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j;
		d = int_to_objtype(inventory[i]->d->type);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+2;
		if (cursor < 42) cursor = 42;
		d = "DAM:.";
		for (j = 0; d[j]!='.'; j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j;
		d = to_string(inventory[i]->d->dam[0])+"+";
		d = d + to_string(inventory[i]->d->dam[1]) + "d";
		d = d + to_string(inventory[i]->d->dam[2]);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+2;

		if (cursor < 52) cursor = 52;
		d = "SPEED:.";
		for (j = 0; d[j]!='.'; j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j;
		d = to_string(inventory[i]->d->speed[0])+"+";
		d = d + to_string(inventory[i]->d->speed[1]) + "d";
		d = d + to_string(inventory[i]->d->speed[2]);
		for (j = 0; j<d.size(); j++) mvaddch(line, cursor+j, d[j]);
		cursor+= j+2;
		line++;
	}
	if (inventory.size()>0)
	{
		d = "DESCRIPTION.";
		for (j = 0; d[j]!='.'; j++) mvaddch(14, j, d[j]);
		d = inventory[k]->d->desc;
		j = 0;
		i = 15;
		int cursor = 0;
		while (1)
		{
			if (d[cursor]=='\n')//(i >= 40)
			{
				i++;
				j = 0;
			}
			if (i==ylenMax-1) break;
			mvaddch(i, j, d[cursor]);
			j++;
			cursor++;
			if (cursor==d.size()) break;
		}
	}


	chtype ch = getch();
	if ((ch == 27) || (ch=='b'))//if ESC key
	{
		return 0;
	}
	else if (ch == 259) //if UP key 259 (and down is 258)
	{
		if (k > 0) k--;
		else {
			k--;
			k+=inventory.size();
		}
		goto start;
	}
	else if (ch == 258) //if DOWN key
	{
		if (k < inventory.size()-1) k++;
		else{
			k++;
			k-=inventory.size();
		}
		goto start;
	}
	else if (ch == 'w') //if we want to wear
	{
		//equipped.push_back(inventory[k]);
		if (inventory.size()==0) goto start;
		if (wearThis(inventory[k])==0)
		{
		inventory.erase(inventory.begin()+k);
		if (k==inventory.size()) k--;
		}
		else warning=1;
		goto start;
	}
	else if (ch == 'e')
	{
		return viewEquipment();
	}
	else if (ch == 'x') //if we choose to expunge
	{
		//equipped.push_back(inventory[k]);
		if (inventory.size()>0)
		{
			inventory.erase(inventory.begin()+k);
			if (k==inventory.size())
			{
				k--;
				if (k<0) k = 0;
			}
		}
		goto start;
	}
	else if (ch == 'd') //if we choose to expunge
	{
		//equipped.push_back(inventory[k]);
		player_node* temp;
		if (inventory.size()>0)
		for (i = 0; i < ylenMax; i++)
		{
			for (j = 0; j < xlenMax; j++)
			{
				if(grid_players[j][i]!=NULL)
				{
					if (grid_players[j][i]->ifPC)
					{
						if (grid_objects[j][i]==NULL)
						{
							if (inventory.size()>0)
							{
								grid_objects[j][i] = inventory[k];
								object_death[inventory[k]->index] = 0;
								inventory.erase(inventory.begin()+k);
								if (k==inventory.size())
								{
									k--;
									if (k<0) k = 0;
								}
								goto start;

							}
						}
						else
						{
							goto start;
						}
					}
				}
			}
		}
		goto start;
	}

	else goto start;
}
/*This method will check if theres a slot available for a particular */
int wearThis(obj_node *o)
{
	int type = o->d->type;
	if (type==(type&(SCROLL-1)))//this checks if theres even a slot for this type in the equipment pack
	{
		int j = 1;
		for (int i = 0; i < 11; i++)
		{
			if (j==type)
			{
				if (type==(type&eq_vector))
				{
					if (type!=RING) return 1;//its already occupied, unless its ring with two slots
					else{
						if (ring2==1) return 1;
						else
						{
							ring2 = 1;
							equipped[11] = o;
							return 0;
						}
					}
				}
				else
				{
					equipped[i] = o;
					eq_vector+=type;
					return 0;
				}
				// if (type==RING){
				// 	if (ring2==1) return 1;
				// 	else{
				// 		ring2 = 1;
				// 		equipped[11] = o;
				// 		return 0;
				// 	}
				// }
			}
			j*=2;
		}
	}
	else return 2;//no slots for weapon of this type
	return 0;
}

int readyPC()
{
	PC* p = NULL;
	for (int i = 0; i < ylenMax; i++)
	{
		for (int j = 0; j < xlenMax; j++)
		{
			if(grid_players[j][i]!=NULL)
			{
				if(grid_players[j][i]->ifPC==1) p = grid_players[j][i]->pc;
			}
		}
	}

	if (p!=NULL)
	{
		p->totDam = roll_dice(p->dam);
		p->speed = 10;
		//int roll_dice(int* d);
		for (int i = 0; i < equipped.size(); i++)
		{
			if (equipped[i]!=NULL){
				p->totDam += roll_dice(equipped[i]->d->dam);
				p->speed += roll_dice(equipped[i]->d->speed);
			}
		}
	}
	 return 0;
}

int untangle()
{
	for (int i = 0; i < ylenMax; i++)
	{
		for (int j = 0; j < xlenMax; j++)
		{
			if(grid_players[j][i]!=NULL)
			{
				if(grid_players[j][i]->ifPC==0)
				{
					grid_players[j][i]->npc->tangle = 0;
				}
			}
		}
	}
	return 0;
}
