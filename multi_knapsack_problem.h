#pragma once
#include<vector>
#include<iostream>
#include<algorithm>
#include<unordered_map>
#include<deque>
#include<map>
using namespace std;

struct item
{
	//item id
	int id;
	int value;
	int weight;
	//is item be choosed
	bool flag;
	//value per weight unit
	double valPerWei;
	item(){}
	item(int value, int weight, int id, bool flag = false) :value(value), weight(weight), id(id), flag(flag) {
		valPerWei = double(value) / double(weight);
	}
};
struct knapsack
{
	int id;
	int capacity;
	int remaining;
	knapsack(int id, int capacity) :id(id), capacity(capacity), remaining(capacity) {}
};

vector<knapsack> InitKnapsack(vector<int> rawKnapsacks);
vector<item> InitItems(vector<pair<int, int>> rawItems);
map<int, knapsack*> Greedy(vector<item> &items,
	vector<knapsack> &knapsacks);
	map<int, knapsack*> Neighborhood_Search(vector<item> &items,
	vector<knapsack> &knapsacks);
bool Rotation_Neighboor(vector<item> &items,
	vector<knapsack> &knapsacks,
	map<int, knapsack*> &id_map);
	map<int, knapsack*>Tabu_Search(vector<item> &items,
	vector<knapsack> &knapsacks);
bool Tabu_Neighboor_search(vector<item> &items,
	vector<knapsack> &knapsacks,
	map<int, knapsack*> &id_map,
	deque<map<int, knapsack*>>);



map<int, knapsack*> test_data(vector<item>& items, vector<knapsack>& knapsacks);

void print_result(const map<int, knapsack*> &id_map, const vector<item>& items);