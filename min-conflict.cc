#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <sys/time.h>
#include <random>
#include <cmath>
#include "printTime.h"
#include "var_binary_heap.h"
#include "conflict_binary_heap.h"

using namespace std;

class nqueen{
public:
        nqueen(int n){
                random_device rnd;
                mt19937 mt(rnd());
                size = n;
                board = new int[n];
                for(int i=0;i<n;i++){
                        board[i] = -1;
                }
                initialize();
                // for(int i=0;i<n;i++){
                //         board[i] = mt()%n;
                // }
        }
        ~nqueen(){
                delete board;
        }
        void initialize(){
                random_device rnd;
                mt19937 mt(rnd());
                //greedy
                unordered_set<int> tmp;
                vector<pair<int,unordered_set<int> > > example(size);
                for(int i=0;i<size;i++){
                        example[i] = make_pair(i,tmp);
                }
                // set<int> up2down;
                // vector<int> up2down,ru2ld,lu2rd;
                int *up2down = (int*)malloc(sizeof(int) * size);
                int *ru2ld = (int*)malloc(sizeof(int) * size*2-1);
                int *lu2rd = (int*)malloc(sizeof(int) * size*2-1);
                // int up2down[size];
                // int ru2ld[size*2-1];
                // int lu2rd[size*2-1];
                //Todo
                //一番最後にconflictlistに入れる
                for(int i=0;i<size*2-1;i++){
                        if(i<size)
                                up2down[i] = 0;
                       ru2ld[i] = 0;
                       lu2rd[i] = 0;
                }
                cout << "is it here?" << endl;
                for(int i=0;i<size;i++){
                        bool flag = true;
                        int kouho,min=100;
                        int r2lstart = i;
                        int l2rstart = size-1+i;
                        int start = mt()%size;
                        for(int j=0;j<size;j++){
                                int nj = start+j;
                                if(nj >= size)
                                        nj -= size;
                                int u2d = up2down[nj];
                                int r2l = ru2ld[r2lstart+nj];
                                int l2r = lu2rd[l2rstart-nj];
                                if(u2d+r2l+l2r == 0){
                                        board[i] = nj;
                                        ru2ld[r2lstart+nj]++;
                                        lu2rd[l2rstart-nj]++;
                                        // up2down.erase(it);
                                        up2down[nj]++;
                                        flag = false;
                                        break;
                                }
                                if(u2d+r2l+l2r < min){
                                        min = u2d + r2l+l2r;
                                        kouho = nj;
                                }
                        }
                        if(flag){
                                board[i] = kouho;
                                ru2ld[r2lstart+kouho]++;
                                lu2rd[l2rstart-kouho]++;
                                conflictcandidate.insert(i);
                                conflictindex.insert(i);
                        }
                }
                //add to conflict list
                for(auto it = conflictcandidate.begin();it!=conflictcandidate.end();it++){
                        // cout << *it << endl;
                        for(int i=0;i<size;i++){
                                if(*it==i)
                                        continue;
                                int diff = abs(i-*it);
                                if(board[*it]==board[i] || board[*it]+diff == board[i] || board[*it]-diff == board[i]){
                                        // cout << *it << " " << i << endl;
                                        if(conflictindex.find(i) == conflictindex.end()){
                                                conflictindex.insert(i);
                                        }
                                }
                        }
                }
                cout << "conflict number " << conflictindex.size() << endl;
                conflictcandidate.clear();
                free(up2down);
                free(ru2ld);
                free(lu2rd);
        }
        int size;
        int* board;
        unordered_set<int> conflictindex;
        unordered_set<int> conflictcandidate;
        bool isSolution(){
                for(auto it = conflictcandidate.begin();it!=conflictcandidate.end();it++){
                        for(int i=0;i<size;i++){
                                if(*it==i)
                                        continue;
                                int diff = abs(i-*it);
                                if(board[*it]==board[i] || board[*it]+diff == board[i] || board[*it]-diff == board[i]){
                                        if(conflictindex.find(*it) == conflictindex.end()){
                                                conflictindex.insert(*it);
                                        }
                                }
                        }
                }
                conflictcandidate.clear();
                if(conflictindex.empty())
                        return true;
                return false;
        }
        void printBoard(){
                for(int i=0;i<size;i++){
                        for(int j=0;j<size;j++){
                                if(board[i]==j)
                                        cout << "*";
                                else
                                        cout << "-";
                        }
                        cout << endl;
                }
                cout << endl;
        }
};

int conflict(nqueen& state,int var){
        int diff,num;
        varBinaryHeap bh;
        for(int i=0;i<state.size;i++){
                bh.add(make_pair(i,0));
        }
        for(int i=0;i<state.size;i++){
                if(var == i)
                        continue;
                num = bh.find(state.board[i]);
                bh.a[num].second++;
                bh.trickleDown(num);
                diff = abs(var-i);
                if(state.board[i]+diff < state.size){
                        num = bh.find(state.board[i]+diff);
                        bh.a[num].second++;
                        bh.trickleDown(num);
                }
                if(0<= state.board[i]-diff){
                        num = bh.find(state.board[i]-diff);
                        bh.a[num].second++;
                        bh.trickleDown(num);
                }
        }
        vector<pair<int,int> > mins;
        pair<int,int> min = bh.findMin();
        bh.remove();
        mins.push_back(min);
        while(bh.findMin().second == mins[0].second){
                mins.push_back(bh.findMin());
                bh.remove();
        }
        random_device rnd;
        int x = rnd()%mins.size();
        return mins[x].first;
}

void eraseconflict(nqueen& state,int var, int value){
        state.conflictindex.erase(var);
        // state.conflictcandidate.insert(var);
        for(int i=0;i<state.size;i++){
                if(var==i)
                        continue;
                int diff = i-var;
                if(value==state.board[i] || value+diff == state.board[i] || value-diff == state.board[i]){
                        state.conflictindex.erase(i);
                        state.conflictcandidate.insert(i);
                }
        }
        return;
}

void addconflict(nqueen& state, int var,int value){
        for(int i=0;i<state.size;i++){
                if(var==i)
                        continue;
                int diff = i-var;
                if(value==state.board[i] || value+diff == state.board[i] || value-diff == state.board[i]){
                        if(state.conflictindex.find(i) == state.conflictindex.end()){
                                state.conflictcandidate.insert(i);
                                state.conflictcandidate.insert(var);
                        }
                }
        }
        return;
}

bool minconflict(nqueen& state,int maxsteps){
        for(int i=0;i<maxsteps;i++){
                // cout << i <<endl;
                // state.printBoard();
                if(state.isSolution()){
                        cout << i << "steps" << endl;
                        // state.printBoard();
                        return true;
                }
                // cout << " " << state.conflictindex.size() << endl <<endl;
                random_device rnd;
                //conflictを起こしてるやつをrandom抽出
                // int var = state.conflictlist[rnd()%state.conflictlist.size()];
                int x = rnd()%state.conflictindex.size();
                auto it = state.conflictindex.begin();
                for(int i=0;i<x;i++){
                        it++;
                }
                int var = *it;
                //conflictが一番少なくなる位置へ移動
                int value = conflict(state,var);
                eraseconflict(state,var,state.board[var]);
                state.board[var] = value;
                addconflict(state,var,value);
        }
        return false;
}

int main(){
        int n;
        cin >> n;
        struct timeval start,goal,ongoing;
        gettimeofday(&start,NULL);
        nqueen init(n);
        gettimeofday(&ongoing,NULL);
        printTime(start,ongoing);
        minconflict(init,100000);
        gettimeofday(&goal,NULL);
        printTime(ongoing,goal);
}
