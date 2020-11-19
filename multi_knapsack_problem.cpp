#include"multi_knapsack_problem.h"

map<int, knapsack*> Greedy(vector<item> &items, vector<knapsack> &knapsacks)
{
	//item id -> knapssack
	map<int, knapsack*> id_map;
	map<int, int> temp_id_map;
	//according to value per weight, descending sort
	sort(items.begin(), items.end(), [](const item &a, const item &b) {
		return a.valPerWei > b.valPerWei;
	});
	//according to capacity, ascending sort
	sort(knapsacks.begin(), knapsacks.end(), [](const knapsack &a, const knapsack &b) {
		return a.capacity < b.capacity;
	});
	for (int i = 0; i < items.size(); i++)
	{
		//for best fit, find 
		auto knapsack_temp = lower_bound(
			knapsacks.begin(),
			knapsacks.end(),
			items[i].weight,
			[](const knapsack &a, const int &b) {
			return a.remaining < b;
		});
		if (knapsack_temp == knapsacks.end()) {
			continue;
		}
		items[i].flag = true;

		temp_id_map[items[i].id] = knapsack_temp->id;
		knapsack_temp->remaining -= items[i].weight;
		//resort knapsacks according to remaining
		//for std::sort() will change the address of vector items,so cant use id_map to store address of knapsack directly
		sort(knapsacks.begin(), knapsacks.end(), [](const knapsack &a, const knapsack &b) {
			return a.remaining < b.remaining;
		});
	}
	//converse temp_id_map to id_map
	for (auto it = temp_id_map.begin(); it != temp_id_map.end(); it++)
	{
		id_map[it->first] = &(*find_if(knapsacks.begin(), knapsacks.end(), [it](const knapsack &a) {
			return a.id == it->second;
		}));
	}
	cout << "Greedy Algorithm Result" << endl;
	print_result(id_map,items);
	return id_map;
}

map<int, knapsack*> Neighborhood_Search(vector<item>& items, vector<knapsack>& knapsacks)
{
	map<int, knapsack*> id_map = Greedy(items, knapsacks);
	//test data
	//map<int, knapsack*> id_map = test_data(items, knapsacks);
	bool find_better = true;
	while (find_better) {
		find_better = Rotation_Neighboor(items, knapsacks, id_map);
	}
	print_result(id_map, items);
	return id_map;
}

bool Rotation_Neighboor(vector<item>& items, vector<knapsack>& knapsacks, map<int, knapsack*>& id_map)
{
	vector<item*> unselected_items;
	vector<item*> selected_items;
	vector<knapsack> unempty_kanpsacks;
	double max_valPerWei = 0.0;
	bool find_better = false;
	item* temp_erase_item = nullptr;
	item* temp_insert_item = nullptr;
	item* temp_move_item = nullptr;
	for (int i = 0; i < items.size(); i++)
	{
		if (items[i].flag)
		{
			selected_items.push_back(&items[i]);
		}
		else
		{
			unselected_items.push_back(&items[i]);
		}
	}
	//neighboors
	for (int insert_item = 0;
		insert_item != unselected_items.size();
		insert_item++)
	{
		
		for (int move_item = 0; 
			move_item != selected_items.size();
			move_item++)
		{
			if (id_map[selected_items[move_item]->id]->remaining + selected_items[move_item]->weight - unselected_items[insert_item]->weight >= 0) {
				for (int erase_item = 0;
					erase_item != selected_items.size();
					erase_item++)
				{
					if (erase_item != move_item
						&& id_map[selected_items[erase_item]->id]->remaining + selected_items[erase_item]->weight - selected_items[move_item]->weight >= 0
						//if move_item and rease_item in the same packnack
						&&(id_map[selected_items[erase_item]->id] == id_map[selected_items[move_item]->id])
						== (id_map[selected_items[move_item]->id]->remaining + selected_items[erase_item]->weight - unselected_items[insert_item]->weight >= 0)
						) {
						//find a better solution
						if (unselected_items[insert_item]->valPerWei - selected_items[erase_item]->valPerWei > max_valPerWei)
						{
							find_better = true;
							//record the max neighboor
							max_valPerWei = unselected_items[insert_item]->valPerWei - selected_items[erase_item]->valPerWei;
							temp_erase_item = selected_items[erase_item];
							temp_insert_item = unselected_items[insert_item];
							temp_move_item = selected_items[move_item];
						}
					}
				}
			}
		}
	}
	if (!find_better)
	{
		cout << "Do not Find a Better One in Neighboors!" << endl;
	}
	if (find_better)
	{
		cout << "Find a Better One in Neighboors! " << find_better << endl;
		cout << "Rotation Way:" << endl;
		cout << "Insert Item's Id: " << temp_insert_item->id << endl;
		cout << "Inserted Knapsack's Id: " << id_map[temp_move_item->id] << endl;
		cout << "Move Item's Id: " << temp_move_item->id << endl;
		cout << "Inserted Knapsack's Id: " << id_map[temp_erase_item->id] << endl;
		cout << "Erased Item's Id: " << temp_erase_item->id << endl;
		//maintain the information of id_map and
		temp_insert_item->flag = true;
		temp_erase_item->flag = false;
		id_map[temp_insert_item->id] = id_map[temp_move_item->id];
		id_map[temp_move_item->id]->remaining = id_map[temp_move_item->id]->remaining
			+ temp_move_item->weight - temp_insert_item->weight;

		id_map[temp_move_item->id] = id_map[temp_erase_item->id];
		id_map[temp_erase_item->id]->remaining = id_map[temp_erase_item->id]->remaining
			+ temp_erase_item->weight - temp_move_item->weight;
		id_map.erase(temp_erase_item->id);
	}
	return find_better;
}

