#include <bits/stdc++.h>
#define bucket_capacity 2
#define bit_size 16
#define N 150000

using namespace std;
string hash_val(unsigned short int ID)
{
    int arr[bit_size];
    string res;
    for (int i = bit_size-1; i >= 0; i--)
    {
        arr[i] = ID % 2;
        res += to_string(arr[i]);
        ID = ID / 2;
    }
    reverse(res.begin(), res.end());
    return res;
}
string RandString()
{
    string res = "";
    for (int i = 1; i <= 3; i++)
    {
        char ch = 'a' + rand() % 26;
        res = res + ch;
    }
    return res;
}
typedef struct d_a_t_a
{
    unsigned short int id;
    string hash_val;
    string left;
} d_a_t_a;

typedef struct bucket
{
    int ld; //local depth
    int size;      // bucketsize
    string Pos_val; // hashvalues
    d_a_t_a d_a_t_a_list[bucket_capacity];
    int overflow_bucket; // pointer to the overflow bucket

    bucket()
    {
        ld = 0;
        Pos_val = "";
        size = 0;
        overflow_bucket = -1; //  -1 indicates no overflow with current bucket
    }

} bucket;

typedef struct directory
{
    int gd; //global depth
    vector<string> Pos_val_list; // for storing hash Pos_val
    vector<int> bucket_add_list;
} directory;

bucket bucket_mem[N]; // Secondary Memory
int curr_bucket_index = 0;
int overflow_bucket_index = N - 1; // overflow index starting from end

void insert_data_bucket(bucket &b, d_a_t_a r)
{
    // insertion of d_a_t_a in the bucket
    b.d_a_t_a_list[b.size] = r;
    b.size++;
}

void Print_Bucket(int bucket_add)
{
    /* display rows or data in the bucket
     */
    bucket b = bucket_mem[bucket_add];
    while (true)
    {
        cout << "\n";
        cout << "Bucket ID: " << bucket_add << ", Pos_val: " << b.Pos_val << endl;
        printf("Local Depth: %d\n", b.ld);
        for (int i = 0; i < b.size; i++)
        {
            printf("ID: %d ", b.d_a_t_a_list[i].id);
            cout<<" -> " << b.d_a_t_a_list[i].left<< endl ;
            cout << "Hash Value: " << b.d_a_t_a_list[i].hash_val << endl;
        }
        // checking for overflow buckets
        if (b.overflow_bucket == -1)
        {
            break;
        }
        bucket_add = b.overflow_bucket;
        b = bucket_mem[bucket_add];
    }
}

void grow_dir(directory &dir)
{
    dir.gd++;

    vector<string> new_Pos_val_list;
    vector<int> new_bucket_add_list;

    // update the Pos_vales
    for (auto Pos_val : dir.Pos_val_list)
    {
        new_Pos_val_list.push_back(Pos_val + "0");
        new_Pos_val_list.push_back(Pos_val + "1");
    }
    dir.Pos_val_list = new_Pos_val_list; // ???

    // update the bucket list
    for (auto bucket_id : dir.bucket_add_list)
    {
        new_bucket_add_list.push_back(bucket_id);
        new_bucket_add_list.push_back(bucket_id);
    }
    dir.bucket_add_list = new_bucket_add_list;
}

void Visualise(directory &dir)
{
    // display directories and buckets.
    printf("\n");
    printf("Directory Structure Intialized:\n");
    printf("------------------------>\n");
    printf("Global Depth: %d\n", dir.gd);

    for (int i = 0; i < dir.bucket_add_list.size(); i++)
    {
        int bucket_id = dir.bucket_add_list[i];
        cout << "\n";
        printf("Directory ID: %d\n", i);
        printf("----------------->\n");
        printf("Pos_val: ");
        cout << dir.Pos_val_list[i] << endl;
        Print_Bucket(bucket_id);
    }
}




