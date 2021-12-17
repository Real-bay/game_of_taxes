// Datastructures.cc
//
// Student name: Matias Aitolahti
// Student email: matias.aitolahti@tuni.fi
// Student number: H293115

#include "datastructures.hh"

#include <random>

#include <cmath>

#include <map>

#include <list>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Write any initialization you need here
}

Datastructures::~Datastructures()
{
    // Write any cleanup you need here
}

unsigned int Datastructures::town_count()
{
    // Gets the count of towns
    return all_town_data_.size();
}

void Datastructures::clear_all()
{
    // Clears the data structure of all data.
    all_town_data_.clear();
}

bool Datastructures::add_town(TownID id, const Name &name, Coord coord, int tax)
{
    // Check to see if the town already exists
    if (all_town_data_.find(id) != all_town_data_.end())
    {
        return false;
    }

    // Form a new struct and add it to the data structure
    struct town_data new_town;
    new_town.town_id = id;
    new_town.name = name;
    new_town.coord = coord;
    new_town.tax = tax;

    all_town_data_[id] = new_town;

    return true;
}

Name Datastructures::get_town_name(TownID id)
{
    // Check to see if the town exists
    if (all_town_data_.find(id) == all_town_data_.end())
    {
        return NO_NAME;
    }

    // Return the specified town's name
    Name townname = all_town_data_.at(id).name;
    return townname;

}

Coord Datastructures::get_town_coordinates(TownID id)
{
    // Check to see if the town exists
    if (all_town_data_.find(id) == all_town_data_.end())
    {
        return NO_COORD;
    }

    // Return the specified town's coordinates
    Coord towncoord = all_town_data_.at(id).coord;
    return towncoord;
}

int Datastructures::get_town_tax(TownID id)
{
    // Check to see if the town exists
    if (all_town_data_.find(id) == all_town_data_.end())
    {
        return NO_VALUE;
    }

    // Return the specified town's tax
    int towntax = all_town_data_.at(id).tax;
    return towntax;
}

std::vector<TownID> Datastructures::all_towns()
{
    std::vector<TownID> towns;

    // Go through all the towns and add their ID's to the vector
    for(auto& it: all_town_data_)
    {
        TownID id = it.first;
        towns.push_back(id);
    }

    return towns;
}

std::vector<TownID> Datastructures::find_towns(const Name &name)
{
    std::vector<TownID> found_towns;

    // Go through all the towns and add the ones with the specified name
    for (auto& it: all_town_data_)
    {
        Name town_name = it.second.name;
        if (town_name == name)
            found_towns.push_back(it.first);
    }

    return found_towns;
}

bool Datastructures::change_town_name(TownID id, const Name &newname)
{
    // Check that the town exists
    if (all_town_data_.find(id) == all_town_data_.end())
    {
        return false;
    }

    // Set the new name
    all_town_data_.at(id).name = newname;
    return true;

}

std::vector<TownID> Datastructures::towns_alphabetically()
{
    std::multimap<Name, TownID> mm_alphabetical;
    std::vector<TownID> vec_alphabetical;


    // Copies all towns to a map for alphabetical order by town name
    for (auto& it: all_town_data_)
    {
        mm_alphabetical.insert({it.second.name,it.first});
    }

    // Creates a vector for the return
    for (auto& it: mm_alphabetical)
    {
        vec_alphabetical.push_back(it.second);
    }

    return vec_alphabetical;

}

std::vector<TownID> Datastructures::towns_distance_increasing()
{
    std::multimap<int,TownID> mm_distance;
    std::vector<TownID> vec_distance;

    // Copies all towns to a map for distance order
    for (auto& it: all_town_data_)
    {
        int dist = distance_from_coord(origin_,it.second.coord);
        mm_distance.insert({dist,it.first});

    }

    // Creates a vector for the return
    for (auto& it: mm_distance)
    {
        vec_distance.push_back(it.second);
    }

    return vec_distance;
}

TownID Datastructures::min_distance()
{
    // If no towns are found, return NO_TOWNID
    if (all_town_data_.empty())
    {
        return NO_TOWNID;
    }

    // Find the closest town to origin with std::min_element()
    auto min = std::min_element(all_town_data_.begin(),all_town_data_.end(),
                                [this](auto town1, auto town2)
                                {return distance_from_coord(origin_,town1.second.coord) <
                                distance_from_coord(origin_,town2.second.coord);});

    // Return the closest town
    return min->first;
}

