//Ziya Emanet
//A recreation of the ls command

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <deque>
#include <time.h>
#include <pwd.h>

using namespace std;

int procDir(const char*,bool,bool,bool);



int main(int argc, char* argv[]){
    

     bool aChk = false;
     bool lChk = false;
     bool RChk = false;
     deque<string> inputQ;


     for(int i = 1; i < argc; i++){

        if(argv[i][0] == '-'){
            
            for(int j = 1; argv[i][j] != '\0'; j++){
                
                if(argv[i][j] == 'a' && !aChk){
                    aChk = true;
                }
                else if(argv[i][j] == 'l' && !lChk){
                    lChk = true;
                }
                else if(argv[i][j] == 'R' && !RChk){
                    RChk = true;
                }
                else{
                    cout << "*ERROR* : undefined parameters detected : " << argv[i] << endl;
                    return -1;
                }
            
            }
        
        }
        else{
            inputQ.push_back(argv[i]);
        }

     }    
     
     /*
     cout << "START" << endl;
     cout << aChk << endl;
     cout << lChk << endl;
     cout << RChk << endl;
     */      
     
     deque<string>::iterator itr = inputQ.begin();

     if(inputQ.size() > 0){

         for(; itr != inputQ.end(); itr++){
            
            cout << "\n" << *itr << ":" << endl;
             
            string grabInput = *itr;
            char* sendInput = new char[grabInput.length()+1];
           
            strcpy(sendInput,grabInput.c_str());
            procDir(sendInput,lChk,aChk,RChk); 
         
         }
     }
     else{
        inputQ.push_back(".");
 
        for(; itr != inputQ.end(); itr++){
           
           cout << "\n" << *itr << ":" << endl;
           
           string grabInput = *itr;
           char* sendInput = new char[grabInput.length()+1];
           
           strcpy(sendInput,grabInput.c_str());
           procDir(sendInput,lChk,aChk,RChk); 
        
        }
        
     }

     return 0;
}


