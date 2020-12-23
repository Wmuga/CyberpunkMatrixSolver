#include <iostream>
#include <vector>
#include <set>
#include <ctime>
using namespace std;

string* matrix;
int matrix_size, buffer_size;
vector<string> sequences;
bool foundBest =false;
int best_score = 0;
string best_line;
vector<pair<int,int>> path;
vector<short> lined_seq;

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

//55 55 55 bd 7a 55 e9 7a
void get_possible_sequences(int& index_x, int index_y, vector<pair<int,int>> thisPath, string thisStr="", bool xy=true){
    if (foundBest) return;
    if (!thisStr.empty() && thisStr.length()/2==buffer_size) {
        int isSeq = 0;
        vector<short> linedSeq;
        for (const auto& seq: sequences){
            if (thisStr.find(seq)!=string::npos) {
                linedSeq.push_back(1);
                isSeq++;
            }
            else linedSeq.push_back(0);
        }
        if (isSeq>best_score) {
            best_score=isSeq;
            path = thisPath;
            best_line=thisStr;
            lined_seq=linedSeq;
        }
        if (isSeq==best_score){
            int thisScore=0;
            int score=1;
            for (auto s: linedSeq) {
                if (s) thisScore+=score;
                score++;
            }
            int outScore=0;
            score=1;
            for (auto s: lined_seq) {
                if (s) outScore+=score;
                score++;
            }
            if (thisScore>outScore){
                path=thisPath;
                best_line=thisStr;
                lined_seq=linedSeq;
            }
        }
        if (isSeq==sequences.size()){
            foundBest=true;
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
        get_possible_sequences(i,0,vector<pair<int,int>>());
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
    cout << "Lined up sequences: ";
    for (int i=0;i<lined_seq.size();i++) if (lined_seq[i]) cout << i+1 << " ";
    cout<< endl << "Line: ";
    for (int i=0;i<best_line.size()/2;i++) cout << best_line[i*2] << best_line[i*2+1] << " ";
    cout << endl << "path:";
    for (auto c: path) cout << "{ x:" << c.first << " y:" << c.second <<" } ";
    delete[]matrix;
    return 0;
}
