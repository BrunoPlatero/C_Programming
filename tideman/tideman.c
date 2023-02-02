#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool is_cycle(int winner, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Iterate over all candidates, and when candidate found, add it's index in argv[] to the ranks array
    // First candidate voted will have index 0, second index 1 and so on.
    for (int i = 0; i < candidate_count; i++)
    {
        if (!strcmp(candidates[i], name))
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Iterate over all candidates
    // keep adding +1 in preferences[i][j] every time they're preferred over another candidate
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]] ++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // Iterate over all candidates
    for (int i = 0; i < candidate_count; i++)
    {
        // If one candidate is preferred over another given preferences[i][j] scores,
        // declare them as winner and loser respectively in pairs[pairs_count] array
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        // declare max strenght victory to the first pair
        int max = i;
        // Declare the current strenght of the first pair
        int current_strength = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
        for (int j = i + 1; j < pair_count; j++)
        {
            // If a bigger strenght victory is found over the next pairs, assing that to max
            int temp_strenght = preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner];
            if (temp_strenght > current_strength)
            {
                max = j;
                current_strength = j;
            }
        }

        // Selection sort. Create a temp variable to store i's value and swap it with the max value
        // found until that moment.
        pair t = pairs[i];
        pairs[i] = pairs[max];
        pairs[max] = t;
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // If there's no cycle, add an 'arrow' from the winner to the loser
    for (int i = 0; i < pair_count; i++)
    {
        if (!is_cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
}


// Check if there is a cycle
bool is_cycle(int winner, int loser)
{
    // Trace back from the current winner to find out the original winner.
    // If the current loser has been a winner, that would create a cycle
    while (winner != -1 && winner != loser)
    {
        bool found = false;
        for (int i = 0; i < candidate_count; i++)
        {
            // If locked[i][i] is true, means that we found the previous winner and trace another step back to find
            // who won to this winner
            if (locked[i][winner])
            {
                found = true;
                winner = i;
            }
        }
        // If no winner found, end the while loop and no cycle would be created
        if (!found)
        {
            winner = -1;
        }

        // If tracing steps back we found that the loser has ever been a winner, that would create a cycle
        if (winner == loser)
        {
            return true;
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    // Iterate over all candidates finding which one has their column full of falses, meaning did not lose any head to head
    for (int i = 0; i < candidate_count; i++)
    {
        // Set current source to i
        bool found_source = true;
        for (int j = 0; j < candidate_count; j++)
        {
            // If any candidate has a true on their column, means that is not the winner and break
            // out of the conditional and iterate over the next candidate. If there's no candidate
            // with their column full of falses, no winner will be printed
            if (locked[j][i])
            {
                found_source = false;
                break;
            }
        }
        // If the source is found (set initially to true; and never enters the locked[i][j] statement)
        // means that this candidate has their column full of falses therefore we have a winner
        if (found_source)
        {
            printf("%s\n", candidates[i]);
            return;
        }
    }
    return;
}
