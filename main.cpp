#include <iostream>
#include <assert.h>
#include <time.h>
#include <math.h>
using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")

#pragma comment(linker,"/subsystem:console")


struct Node
{
	int x, y;

};


namespace Simulation
{
	int screen_width = 800;
	int screen_height = 600;

	unsigned char prev_key_state[256];
	unsigned char *keys = NULL;

	SDL_Window *window = NULL;
	SDL_Renderer * renderer = NULL;
	int nodeSize = 20;
	int n_nodes = 3;
	Node * nodes;

	int *CurrentPath = NULL;
	int *PermutedPath = NULL;
	int startIndex = 0;
	int endIndex = n_nodes - 1;

	double current_error = 0.0;
	float temperature = 10.0;
	float n_permute = 1;
	float tmp_decay = .98;
	int N_PermuteSameTmp = 3;

	int state = 0;
	Node traveller;
	int salesmanSize = 10;
	int destination;
	double travelSpeed = 5;
	float angle;

	void create_Random_Nodes()
	{
		for (int i = 0; i < n_nodes; i++)
		{

			nodes[i].x = rand() % (screen_width - nodeSize);
			nodes[i].y = rand() % (screen_height - nodeSize);
		}
	}

	void retrieve_Nodes()
	{
		nodes = (Node*)malloc(n_nodes * sizeof(Node));
		create_Random_Nodes();

	}

	void shuffle()
	{

		for (int i = n_nodes - 1; i >= 1; i--)
		{
			int j = rand() % i;
			int tmp = CurrentPath[j];
			CurrentPath[j] = CurrentPath[i];
			CurrentPath[i] = tmp;
		}
	}


	void makePath()
	{
		for (int i = 0; i < n_nodes; i++)
		{
			CurrentPath[i] = i;
		}

		shuffle();

		for (int i = 0; i < n_nodes; i++)
		{
			if (CurrentPath[i] == startIndex)
			{
				int tmp = CurrentPath[0];
				CurrentPath[0] = CurrentPath[i];
				CurrentPath[i] = tmp;
			}
			if (CurrentPath[i] == endIndex)
			{
				int tmp = CurrentPath[n_nodes - 1];
				CurrentPath[n_nodes - 1] = CurrentPath[i];
				CurrentPath[i] = tmp;
			}
		}
	}
	void permute_path(Node *n, int *path)
	{
		for (int i = 0; i < n_permute; i++)
		{
			int index = 1 + rand() % (n_nodes - 2);
			int tmp = path[index];
			bool check = true;
			int num = 0;
			while (check)
			{
				if (n_nodes == 3)check = false;
				num = 1 + rand() % (n_nodes - 2);
				if (num != index)
				{
					check = false;
				}
			}
			path[index] = path[num];
			path[num] = tmp;

		}
	}
	double distance(int * path)
	{
		double sum = 0.0;
		for (int i = 1; i < n_nodes-1; i++)
		{
			int x = nodes[path[i]].x  - nodes[path[i - 1]].x;
			int y = nodes[path[i]].y - nodes[path[i - 1]].y;
			sum += (x *x) + (y *y);
		}
		return sum;

	}
	void init()
	{
		SDL_Init(SDL_INIT_VIDEO);

		prev_key_state[256];
		keys = (unsigned char*)SDL_GetKeyboardState(NULL);

		SDL_Window *window = SDL_CreateWindow(
			"Travelling Salesman",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			screen_width, screen_height, SDL_WINDOW_SHOWN);

		renderer = SDL_CreateRenderer(window,
			-1, SDL_RENDERER_ACCELERATED);

		CurrentPath = (int*)malloc(sizeof(int*)*n_nodes);
		PermutedPath = (int*)malloc(sizeof(int*)*n_nodes);
		retrieve_Nodes();
		startIndex = rand() % n_nodes;
		for (;;)
		{
			if (n_nodes == 1)
			{
				endIndex = startIndex;
				break;
			}
			endIndex = rand() % n_nodes;
			if (endIndex != startIndex)break;
		}
		makePath();
		current_error = distance(CurrentPath);
		traveller.x = nodes[CurrentPath[0]].x + nodeSize * 0.5;
		traveller.y = nodes[CurrentPath[0]].y + nodeSize * 0.5;
		cout << "Traveller Start " << traveller.x << "   " << traveller.y << endl;
		cout << "Pathway Start " <<  nodes[CurrentPath[0]].x << "   " << nodes[CurrentPath[0]].y << endl;
		destination = 1;
		state = 0;
	}
	void draw_Paths()
	{
		for (int i = 1; i < n_nodes; i++)
		{

			SDL_RenderDrawLine(renderer, nodes[CurrentPath[i]].x + nodeSize*0.5, nodes[CurrentPath[i]].y + nodeSize*0.5,
				nodes[CurrentPath[i-1]].x + nodeSize*0.5, nodes[CurrentPath[i-1]].y + nodeSize*0.5);
			
		}
	}