map<int, knapsack*> Tabu_Search(vector<item>& items, vector<knapsack>& knapsacks)
{
	const int iteration_time = 10;
	const int tabu_List_Size = 5;
	//if there have non tabu solution in the iteration
	bool non_tabu = true;
	//declare a deque of  10 size to store tabu list 
	vector<map<int, knapsack*>> tabu_map_list;
	vector<vector<item>> tabu_items_list;
	deque<map<int, knapsack*>> tabu_deque;
	/*map<int, knapsack*> id_map = Greedy(items, knapsacks);*/
	//test data
	map<int, knapsack*> id_map = test_data(items, knapsacks);
	tabu_deque.push_back(id_map);
	tabu_items_list.push_back(items);
	tabu_map_list.push_back(id_map);
	for (int i = 0; i < iteration_time && non_tabu; i++)
	{
		cout << "Iteration: " << i + 1 << endl;
		if (Tabu_Neighboor_search(items, knapsacks, id_map, tabu_deque)) {
			tabu_map_list.push_back(id_map);
			tabu_items_list.push_back(items);
			if (tabu_deque.size() >= tabu_List_Size)
			{
				tabu_deque.pop_front();
			}
			tabu_deque.push_back(id_map);
		}
		else
		{
			break;
		}
	}
	int max_position = max_element(tabu_items_list.begin(), tabu_items_list.end(),
		[](vector<item> const lv, vector<item> const rv){
		int left_value = 0, right_value = 0;
		for (auto it = lv.begin(); it != lv.end(); it++)
		{
			if (it->flag)
			{
				left_value += it->value;
			}
		}
		for (auto it = rv.begin(); it != rv.end(); it++)
		{
			if (it->flag)
			{
				right_value += it->value;
			}
		}
		return left_value < right_value;
	}) - tabu_items_list.begin();
	cout << "Best One in Tabu List: " << endl;
	print_result(tabu_map_list[max_position], tabu_items_list[max_position]);
	return id_map;
}

