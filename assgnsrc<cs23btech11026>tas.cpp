#include<bits/stdc++.h>     
#include<mutex> 
#include<chrono>                               //tas
using namespace std;
using namespace chrono;
int ** a;
int k,n,t;
pthread_t* th;
mutex m;
vector<string> vec;

vector<long long> access_time;
vector<long long> cs_time;
struct s_arg{
    int num;
    int* pp;
};
int p=0; int termi=0; 
atomic<int> u=1;                                                  //shared variable

int check_row(int x){
    int v[n];
    for(int i=0;i<n;i++){
        v[i]=1;
    }
    for(int i=0;i<n;i++){
        v[a[x][i]-1]=0;
    }
    int xx=0;
    for(int i=0;i<n;i++){
        xx+=v[i];
    }
    return !xx;
}
int check_col(int x){
    int v[n];
    for(int i=0;i<n;i++){
        v[i]=1;
    }
    for(int i=0;i<n;i++){
        v[a[i][x]-1]=0;
    }
    int xx=0;
    for(int i=0;i<n;i++){
        xx+=v[i];
    }
    return !xx;
}
int check_grid(int x){
    int v[n];
    for(int i=0;i<n;i++){
        v[i]=1;
    }
    int xy=1;
     while(xy){
          if(xy*xy==n)break;
          xy++;
     }
    int nn=xy;
    int xx=(x%nn)*nn,yy=(x/nn)*nn;
    for(int i=yy;i<yy+nn;i++){
        for(int j=xx;j<xx+nn;j++){
            v[a[i][j]-1]=0;
        }
    }
    int xxx=0;
    for(int i=0;i<n;i++){
        xxx+=v[i];
    }
    return !xxx;

}


void* f(void* arg){
    
    s_arg* x=(s_arg*)arg;
    while(p<3*n&&!termi){
    auto start = high_resolution_clock::now();
    //thread requests
    time_t now = time(0);
    tm* ltm = localtime(&now);
    string st="Thread "+ to_string(x->num)+" requests to enter CS at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs" ;
    m.lock();
    vec.push_back(st);
    
    m.unlock();
    int nav;
    while(!u.exchange(0,memory_order_acquire));
    //thread gets access
    auto stop1= high_resolution_clock::now();
    auto d1 = duration_cast<microseconds>(stop1 - start);
    now = time(0);
    ltm = localtime(&now);
    st="Thread "+ to_string(x->num)+" enters CS at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs" ;
    m.lock();
    vec.push_back(st);
    access_time.push_back(d1.count());
    m.unlock();
    //critical section

    nav=p;
    if(3*n-p>t)p+=t;
    else p=3*n;
    auto stop2= high_resolution_clock::now();
    auto d2 = duration_cast<microseconds>(stop2 - stop1);
    now = time(0);
    ltm = localtime(&now);
    st="Thread "+ to_string(x->num)+" exits CS at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs" ;
    m.lock();
    vec.push_back(st);
    cs_time.push_back(d2.count());
    access_time.push_back(d1.count());
    m.unlock();

    u.store(1,memory_order_release);
    //exit critcal section
    for(int i=nav;i<nav+t&&i<3*n;i++){
        if((i)/n==0){
            //grabs a row
            time_t now = time(0);
            tm* ltm = localtime(&now);
            string st="Thread "+to_string(x->num)+" grabs row "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs" ;
            m.lock();
            vec.push_back(st);
            m.unlock();
            if(!check_row(i%n)){
                termi=1;
                string st="Thread "+to_string(x->num)+" invalidates row "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs";
                m.lock();
            vec.push_back(st);
            m.unlock();
                break;
            }
            else{
                now = time(0);
                string st="Thread "+to_string(x->num)+" validates row "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs";
                m.lock();
            vec.push_back(st);
            m.unlock();
            }
            //validates a row
        }
        else if((i)/n==1){
            time_t now = time(0);
            tm* ltm = localtime(&now);
            string st="Thread "+to_string(x->num)+" grabs column "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs";
            
            m.lock();
            vec.push_back(st);
            m.unlock();
            if(!check_col(i%n)){
                termi=1;
                now = time(0);
                string st="Thread "+to_string(x->num)+" invalidates column "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" +to_string(ltm->tm_min) +" hrs";
                m.lock();
            vec.push_back(st);
            m.unlock();
                break;
                
            }
            else{
                now = time(0);
                string st="Thread "+to_string(x->num)+" validates column "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" +to_string(ltm->tm_min) +" hrs";
                m.lock();
            vec.push_back(st);
            m.unlock();
            }
        }
        else if((i)/n==2){
            time_t now = time(0);
            tm* ltm = localtime(&now);
            string st="Thread "+to_string(x->num)+" grabs grid "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs";
            m.lock();
            vec.push_back(st);
            m.unlock();
            if(!check_grid(i%n)){
                termi=1;
                now = time(0);
                string st="Thread "+to_string(x->num)+" invalidates grid "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs";
                m.lock();
            vec.push_back(st);
            m.unlock();
                break;
            }else{
                now = time(0);
                string st="Thread "+to_string(x->num)+" validates grid "+to_string(i%n+1)+" at "+to_string(ltm->tm_hour)+":" + to_string(ltm->tm_min)+" hrs";
                m.lock();
            vec.push_back(st);
            m.unlock();
            }
        }

    }
    }pthread_exit(NULL);
}

int main(){
    string ff;
    ff="inp.txt";
    ifstream file(ff);
    ofstream f1("output.txt");    
    file>>k>>n>>t;
    a=new int*[n];
    for(int i=0;i<n;i++){
        a[i]=new int[n];
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            file>>a[i][j];
        }
    }
    auto stt=high_resolution_clock::now();
    th =new pthread_t[k];
    s_arg s[k];
    for(int i=0;i<k;i++){
        s[i]={i+1,&p};
    }
    for(int i=0;i<k;i++){
       
        pthread_create(th+i,NULL,&f,(void*)&s[i]);
    }
    for(int i=0;i<k;i++){
        pthread_join(th[i],NULL);
   }auto stp=high_resolution_clock::now();
   auto d4 = duration_cast<microseconds>(stp - stt);
   for(string x:vec)f1<<x<<endl;
    if(!termi)f1<<"Sudoko is valid\n";
    else f1<<"Sudoko is invalid\n";
    f1<<"The total time taken is "<<d4.count()<<" microseconds.\n";
    int av=0;
    for(int i=0;i<access_time.size();i++){
        av+=access_time[i];
    }
    f1<<"Average time taken by a thread to enter the CS is "<<av/access_time.size()<<" microseconds\n";
    av=0;
    for(int i=0;i<cs_time.size();i++){
        av+=cs_time[i];
    }
    f1<<"Average time taken by a thread to exit the CS is "<<av/cs_time.size()<<" microseconds\n";
    int max=0;
    for(int i=0;i<access_time.size();i++){
        if(max<access_time[i]){
            max=access_time[i];
        }
    }
    f1<<"Worst-case time taken by a thread to enter the CS is "<<max<<" microseconds\n";
    max=0;
    for(int i=0;i<cs_time.size();i++){
        if(max<cs_time[i]){
            max=cs_time[i];
        }
    }
    f1<<"Worst-case time taken by a thread to exit the CS is "<<max<<" microseconds\n";
}