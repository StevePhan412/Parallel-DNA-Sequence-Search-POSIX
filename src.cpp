#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>

using namespace std;

//CSCI4500
//Trung Phan - 12530351

// Global variables for the sequence and subsequence
string sequence;
int sequence_size;
string subsequence;
int subsequence_size;

// Global variables for tracking the best match
int best_match_count = 0;
int best_match_pos = 0;

// Global variables for thread management
int num_threads;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread function
void *search_subsequence(void *thread_num_ptr)
{
    int thread_num = *(int *)thread_num_ptr;

    // calculate the starting position of the sequence for this thread
    int start_pos = thread_num * (sequence_size - subsequence_size + 1) / num_threads;

    // calculate the ending position of the sequence for this thread
    int end_pos = (thread_num + 1) * (sequence_size - subsequence_size + 1) / num_threads;

    // handle the extreme case where the subsequence matches at the end of the sequence
    if (end_pos == sequence_size - subsequence_size + 1 && sequence.substr(end_pos - 1, subsequence_size) == subsequence)
    {
        start_pos = end_pos - 1;
    }

    // search the sequence for the subsequence
    int match_count = 0;
    int max_match_count = 0;
    int max_match_pos = 0;
    for (int i = start_pos; i < end_pos + subsequence_size; i++)
    {
        match_count = 0;
        for (int j = 0; j < subsequence_size; j++)
        {
            if (sequence[i + j] == subsequence[j])
            {
                match_count++;
            }
        }
        if (match_count > max_match_count)
        {
            max_match_count = match_count;
            max_match_pos = i;
        }
    }

    // update global variables if necessary
    pthread_mutex_lock(&mutex);
    if (max_match_count > best_match_count)
    {
        best_match_count = max_match_count;
        best_match_pos = max_match_pos;
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[])
{
    // Check command line arguments
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " <sequence_file> <subsequence_file> <num_threads>" << endl;
        exit(1);
    }

    // Parse command line arguments
    string sequence_file = argv[1];
    string subsequence_file = argv[2];
    num_threads = atoi(argv[3]);

    // Read in the sequence file
    ifstream sequence_fp(sequence_file);
    if (!sequence_fp.is_open())
    {
        cout << "Error: Can't open sequence file" << endl;
        exit(1);
    }
    getline(sequence_fp, sequence);
    sequence_size = sequence.length();
    sequence_fp.close();

    // Read in the subsequence file
    ifstream subsequence_fp(subsequence_file);
    if (!subsequence_fp.is_open())
    {
        cout << "Error: Can't open subsequence file" << endl;
        exit(1);
    }
    getline(subsequence_fp, subsequence);
    subsequence_size = subsequence.length();
    subsequence_fp.close();

    // Create threads
    threads = new pthread_t[num_threads];
    int *thread_nums = new int[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        thread_nums[i] = i;
        if (pthread_create(&threads[i], NULL, search_subsequence, &thread_nums[i]) != 0)
        {
            cout << "Error: Failed to create thread" << endl;
            exit(1);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Print results
    cout << "Result:" << endl
         << endl
         << argv[1] << " and " << argv[2] << endl
         << endl;
    cout << "Number of Threads: " << num_threads << endl;
    cout << "Best Match Position: " << best_match_pos << endl;
    cout << "Best Match Count: " << best_match_count << endl;

    // Clean up
    delete[] threads;
    delete[] thread_nums;
    pthread_mutex_destroy(&mutex);

    return 0;
}