TownID Datastructures::max_distance()
{
    // If no towns are found, return NO_TOWNID
    if (all_town_data_.empty())
    {
        return NO_TOWNID;
    }

    // Find the furthest away town from the origin with std::max_element()
    auto max = std::max_element(all_town_data_.begin(),all_town_data_.end(),
                                [this](auto town1, auto town2)
                                {return distance_from_coord(origin_,town1.second.coord) >
                                distance_from_coord(origin_,town2.second.coord);});

    // Return the furthest away town
    return max->first;
}

bool Datastructures::add_vassalship(TownID vassalid, TownID masterid)
{
    auto master = all_town_data_.find(masterid);
    auto vassal = all_town_data_.find(vassalid);

    // Check that the master exists
    if (master == all_town_data_.end())
    {
        return false;
    }
    // Check that the vassal exists
    else if (vassal == all_town_data_.end())
    {
        return false;
    }
    // Check that the vassal doesn't have a master yet
    if (vassal->second.master_ != nullptr)
    {
        return false;
    }

    // Assign the master as the vassals master pointer
    vassal->second.master_ = &master->second;

    // Add the vassal to the master's vassals-vector
    master->second.vassals_.push_back(&vassal->second);

    return true;

}

std::vector<TownID> Datastructures::get_town_vassals(TownID id)
{
    std::vector<TownID> vassals_vec;
    auto town = all_town_data_.find(id);

    // Check if id exists, if not return vector with NO_TOWNID
    if (town == all_town_data_.end())
    {
        vassals_vec.push_back(NO_TOWNID);
        return vassals_vec;
    }

    // Check if the town has no vassals at all, if so returns an empty vector
    if (town->second.vassals_.empty())
    {
        return vassals_vec;
    }

    // Form vector of vassals
    for (auto it: town->second.vassals_)
    {
        vassals_vec.push_back(it->town_id);
    }

    return vassals_vec;

}

std::vector<TownID> Datastructures::taxer_path(TownID id)
{
    std::vector<TownID> all_taxers;
    auto town = all_town_data_.find(id);

    // If no town is found, return a vector with just NO_TOWNID
    if (town == all_town_data_.end())
    {
        all_taxers.push_back(NO_TOWNID);
        return all_taxers;
    }

    // Add the initial town to vector
    all_taxers.push_back(town->second.town_id);

    // Check if there is no master at all
    if (town->second.master_ == nullptr)
    {
        return all_taxers;
    }


    auto current_taxer = town->second.master_;

    // Loop through masters until nullptr
    // Add current master to vector each time
    for(;;)
    {

        if (current_taxer == nullptr)
        {
            return all_taxers;
        }

        all_taxers.push_back(current_taxer->town_id);
        current_taxer = all_town_data_.at(current_taxer->town_id).master_;
    }

}

bool Datastructures::remove_town(TownID /*id*/)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    throw NotImplemented("remove_town()");
}

std::vector<TownID> Datastructures::towns_nearest(Coord /*coord*/)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    throw NotImplemented("towns_nearest()");
}

std::vector<TownID> Datastructures::longest_vassal_path(TownID /*id*/)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    throw NotImplemented("longest_vassal_path()");
}

int Datastructures::total_net_tax(TownID /*id*/)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    throw NotImplemented("total_net_tax()");
}


//
// Phase 2 operations
//


void Datastructures::clear_roads()
{
    // Clears the roads_ and connected_towns_ -vectors of each town.
    std::for_each(all_town_data_.begin(),all_town_data_.end(),[] (auto& town) {town.second.roads_.clear();});
    std::for_each(all_town_data_.begin(),all_town_data_.end(),[] (auto& town) {town.second.connected_towns_.clear();});
    all_roads_.clear();
}

std::vector<std::pair<TownID, TownID>> Datastructures::all_roads()
{
    return all_roads_;
}