void split_bucket(int bucket_add)
{
    /* Creates an extra bucket
     * */
    curr_bucket_index++;                      // create a new bucket
    bucket_mem[bucket_add].ld++; // increasing ld
    string previous_Pos_val = bucket_mem[bucket_add].Pos_val;
    bucket_mem[bucket_add].Pos_val = previous_Pos_val + "0";

    bucket_mem[curr_bucket_index].ld = bucket_mem[bucket_add].ld; // ld of the new bucket
    bucket_mem[curr_bucket_index].Pos_val = previous_Pos_val + "1";
}

int fetch_bucket(directory &dir, d_a_t_a r)
{
    //search dir and return address of where to be inserted
    string hash_val = r.hash_val;                         // hashvalue of the data
    string to_check = hash_val.substr(0, dir.gd); // checking MSB Bits

    int directory_id = -1;
    for (int i = 0; i < dir.Pos_val_list.size(); i++)
    {
        if (to_check.compare(dir.Pos_val_list[i]) == 0)
        {
          
            directory_id = i;
            break;
        }
    }

    int bucket_add = dir.bucket_add_list[directory_id];

    return bucket_add;
}



void rehash_dir(directory &dir, int previous_bucket, int new_bucket)
{
    //previous_bucket:  contains the address of the previous bucket
     //new_bucket: contains the  address of the new bucket//

    // changing only those directories  who were pointing to the overflow bucket
    vector<int> dir_id_to_change;
    for (int i = 0; i < dir.bucket_add_list.size(); i++)
    {
        if (dir.bucket_add_list[i] == previous_bucket)
        {
            dir_id_to_change.push_back(i);
        }
    }

    cout << "Modify Directories:" << endl;
    for (int i : dir_id_to_change)
    {
        cout << i << " ";
    }
    cout << endl;

    string previous_bucket_Pos_val = bucket_mem[previous_bucket].Pos_val;


    int ld = bucket_mem[new_bucket].ld;

    for (auto d_id : dir_id_to_change)
    {
        string to_check = dir.Pos_val_list[d_id].substr(0, ld); // checking MSB
        if (to_check.compare(previous_bucket_Pos_val) == 0)
        {
            
            dir.bucket_add_list[d_id] = previous_bucket;
        }
        else
        {
    
            dir.bucket_add_list[d_id] = new_bucket;
        }
    }
}

void insert_data_overflow(int bucket_add, d_a_t_a r)
{
   //traverse to bucket where overflow is to be done
    int overflow_bucket_add = bucket_mem[bucket_add].overflow_bucket;
    while (overflow_bucket_add != -1)
    {
        bucket_add = overflow_bucket_add;
        overflow_bucket_add = bucket_mem[bucket_add].overflow_bucket;
    }
    //bucket_add now has address where overflow is to be done
    if (bucket_mem[bucket_add].size < bucket_capacity)
    {
        // if space in bucket enter
        insert_data_bucket(bucket_mem[bucket_add], r);
        return;
    }

    // else make a new overflow
    overflow_bucket_add = overflow_bucket_index;
    overflow_bucket_index--; //allocation of overflow bucket from rear side of the array

    // same local and global depth for overflow bucket
    bucket b = bucket_mem[bucket_add];
    b.size = 0;
    b.overflow_bucket = -1;
    bucket_mem[overflow_bucket_add] = b;
    insert_data_bucket(bucket_mem[overflow_bucket_add], r);         // inserting data
    bucket_mem[bucket_add].overflow_bucket = overflow_bucket_add; // pointing to  overflow bucket
}