int procDir(const char* dirName, bool lChk, bool aChk, bool RChk){

     int dirTotal = 0;   

     DIR* dirp = opendir(dirName);
     
     if(dirp == NULL){
        perror("opendir");
        return -1;
     }
    
     dirent* direntp;
     
     while ((direntp = readdir(dirp))){
        
        if( !(direntp->d_name[0] == '.') ){                  
            
            if(lChk){
                struct stat buffer;

                int status = lstat(direntp->d_name,&buffer);
                
                if(status != 0){
                    perror("lstat");
                }
                
                if(S_ISDIR(buffer.st_mode)){
                    cout << "d";
                } 
                else{
                
                    cout << "-";
                
                }
                
                /**************************************** OWNER PERMISSIONS */ 
                
                if(buffer.st_mode & S_IRUSR){
                    cout << "r";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IWUSR){
                    cout << "w";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IXUSR){
                    cout << "x";
                } 
                else{
                
                    cout << "-";
                }
 
                /**************************************** GROUP PERMISSIONS */ 
                
                if(buffer.st_mode & S_IRGRP){
                    cout << "r";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IWGRP){
                    cout << "w";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IXGRP){
                    cout << "x";
                } 
                else{
                
                    cout << "-";
                }
 
                /**************************************** WORLD PERMISSIONS */ 
                
                if(buffer.st_mode & S_IROTH){
                    cout << "r";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IWOTH){
                    cout << "w";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IXOTH){
                    cout << "x";
                } 
                else{
                
                    cout << "-";
                }

                cout << " " << buffer.st_nlink;

                struct passwd* usrInfo;
                errno = 0;
                usrInfo = getpwuid(buffer.st_uid);
                
                if(usrInfo == NULL){
                    
                    if(errno != 0){
                        perror("getpwuid");
                        return -1;
                    }
                    
                    cout << " Unknown";
                }
                else{
                    
                    cout << " " << usrInfo->pw_name;
                }
               

                struct passwd* grpInfo;
                grpInfo = getpwuid(buffer.st_gid);
                
                if(grpInfo == NULL){
                    if(errno != 0){
                        perror("getpwuid");
                        return -1;
                    }

                    cout << " Unknown";
                }
                else{
                    cout << " " << grpInfo->pw_name;
                
                }

                cout << " " << buffer.st_size;

                char* timeStat = ctime(&(buffer.st_ctime));
                char timeOut[19]; 
               
                for(int k = 4;k < 16; k++){
                    
                    timeOut[k-4] = timeStat[k];
                    
                    if(k == 15){
                        timeOut[12] = '\0';  
                    }
                
                }
                

                cout << " " << timeOut;

                cout << " " << direntp->d_name << endl;
                    
                dirTotal += buffer.st_blocks;    
            }
            else{
                
                cout << direntp->d_name << "  ";
            
            }

        }
        else if(aChk){
          
            if(lChk){
                struct stat buffer;

                int status = lstat(direntp->d_name,&buffer);
                
                if(status != 0){
                    perror("lstat");
                }
                
                if(S_ISDIR(buffer.st_mode)){
                    cout << "d";
                } 
                else{
                
                    cout << "-";
                
                }
                
                /**************************************** OWNER PERMISSIONS */ 
                
                if(buffer.st_mode & S_IRUSR){
                    cout << "r";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IWUSR){
                    cout << "w";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IXUSR){
                    cout << "x";
                } 
                else{
                
                    cout << "-";
                }
 
                /**************************************** GROUP PERMISSIONS */ 
                
                if(buffer.st_mode & S_IRGRP){
                    cout << "r";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IWGRP){
                    cout << "w";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IXGRP){
                    cout << "x";
                } 
                else{
                
                    cout << "-";
                }
 
                /**************************************** WORLD PERMISSIONS */ 
                
                if(buffer.st_mode & S_IROTH){
                    cout << "r";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IWOTH){
                    cout << "w";
                } 
                else{
                
                    cout << "-";
                }

 
                if(buffer.st_mode & S_IXOTH){
                    cout << "x";
                } 
                else{
                
                    cout << "-";
                }

                cout << " " << buffer.st_nlink;

                struct passwd* usrInfo;
                errno = 0;
                usrInfo = getpwuid(buffer.st_uid);
                
                if(usrInfo == NULL){
                    
                    if(errno != 0){
                        perror("getpwuid");
                        return -1;
                    }
                    
                    cout << " Unknown";
                }
                else{
                    
                    cout << " " << usrInfo->pw_name;
                }
               

                struct passwd* grpInfo;
                grpInfo = getpwuid(buffer.st_gid);
                
                if(grpInfo == NULL){
                    if(errno != 0){
                        perror("getpwuid");
                        return -1;
                    }

                    cout << " Unknown";
                }
                else{
                    cout << " " << grpInfo->pw_name;
                
                }

                cout << " " << buffer.st_size;

                char* timeStat = ctime(&(buffer.st_ctime));
                char timeOut[12]; 
               
                for(int k = 4;k < 16; k++){
                    
                    timeOut[k-4] = timeStat[k];
                    
                    if(k == 15){
                        timeOut[12] = '\0';  
                    }
                
                }
                

                cout << " " << timeOut;

                cout << " " << direntp->d_name << endl;
                
                dirTotal += buffer.st_blocks;    

            }
            else{
                
                cout << direntp->d_name << "  ";
            
            }
        }   
     }
      
     if(direntp == NULL){
        int errChk = errno;
        cout << endl;

        if(errChk != 0){
            perror("readdir");
            return -1;
        }
     }

     if(lChk){
        dirTotal = dirTotal/2;   
        cout << "total " << dirTotal << endl;
     }

     int closeChk = closedir(dirp);
     
     if(closeChk == -1){
        perror("closedir");
     }


    return 0;
}
