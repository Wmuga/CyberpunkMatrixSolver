#include <iostream>
#include <vector>
#include <set>
#include <ctime>
#include <map>
using namespace std;

string* matrix;
int matrix_size, buffer_size;
vector<string> sequences;
bool foundBest =false;

using path_array  = vector<pair<int,int>>;
using seq_nums = vector<short>;

map<short,map<seq_nums,path_array>> seq_path_arranged;
map<short,map<seq_nums,string>> seq_lines_arranged;

void get_data(){
    cout <<"Matrix:\n";
    FILE* filein = fopen("in.txt","r");
    for (int line=0;line<matrix_size;line++){
        char buffer[matrix_size*2+1];
        fread(buffer,1,matrix_size*2+1,filein);
        for (int byte=0;byte<matrix_size;byte++){
            (matrix)[line*6+byte]=buffer[byte*2];
            (matrix)[line*6+byte]+=buffer[byte*2+1];
            cout << (matrix)[line*6+byte] << " ";
        }
        cout << endl;
    }
    cout << "Sequences\n";
    char buffer[256];
    while(fscanf(filein,"%s",buffer)!=EOF){
        sequences.emplace_back(buffer);
        cout << buffer << endl;
    }
}

bool ContainsCoords(const vector<pair<int,int>>& thisPath,const pair<int,int>& coord){
    for (auto c: thisPath){
        if (c.first==coord.first && c.second==coord.second) return true;
    }
    return false;
}


void get_possible_sequences(int& index_x, int index_y, vector<pair<int,int>> thisPath, string thisStr="", bool xy=true){
    if (foundBest) return;
    if (!thisStr.empty() && thisStr.length()/2==buffer_size) {
        short isSeq = 0,seq_num=0;
        vector<short> linedSeq;
        linedSeq.reserve(sequences.size());
        for (const auto& seq: sequences){
            if (thisStr.find(seq)!=string::npos) {
                linedSeq.emplace_back(seq_num);
                isSeq++;
            }
            seq_num++;
        }
        if (isSeq) {
            if (seq_path_arranged[isSeq].count(linedSeq)==0) {
                seq_path_arranged[isSeq][linedSeq] = thisPath;
                seq_lines_arranged[isSeq][linedSeq] = thisStr;
            }

            if (isSeq == sequences.size()) {
                foundBest = true;
            }
        }
    }
    else{
        thisStr+=matrix[index_y*matrix_size+index_x];
        thisPath.emplace_back(index_x, index_y);
        for (int i=0;i<matrix_size;i++){
            if ( xy && !ContainsCoords(thisPath,make_pair(index_x,i))) get_possible_sequences(index_x, i, thisPath, thisStr, !xy);
            if (!xy && !ContainsCoords(thisPath,make_pair(i,index_y))) get_possible_sequences(i, index_y, thisPath, thisStr, !xy);
        }
    }
}

void set_search(int start_index,int end_index){
    for (int i=start_index;i<=end_index;i++){
        vector<pair<int,int>> thisPath;
        thisPath.reserve(matrix_size);
        get_possible_sequences(i,0,thisPath);
    }
}

void get_possible_sequences_start(){
    set<string> s1,s2,s3;
    //Огрызок для поиска в нескольких потоках
    set_search(0,matrix_size/3);
    set_search(matrix_size/3+1,matrix_size/3*2);
    set_search(matrix_size/3*2+1,matrix_size-1);

}

int main() {
    cout << "Type matrix size and buffer size\n";
    cin>>matrix_size>>buffer_size;
    matrix =  new string[matrix_size*matrix_size];
    get_data();
    auto start = clock();
    get_possible_sequences_start();
    cout << "Done\nElapsed time = " << (clock()-start) <<" clock ticks" << endl;
    if (foundBest){
        cout <<"Best Line: " << (*(seq_lines_arranged[sequences.size()].begin())).second;
        cout <<"\nPath: ";
        for (auto coord: (*(seq_path_arranged[sequences.size()].begin())).second) cout << "( x:" << coord.first << " y:" << coord.second << " ), ";
        cout << endl;
    }
    else{
        auto path = seq_path_arranged.begin();
        auto line = seq_lines_arranged.begin();
        for (int score=0; score < seq_path_arranged.size(); score++){
            cout << "============ AT " << path->first << " SEQUENCES ============" << endl;
            auto path_with_seq =path->second.begin();
            auto line_with_seq =line->second.begin();
            for (int lined_seq=0;lined_seq<path->second.size();lined_seq++){
                cout << "Lined sequences: ";
                for (auto seq_num:line_with_seq->first) cout << seq_num+1 << " ";
                cout << endl << "Line: " << line_with_seq->second << endl << "Path: ";
                for (auto coord:path_with_seq->second) cout << "( x:" << coord.first << " y:" << coord.second << " ), ";
                cout << endl << "----------------------------\n";
                path_with_seq++;
                line_with_seq++;
            }
            path++;
            line++;
        }
    }
    delete[]matrix;
    return 0;
}