void rehash_bucket(int previous_bucket, int new_bucket)
{

    vector<d_a_t_a> d_list; // 
    bucket previous_b = bucket_mem[previous_bucket];
    while (true)
    {

        for (int i = 0; i < previous_b.size; i++)
        {
            // addition of all rows fetched in to d_a_t_as
            d_a_t_a r = previous_b.d_a_t_a_list[i];
            d_list.push_back(r);
        }
        // checking the linked lists
        int linked_bucket = previous_b.overflow_bucket;
        if (linked_bucket == -1)
            break;
        previous_b = bucket_mem[linked_bucket];
    }

    bucket_mem[previous_bucket].size = 0; //  enter rows  again
    bucket_mem[previous_bucket].overflow_bucket = -1;
    int ld = bucket_mem[previous_bucket].ld;
    string previous_bucket_Pos_val = bucket_mem[previous_bucket].Pos_val;
    // rehashing on data of previous bucket
    for (d_a_t_a r : d_list)
    {
        string to_compare = r.hash_val.substr(0, ld);
        if (to_compare.compare(previous_bucket_Pos_val) == 0)
        {

            insert_data_overflow(previous_bucket, r);
        }
        else
        {
            // insert the d_a_t_a in new bucket
            insert_data_overflow(new_bucket, r);
        }
    }

    cout << "Rehashing Done" << endl;
}

void insert_data(directory &dir, d_a_t_a r)
{
 
    int bucket_add = fetch_bucket(dir, r);

    if (bucket_mem[bucket_add].size == bucket_capacity)
    {
        // while bucket not empty
        int ld = bucket_mem[bucket_add].ld;
        // if ld< gd

        // if lc>=gd expansion of directory
        if (dir.gd == ld)
        {
            grow_dir(dir);
        }

        // spliting buckets
        split_bucket(bucket_add);

        // rehashing buckets
        int previous_bucket = bucket_add;
        int new_bucket = curr_bucket_index;
        rehash_bucket(previous_bucket, new_bucket);

        // rehashing directories
        rehash_dir(dir, previous_bucket, new_bucket);

        //  insert data again
        bucket_add = fetch_bucket(dir, r);
        if (bucket_mem[bucket_add].size == bucket_capacity)
        {
            // create  overflow and insert again
            insert_data_overflow(bucket_add, r);
        }
        else
        {
            // no need of overflow bucket
            insert_data_bucket(bucket_mem[bucket_add], r);
        }
    }

    else
    {
        // inserting data in the bucket
        insert_data_bucket(bucket_mem[bucket_add], r);
    }
}

int main()
{
	//global variable with intial state
	bucket inital_bucket;
    bucket_mem[0] = inital_bucket;
    directory dir;
    dir.gd = 0;
    dir.Pos_val_list.push_back("");
    dir.bucket_add_list.push_back(curr_bucket_index);

    int rows;
    cout << "Enter the number of data rows: ";
    cin >> rows;
/*
Each d_a_t_a in this file contains four fields: 
(1) Transaction ID (an integer), 
(2) Transaction sale amount (an integer),(x>1 and x<500000)
 (3) Customer name (string 3 letter) and, 
 (4) category of item. (int 1-1500)
 

    Generating Dataset
    srand(time(0));
    ofstream ptr("dataset.txt");
    for (int i = 1; i <= rows; i++)
    {
        ptr << i << ","                             // Transaction ID
               << 1 + rand() % (500000 - 1 + 1) << "," // Transaction Amount
               << generateRandomStrings() << ","       // Customer Name
               << 1 + rand() % (1500 - 1 + 1) << "\n"; // Category
    }
    ptr.close();

*/
    


    ifstream ReadID("dataset1.txt");
    unsigned short int ID;
    char cred[20];
    for (int i = 1; i <= rows; i++)
    {
        string buffer;  
        getline(ReadID, buffer);
        sscanf((char *)&buffer[0], "%hu,%s[^,]", &ID, cred);
        // cout << ID << " => " << cred << endl;

        d_a_t_a obj;
        obj.id = ID;
        obj.hash_val = hash_val(ID);
        obj.left = cred;
        insert_data(dir,obj);
    }
    ReadID.close();
    cout << "\n";
    Visualise(dir);
    return 0;
}