	int collision()
	{
		if (traveller.x + salesmanSize < nodes[CurrentPath[destination]].x) return 0;
		if (traveller.x > nodes[CurrentPath[destination]].x + nodeSize) return 0;
		if (traveller.y + salesmanSize < nodes[CurrentPath[destination]].y) return 0;
		if (traveller.y > nodes[CurrentPath[destination]].y + nodeSize) return 0;
		return 1;
	}
	int getSign(int s, int d)
	{
		if (d - s > 0)return 1;
		else return -1;
	}

	void moveSalesman()
	{

		int colide = collision();
		if (colide==1)
		{
			destination++;

		}
		else
		{
			angle = atan2(nodes[CurrentPath[destination]].y + nodeSize * 0.5 - traveller.y, nodes[CurrentPath[destination]].x + nodeSize * 0.5 - traveller.x);
			cout << "angle " << angle << endl;
			double x = travelSpeed * cos(angle);
			double y = travelSpeed * sin(angle);
			cout << "x speed " << x << "   y speed " << y << endl;
			traveller.x += travelSpeed * cos(angle);
			traveller.y += travelSpeed * sin(angle);
		}

	}

	void update()
	{
		//copy previous key state
		memcpy(prev_key_state, keys, 256);

		//consume all window events, key state gets updated here automatically
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		if (state == 0)
		{
			memcpy(PermutedPath, CurrentPath, sizeof(int)*n_nodes);
			permute_path(nodes, PermutedPath);


			double permuted_error = distance(PermutedPath);
			cout << "permuted error: " << permuted_error << endl;
			float p = exp((current_error - permuted_error) / temperature);
			float r = (double)rand() / RAND_MAX;
			if (permuted_error < current_error)
			{
				memcpy(CurrentPath, PermutedPath, sizeof(int)*n_nodes);
				current_error = permuted_error;
			}
			else if (r < p)
			{
				memcpy(CurrentPath, PermutedPath, sizeof(int)*n_nodes);
				current_error = permuted_error;
			}
			printf("temp: %f current error: %f\n", temperature, current_error);

			temperature *= tmp_decay;
		}
		if (state == 1)
		{
			if (destination >= n_nodes)
			{
				
				retrieve_Nodes();
				makePath();
				current_error = distance(CurrentPath);
				temperature = 10;
				state = 0;
				destination = 1;
				traveller.x = nodes[CurrentPath[0]].x + nodeSize * 0.5;
				traveller.y = nodes[CurrentPath[0]].y + nodeSize * 0.5;
			}
			else moveSalesman();
		}

		if (temperature < 0.005)
		{
			state = 1;
		}

	}

	void draw()
	{
		//set color to white
		SDL_SetRenderDrawColor(renderer, 50, 55, 55, 255);
		//clear screen with white
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_Rect rect;
		rect.x = nodes[CurrentPath[0]].x;
		rect.y = nodes[CurrentPath[0]].y;
		rect.w = nodeSize;
		rect.h = nodeSize;
		SDL_RenderFillRect(renderer, &rect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		rect.x = nodes[CurrentPath[n_nodes-1]].x;
		rect.y = nodes[CurrentPath[n_nodes-1]].y;
		rect.w = nodeSize;
		rect.h = nodeSize;
		SDL_RenderFillRect(renderer, &rect);

		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		for (int i = 1; i < n_nodes-1; i++)
		{
			rect.x = nodes[CurrentPath[i]].x;
			rect.y = nodes[CurrentPath[i]].y;
			rect.w = nodeSize;
			rect.h = nodeSize;
			SDL_RenderFillRect(renderer, &rect);
		}
		if (state == 1)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_Rect salesman;
			salesman.x = traveller.x;
			salesman.y = traveller.y;
			salesman.h = salesmanSize;
			salesman.w = salesmanSize;
			SDL_RenderFillRect(renderer, &salesman);
		}
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		draw_Paths();

		//flip buffers
		SDL_RenderPresent(renderer);
	}

}
int main(int argc, char **argv)
{

	//srand(time(NULL));
	Simulation::init();

	int measure_freq = 100;
	int measure_frame = 0;

	unsigned int   currentTime, last_frame_time = 0;


	for (;;)
	{
		currentTime = SDL_GetTicks();
		if (currentTime - last_frame_time < 15) continue;
		float dt = (currentTime - last_frame_time) / 1000.0;
		last_frame_time = currentTime;

		Simulation::update();
		
		Simulation::draw();
	}


	return 0;
}