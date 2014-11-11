//Ziya Emanet
//A recreation of the ls command

#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[]){
    

     bool aChk;
     bool lChk;
     bool RChk;
     int dirCount = 0;
     const char* dirName;

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
            //dirName = argv[i];
            //dirCount++;
        }

     }    
     
     cout << "START" << endl;
     cout << aChk << endl;
     cout << lChk << endl;
     cout << RChk << endl;
           
       


        
     const char* dirName = ".";
     
     DIR* dirp = opendir(dirName);
     
     if(dirp == NULL){
        perror("opendir");
        return -1;
     }
    
     dirent* direntp;
     
     while ((direntp = readdir(dirp))){
         
         if(direntp->d_name[0] == '.'){

         }
         else{
            cout << direntp->d_name << "   ";
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

     int closeChk = closedir(dirp);
     
     if(closeChk == -1){
        perror("closedir");
     }


    return 0;
}