bool Datastructures::add_road(TownID town1, TownID town2)
{
    auto town_1 = all_town_data_.find(town1);
    auto town_2 = all_town_data_.find(town2);
    auto dist = distance_from_coord(town_1->second.coord,town_2->second.coord);

    // Check that the towns exist and they are distinct
    if (town_1 == all_town_data_.end()
            || town_2 == all_town_data_.end()
            || town1 == town2)
    {
        return false;
    }

    // Check that there is no road between the towns yet
    if (std::find(town_1->second.connected_towns_.begin(),town_1->second.connected_towns_.end(), town2)
            != town_1->second.connected_towns_.end())
    {
        return false;
    }


    // Add the road to all_roads_
    if(town_1->first < town_2->first)
    {
        all_roads_.push_back(std::make_pair(town_1->first,town_2->first));
    } else {
        all_roads_.push_back(std::make_pair(town_2->first,town_1->first));
    }


    // Add the road to both towns' connected_towns_
    town_1->second.connected_towns_.push_back(town2);
    town_2->second.connected_towns_.push_back(town1);

    // Create pointers to the towns' data structs
    auto town1_data = &town_1->second;
    auto town2_data = &town_2->second;

    // Make pairs with those pointers and distances

    auto town1_road = std::make_pair(town2_data, dist);
    auto town2_road = std::make_pair(town1_data, dist);

    // Finally, add the pairs to boths towns' roads_
    town_1->second.roads_.push_back(town1_road);
    town_2->second.roads_.push_back(town2_road);

    return true;
}

std::vector<TownID> Datastructures::get_roads_from(TownID id)
{
    auto town = all_town_data_.find(id);

    // Check that the town exists
    // If no town is found, return a vector with just NO_TOWNID
    if (town == all_town_data_.end())
    {
        std::vector<TownID> no_town;
        no_town.push_back(NO_TOWNID);
        return no_town;
    }

    // If the town is found, we can just return its corresponding connected_towns_
    return town->second.connected_towns_;
}

std::vector<TownID> Datastructures::any_route(TownID fromid, TownID toid)
{
    // Uses least_towns_route to avoid copy-paste code
    return least_towns_route(fromid,toid);
}

bool Datastructures::remove_road(TownID /*town1*/, TownID /*town2*/)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    throw NotImplemented("remove_road()");
}

std::vector<TownID> Datastructures::least_towns_route(TownID fromid, TownID toid)
{
    auto start_town = all_town_data_.find(fromid);
    auto end_town = all_town_data_.find(toid);
    std::vector<TownID> found_route;

    // Check that the towns exist
    // If no towns are found, return a vector with just NO_TOWNID
    if (start_town == all_town_data_.end() || end_town == all_town_data_.end())
    {
        found_route.push_back(NO_TOWNID);
        return found_route;
    }


    // BFS


    // Set up queue
    std::list<town_data*> queue;

    // Initialize all nodes
    for (auto it : all_town_data_)
    {
        it.second.search_parent = nullptr;
        it.second.search_color = white;
        // "set to infinity"
        it.second.search_distance = INT32_MAX;
    }

    // Start node to grey and add it to the queue
    start_town->second.search_color = grey;
    start_town->second.search_distance = 0;
    auto *from_town = &start_town->second;
    queue.push_back(from_town);

    // While-loop until queue is empty
    while (!queue.empty())
    {
        // Pop a node from the queue, u
        auto u = queue.front();
        queue.pop_front();

        // For-loop to go through the neighboring nodes, v
        for (auto v : u->roads_)
        {
            // If v is white, then
            if (v.first->search_color == white)
            {

               // Set u as the parent of v
                v.first->search_parent = u;

                // Set v to grey
                v.first->search_color = grey;

                // Set v-distance to u-distance+1
                v.first->search_distance = u->search_distance + 1;

                // Push v to (the "end" of) the queue
                queue.push_front(v.first);

                // Break if the end of the route has been reached
                if(v.first->town_id == toid) {
                    break;
                }
            }

            // Set u to black
            u->search_color = black;

        }

    }

    // Construct the return vector
    found_route.push_back(toid);

    auto temp = end_town->second;

    while(temp.search_parent != nullptr)
    {
        found_route.push_back(temp.search_parent->town_id);
        temp = *temp.search_parent;
    }

    // Reverse the vector to the proper order (start, ... , end)
    std::reverse(found_route.begin(), found_route.end());

    return found_route;
}

std::vector<TownID> Datastructures::road_cycle_route(TownID /*startid*/)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    throw NotImplemented("road_cycle_route()");
}

std::vector<TownID> Datastructures::shortest_route(TownID /*fromid*/, TownID /*toid*/)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    throw NotImplemented("shortest_route()");
}

Distance Datastructures::trim_road_network()
{
    // Replace the line below with your implementation
    throw NotImplemented("trim_road_network()");
}

int Datastructures::distance_from_coord(Coord coord1, Coord coord2 )
{
    return (sqrt((coord1.x-coord2.x)*(coord1.x-coord2.x) + (coord1.y-coord2.y)*(coord1.y-coord2.y)));
}
