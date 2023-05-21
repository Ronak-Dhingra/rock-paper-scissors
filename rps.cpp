#include <iostream>
#include <bits/stdc++.h>
#include <cstdlib>
#include <time.h>
using namespace std;

// checks if you strictly win a round against opponent - returns 0 in this case
int checker(char a, char b)
{
    if ((a == 'R' && b == 'P') || (a == 'P' && b == 'S') || (a == 'S' && b == 'R') || (a == b))
    {
        return 1;
    }
    return 0;
}

int main()
{
    int n1, r1, p1, s1;
    char c1;
    cin >> n1;
    tuple<char, int, int, int> state[n1]; // array of tuples storing input FSA 
    int count[n1][3][3] = {0}; // maintains count of m-n mappings of states where m,n = {R, P, S} 
    int max[3][3] = {0}; // finds maximum m-n mappings using count
    int idx[3][3] = {0}; // stores index/state number of maximum m-n mappings 
    // 0 - R;   1 - P;   2 - S
    for (int i = 0; i < n1; i++)
    {
        cin >> c1 >> r1 >> p1 >> s1;
        state[i] = make_tuple(c1, r1, p1, s1);
        if (c1 == 'R')
        {
            count[r1 - 1][0][0]++; // update the occurrence of R->r transition state  
            count[s1 - 1][0][2]++; // update the occurrence of R->s transition state
            // occurrence of p transition state is not being updated since we are not interested in filling R->p transition
        }
        if (c1 == 'P')
        {
            count[r1 - 1][1][0]++;
            count[p1 - 1][1][1]++;
        }
        if (c1 == 'S')
        {
            count[p1 - 1][2][1]++;
            count[s1 - 1][2][2]++;
        }

        if (count[r1 - 1][0][0] > max[0][0])
        {
            max[0][0] = count[r1 - 1][0][0]; 
            idx[0][0] = r1; // save index of most frequently mapped to R->r transition 
        }
        if (count[s1 - 1][0][2] > max[0][2])
        {
            max[0][2] = count[s1 - 1][0][2];
            idx[0][2] = s1;
        }
        if (count[r1 - 1][1][0] > max[1][0])
        {
            max[1][0] = count[r1 - 1][1][0];
            idx[1][0] = r1;
        }
        if (count[p1 - 1][1][1] > max[1][1])
        {
            max[1][1] = count[p1 - 1][1][1];
            idx[1][1] = p1;
        }
        if (count[p1 - 1][2][1] > max[2][1])
        {
            max[2][1] = count[p1 - 1][2][1];
            idx[2][1] = p1;
        }
        if (count[s1 - 1][2][2] > max[2][2])
        {
            max[2][2] = count[s1 - 1][2][2];
            idx[2][2] = s1;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (idx[i][j] == 0) 
            {
                idx[i][j]++; // we want to output 1-indexed values
            }
        }
    }
    int r, p, s;
    srand(time(NULL)); // random number generator
    char c;
    tuple<char, int, int, int> fsa[n1]; // array of tuples storing output FSA
    for (int i = 0; i < n1; i++)
    {
        // Initial attempt: Randomize all state->transition mappings
        r = (rand() % n1) + 1;
        p = (rand() % n1) + 1;
        s = (rand() % n1) + 1;
        char symb = get<0>(state[i]); // access input FSA states
        if (symb == 'R')
        {
            c = 'P'; // output FSA states should have a one-to-one mapping to input FSA states by being the state that beats it (the complement)
            r = get<2>(state[i]); // P->r transition of output FSA should equate to R->p transition of input FSA
            // Reason: once we match the state numbers of input and output FSA we will win every round
            p = idx[1][1]; // This is a probabilistic guess that P->p transition of output FSA goes to the most commonly mapped P->p transition in input FSA
            s = idx[2][1];
        }
        else if (symb == 'P')
        {
            c = 'S';
            p = get<3>(state[i]);
            r = idx[0][2];
            s = idx[2][2];
        }
        else
        {
            c = 'R';
            s = get<1>(state[i]);
            r = idx[0][0];
            p = idx[1][0];
        }
        fsa[i] = make_tuple(c, r, p, s);
    }

    // Clearly, the above output FSA isn't accurate enough as it is mostly based on probabilistic conjectures 
    // Make a simulator that identifies where you are losing and fixes errors 
    int check = 1, eval = 0;
    for (int i = 1; i < n1; i++) // round simulation starts from second row as initial state since by construction our output FSA beats the first row state maximally 
    {
        char opp_move = get<0>(state[i]);
        char my_move = get<0>(fsa[0]);
        int my_prev_state, opp_prev_state;
        // pre-compute opponent's next state
        int opp_next_state;
        if (my_move == 'R')
        {
            opp_next_state = (get<1>(state[i])) - 1;
        }
        else if (my_move == 'P')
        {
            opp_next_state = (get<2>(state[i])) - 1;
        }
        else
        {
            opp_next_state = (get<3>(state[i])) - 1;
        }
        // pre-compute your next state
        int my_next_state;
        if (opp_move == 'R')
        {
            my_next_state = get<1>(fsa[0]) - 1;
        }
        else if (opp_move == 'P')
        {
            my_next_state = get<2>(fsa[0]) - 1;
        }
        else
        {
            my_next_state = get<3>(fsa[0]) - 1;
        }
        // store the values of previous state
        opp_prev_state = i;
        my_prev_state = 0;
        // in case next states are the same, then we know by our construction our output FSA will win maximally so we go onto the next round
        // next  round implies taking next row's state as initial state
        if (my_next_state == opp_next_state)
        {
            continue;
        }
        // ran a for loop rather than a while loop to avoid TLE
        //  there are break statements to terminate the simulation
        for (int a = 0; a < 50; a++)
        {
            opp_move = get<0>(state[opp_next_state]);
            my_move = get<0>(fsa[my_next_state]);
            opp_prev_state = opp_next_state;
            my_prev_state = my_next_state;
            if (my_move == 'R')
            {
                opp_next_state = (get<1>(state[opp_next_state])) - 1;
            }
            else if (my_move == 'P')
            {
                opp_next_state = (get<2>(state[opp_next_state])) - 1;
            }
            else
            {
                opp_next_state = (get<3>(state[opp_next_state])) - 1;
            }

            if (opp_move == 'R')
            {
                my_next_state = get<1>(fsa[my_next_state]) - 1;
            }
            else if (opp_move == 'P')
            {
                my_next_state = get<2>(fsa[my_next_state]) - 1;
            }
            else
            {
                my_next_state = get<3>(fsa[my_next_state]) - 1;
            }
            char my_prev_move = get<0>(fsa[my_prev_state]);
            char opp_prev_move = get<0>(state[opp_prev_state]);
            if (my_next_state == opp_next_state)
            {
                break;
            }
            // if you win against your opponent then no need to make changes
            if (checker(my_prev_move, opp_prev_move) == 0)
            {
                continue;
            }
            // if you lose or draw
            eval++;
            if (eval == check)
            {
                eval = 0;
                check++; // the better your FSA gets, the less frequently you want to make changes to it
                // access your output FSA's previous state and change your next state so that it keeps beating your opponent
                char temp = get<0>(state[opp_prev_state]);
                char temp_c = get<0>(fsa[my_prev_state]);
                int temp_r = get<1>(fsa[my_prev_state]);
                int temp_p = get<2>(fsa[my_prev_state]);
                int temp_s = get<3>(fsa[my_prev_state]);
                if (temp == 'R')
                {
                    temp_r = opp_next_state + 1;
                }
                else if (temp == 'P')
                {
                    temp_p = opp_next_state + 1;
                }
                else
                {
                    temp_s = opp_next_state + 1;
                }
                tuple<char, int, int, int> tmp_tup; // make a temp tuple because tuples are immutable
                tmp_tup = make_tuple(temp_c, temp_r, temp_p, temp_s);
                fsa[my_prev_state].swap(tmp_tup);
                break;
            }
        }
    }

    cout << n1 << endl;
    for (int i = 0; i < n1; i++)
    {
        cout << get<0>(fsa[i]) << " " << get<1>(fsa[i]) << " " << get<2>(fsa[i]) << " " << get<3>(fsa[i]) << endl;
    }
}
