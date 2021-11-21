// Datastructures.hh
//
// Student name: Matias Aitolahti
// Student email: matias.aitolahti@tuni.fi
// Student number: H293115

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <exception>

// Types for IDs
using TownID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
TownID const NO_TOWNID = "----------";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for a distance (in metres)
using Distance = int;

// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg) : msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};


// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: Documentation states the complexity
    // of .size() is constant.
    unsigned int town_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Documentation states the complexity
    // of .clear() is linear.
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: The complexity of .find() is
    // expected to be linear at worst, as is indexing the data structure.
    bool add_town(TownID id, Name const& name, Coord coord, int tax);

    // Estimate of performance: O(n)
    // Short rationale for estimate: The complexity of .find() is
    // expected to be linear at worst, as is indexing the data structure.
    Name get_town_name(TownID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: The complexity of .find() is
    // expected to be linear at worst, as is indexing the data structure.
    Coord get_town_coordinates(TownID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: The complexity of .find() is
    // expected to be linear at worst, as is indexing the data structure.
    int get_town_tax(TownID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: For-loop iterates through the entire data
    // structure once.
    std::vector<TownID> all_towns();

    // Estimate of performance: O(n)
    // Short rationale for estimate: For-loop iterates through the entire data
    // structure once.
    std::vector<TownID> find_towns(Name const& name);

    // Estimate of performance: O(n)
    // Short rationale for estimate: The complexity of .find() is
    // expected to be linear at worst, as is indexing the data structure.
    bool change_town_name(TownID id, Name const& newname);

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: Inserting into a map has a complexity
    // of O(log n), the for-loops are linear.
    std::vector<TownID> towns_alphabetically();

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: Inserting into a map has a complexity
    // of O(log n), the for-loops are linear.
    std::vector<TownID> towns_distance_increasing();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Std::min_element does at most
    // N comparisons, the calculations are constant.
    TownID min_distance();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Std::max_element does at most
    // N comparisons, the calculations are constant.
    TownID max_distance();

    // Estimate of performance: O(n)
    // Short rationale for estimate: The complexity of .find() is
    // expected to be linear at worst.
    bool add_vassalship(TownID vassalid, TownID masterid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: The complexity of .find() is
    // expected to be linear at worst, as is looping through vassals.
    std::vector<TownID> get_town_vassals(TownID id);

    // Estimate of performance: O(n²), theta(n)
    // Short rationale for estimate: The for-loop can potentially loop
    // through the entire data structure, as well as index the data structure on
    // each round of the loop with .at().
    std::vector<TownID> taxer_path(TownID id);

    // Non-compulsory phase 1 operations

    // Estimate of performance:
    // Short rationale for estimate:
    bool remove_town(TownID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> towns_nearest(Coord coord);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> longest_vassal_path(TownID id);

    // Estimate of performance:
    // Short rationale for estimate:
    int total_net_tax(TownID id);

private:
    // Struct that contains all data for a single town
    struct town_data {
        TownID town_id;
        Name name;
        Coord coord;
        int tax;
        town_data *master_ = nullptr;
        std::vector<town_data*> vassals_ = {};

    } ;

    using Distance = int;

    const Coord origin_ = {0,0};

    std::unordered_map<TownID, town_data> all_town_data_;

    // Estimate of performance: O(1), theta(1)
    // Short rationale for estimate: A simple calculation
    int distance_from_coord(Coord coord1, Coord coord2);

};

#endif // DATASTRUCTURES_HH