bool Tabu_Neighboor_search(vector<item>& items, vector<knapsack>& knapsacks, map<int, knapsack*>& id_map, deque<map<int, knapsack*>> tabu_deque)
{
	vector<item*> unselected_items;
	vector<item*> selected_items;
	vector<knapsack> unempty_kanpsacks;
	double max_valPerWei = 0.0;
	bool find_better = false;
	item* temp_erase_item = nullptr;
	item* temp_insert_item = nullptr;
	item* temp_move_item = nullptr;
	map<int, knapsack*> temp_id_map = id_map;
	for (int i = 0; i < items.size(); i++)
	{
		if (items[i].flag)
		{
			selected_items.push_back(&items[i]);
		}
		else
		{
			unselected_items.push_back(&items[i]);
		}
	}
	//neighboors
	for (int insert_item = 0;
		insert_item != unselected_items.size();
		insert_item++)
	{

		for (int move_item = 0;
			move_item != selected_items.size();
			move_item++)
		{
			if (id_map[selected_items[move_item]->id]->remaining + selected_items[move_item]->weight - unselected_items[insert_item]->weight >= 0) {
				for (int erase_item = 0;
					erase_item != selected_items.size();
					erase_item++)
				{
					if (erase_item != move_item
						&& id_map[selected_items[erase_item]->id]->remaining + selected_items[erase_item]->weight - selected_items[move_item]->weight >= 0
						//if move_item and rease_item in the same packnack
						&& (id_map[selected_items[erase_item]->id] == id_map[selected_items[move_item]->id])
						== (id_map[selected_items[move_item]->id]->remaining + selected_items[erase_item]->weight - unselected_items[insert_item]->weight >= 0)) {
						//find a better solution
						if (unselected_items[insert_item]->valPerWei - selected_items[erase_item]->valPerWei > max_valPerWei)
						{
							temp_id_map[unselected_items[insert_item]->id] = temp_id_map[selected_items[move_item]->id];
							temp_id_map[selected_items[move_item]->id] = temp_id_map[selected_items[erase_item]->id];
							temp_id_map.erase(selected_items[erase_item]->id);
							//not find in tabu deque
							if (std::find(tabu_deque.begin(), tabu_deque.end(), temp_id_map) == tabu_deque.end()) {
								find_better = true;
								//record the max neighboor
								max_valPerWei = unselected_items[insert_item]->valPerWei - selected_items[erase_item]->valPerWei;
								temp_erase_item = selected_items[erase_item];
								temp_insert_item = unselected_items[insert_item];
								temp_move_item = selected_items[move_item];
							}
						}
					}
				}
			}
		}
	}
	if (!find_better)
	{
		cout << "Do not Find a Better One in Neighboors!" << endl;
	}
	if (find_better)
	{
		cout << "Find a Better One in Neighboors! " << find_better << endl;
		cout << "Rotation Way:" << endl;
		cout << "Insert Item's Id: " << temp_insert_item->id << endl;
		cout << "Inserted Knapsack's Id: " << id_map[temp_move_item->id]->id << endl;
		cout << "Move Item's Id: " << temp_move_item->id << endl;
		cout << "Inserted Knapsack's Id: " << id_map[temp_erase_item->id]->id << endl;
		cout << "Erased Item's Id: " << temp_erase_item->id << endl;
		//maintain the information of id_map and
		temp_insert_item->flag = true;
		temp_erase_item->flag = false;
		id_map[temp_insert_item->id] = id_map[temp_move_item->id];
		id_map[temp_move_item->id]->remaining = id_map[temp_move_item->id]->remaining
			+ temp_move_item->weight - temp_insert_item->weight;

		id_map[temp_move_item->id] = id_map[temp_erase_item->id];
		id_map[temp_erase_item->id]->remaining = id_map[temp_erase_item->id]->remaining
			+ temp_erase_item->weight - temp_move_item->weight;
		id_map.erase(temp_erase_item->id);
	}
	return find_better;
}




vector<knapsack> InitKnapsack(vector<int> rawKnapsacks)
{
	vector<knapsack> knapsacks;
	for (int i = 0; i < rawKnapsacks.size(); ++i)
	{
		knapsacks.push_back(knapsack(i + 1, rawKnapsacks[i]));
	}
	return knapsacks;
}

vector<item> InitItems(vector<pair<int, int>> rawItems)
{
	vector<item> items;
	for (int i = 0; i < rawItems.size(); ++i)
	{
		items.push_back(item(rawItems[i].first, rawItems[i].second, i + 1));
	}
	return items;
}

map<int, knapsack*> test_data(vector<item>& items, vector<knapsack>& knapsacks) {
	map<int, knapsack*> id_map = { {2, &knapsacks[0]},{3, &knapsacks[2]},{4, &knapsacks[1]},{5, &knapsacks[1]},{6, &knapsacks[2]},{7, &knapsacks[0]} };
	for (auto it = id_map.begin(); it != id_map.end(); it++)
	{
		items[it->first - 1].flag = true;
		it->second->remaining -= items[it->first - 1].weight;
	}
	return id_map;
}

void print_result(const map<int, knapsack*>& id_map, const vector<item>& items)
{
	int total_value = 0;
	for (auto it = id_map.begin(); it != id_map.end(); it++)
	{
		cout << "第 " << it->first << " 个物品，装入了第 " << it->second->id << " 个背包中。" << endl;
	}
	for (auto it = items.begin(); it != items.end(); it++)
	{
		if (it->flag)
		{
			total_value += it->value;
		}
	}
	cout << "背包中总共的价值为 " << total_value << endl;
}


int main() {
	vector<int> b = { 10,16,12 };
	vector<pair<int, int>> a = { {7,9},{7,5},{1,3},{4,6},{5,7},{6,8},{7,4} };
	vector<item> items = InitItems(a);
	vector<knapsack> knapsacks = InitKnapsack(b);
	//Greedy(items, knapsacks);
	/*Neighborhood_Search(items, knapsacks);*/
	Tabu_Search(items, knapsacks);
	system("pause");
}