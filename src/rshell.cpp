// ZIYA EMANET
// rshell: a basic shell

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <deque>

using namespace std;

string strtokPrep(string);

int main(){

    char hostName[65];
    char userName[33];
    int eUser = getlogin_r(userName,sizeof(userName));
    int eHost = gethostname(hostName,sizeof(hostName));
    
    if(eHost == -1){
        perror("gethostname");
    }
    if(eUser != 0){
        perror("getlogin_r");
    }

    bool runMain = true;
    
    while(runMain){
        string usrInput = "";

        cout << userName  << "@" << hostName << "$ ";
        getline(cin,usrInput);
  
        if(usrInput == "exit"){
           runMain = false;
           break;
        }

        usrInput = strtokPrep(usrInput);
    
        char* cInput = new char[usrInput.length()+1];
        strcpy(cInput,usrInput.c_str());
        char* grab = strtok(cInput," ");
    
        deque<string> inputQ;
    
        while(grab != NULL){
            inputQ.push_back(string(grab));
            grab = strtok(NULL," ");
        }
 
        int numTerms = 0;
        string chkInp;
        int delEnd = 0;
        deque<string>::iterator itr = inputQ.begin();
        bool cont = true;
 
        while(cont){
            bool ampCon = false;    
            bool pipeCon = false;
            
            numTerms++;
            chkInp = *itr;
            bool conChk;
            bool run = false;
            
            bool inRedir = false;
            bool outRedir = false;
            bool outAppRedir = false;
            bool piping = false;

            if(chkInp == "exit"){
                runMain = false;
                break;
            }

            if(chkInp == ";" || chkInp == "||" || chkInp == "&&"){
                conChk = true;
                if(chkInp == "&&"){
                    ampCon = true;
                
                }
                else if(chkInp == "||"){
                    pipeCon = true;
                }
            }
            else{
                conChk = false;
            }


            if(chkInp == "<"){
                inRedir = true;
                conChk = true;
            }
            else if(chkInp == ">"){
                outRedir = true;
                conChk = true;
            }
            else if(chkInp == ">>"){
                outAppRedir = true;
                conChk = true;
            }
            else if(chkInp == "|"){
                piping = true;
                conChk = true;
            }
            
            int inputQsize = inputQ.size();
            if(numTerms == inputQsize || conChk){
                char* argv[999];
            
                if(numTerms == inputQsize){
               
                    if(conChk && chkInp != ";"){
                        cout << "*ERROR last command not run, check connector or redirection useage*" << endl;
                        break;
                    }
                    cont =  false;
                }

                if(conChk){
                    numTerms--;
                    delEnd = 1;
                }
           
                    

                for(int i = 0;i < numTerms; i++){
                    argv[i] = new char[inputQ[i].size()+1];
                    strcpy(argv[i],inputQ[i].c_str());
                }
                
                argv[numTerms] = '\0';
          
                for(int j = 0; j < numTerms + delEnd; j++){
                    inputQ.pop_front();
                }

                

                int pid = fork();    
                int fd[2];
                int pipeChk = 0;
                
                if(piping){
                    pipeChk = pipe(fd);
                }
                
                if(pipeChk == -1){
                    perror("pipe");
                }

	            if(pid == -1){
		            perror("fork");
	            }
	            else if(pid == 0){
                    if(outRedir || outAppRedir){
                        itr++;
                        string outFile = *itr;
                        if(outFile.empty()){
                            cout << "*ERROR missing outfile*" << endl;
                        }
                        else{
                            int fdOut = 0;

                            if(outRedir){
                                fdOut = open(outFile.c_str(),O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR); 

                            }
                            else{
                                
                                fdOut = open(outFile.c_str(),O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR); 
                            }
                            
                            if(fdOut == -1){
                                perror("open");
                            }
                            else{
                                int dupChk = dup2(fdOut,1);
                                
                                if(dupChk == -1){
                                    perror("dup2");
                                }
                            }
                            
                            int clsChk = close(fdOut);
                            
                            if(clsChk == -1){
                                perror("close");
                            }
                        }
                    }
                    else if(inRedir){
                        itr++;
                        string inFile = *itr;
                        if(inFile.empty()){
                             cout << "*ERROR missing infile*" << endl;
                        }
                        else{
                            int fdIn = open(inFile.c_str(),O_RDONLY); 

                            if(fdIn == -1){
                                perror("open");
                            }
                            else{
                                int dupChk = dup2(fdIn,0);
                                
                                if(dupChk == -1){
                                    perror("dup2");
                                }
                            }
                            
                            int clsChk = close(fdIn);
                            
                            if(clsChk == -1){
                                perror("close");
                            
                            }
                        }
                    }
                    else if(piping){
                        int dupChk = dup2(fd[1],1);

                        if(dupChk == -1){
                            perror("dup2");
                        }
                        
                        int clsChk = close(fd[0]);

                        if(clsChk == -1){
                            perror("close");
                        }
                    
                    }
                    
                    execvp(argv[0],argv);
		            perror("execvp");
	            }
	            else{
                    int status;
                    
                    if(outRedir || outAppRedir || inRedir){
                        inputQ.pop_front();
                        if(inputQ.size() == 0){
                            break;
                        }
                    }

                    int stdIn;
                    
                    if(piping){
                        stdIn = dup(0);
                        if(stdIn == -1){
                            perror("dup");
                        }
                        
                        int dupChk = dup2(fd[0],0);
                        if(dupChk == -1){
                            perror("dup2");
                        }

                        int clsChk = close(fd[1]);
                        if(clsChk == -1){
                            perror("close");
                        }
                    }
 
                    int eWait = wait(&status);

                    if(eWait == -1){
			            perror("wait");
		            }
                                       
                    if(piping){
                        int pipeForkPid = fork();

                        if(pipeForkPid == -1){
                            perror("fork");
                        }
                        else if(pipeForkPid == 0){ //child
                            itr++;
                            string nextPipe = *itr;
                            unsigned int pipeTerms = 0;

                            while(!nextPipe.compare("|") ||  pipeTerms < inputQ.size()){
                                pipeTerms++;
                                itr++;
                                nextPipe = *itr;
                            }
                                                
                            char* argw[999];
                            
                            
                            for(unsigned int i = 0;i < pipeTerms; i++){
                                argw[i] = new char[inputQ[i].size()+1];
                                strcpy(argw[i],inputQ[i].c_str());
                            }
                             
                            argw[pipeTerms] = '\0';
   
                            execvp(argw[0],argw);
		                    perror("execvp");
                            
                        }
                        else{ //parent
                            int eWaitPipe = wait(&status);
                            if(eWaitPipe == -1){
                                perror("wait");
                            }
                        }
                        
                        int dupChk = dup2(stdIn,0);
                        
                        if(dupChk == -1){
                            perror("dup2");
                        }
                   }

                    if(WIFEXITED(status)){
                        if(WEXITSTATUS(status) == 0){
                            if(pipeCon){
                                cont =  false;
                                break;
                            }
                        }
                        else{
                            if(ampCon){
                                cont = false;
                                break;
                            }
                        }
                    }
	            }
            
                itr = inputQ.begin();
                run = true;
                delEnd = 0;
                numTerms = 0;

            }

            if(!run){
                itr++;
            }
        }
    }

    return 0;
}

string strtokPrep(string inString){
    string currChar;
    inString += " ";

    int strLen = inString.length();
    for(int i = 0; i < strLen;i++){
        currChar = inString[i];
    
        if(currChar.compare(";") == 0){
            inString.insert(i," ");
            inString.insert(i+2," ");
            i += 2;
        }

        if(currChar.compare("|") == 0){
           int chkLen = inString.length();
           if((i+2) <= chkLen){
                string afterChar; 
                afterChar = inString[i+1];
                if(afterChar.compare("|") == 0){
                    inString.insert(i," ");
                    inString.insert(i+3," ");
                    i += 2;
                }
           }
        }
        
        if(currChar.compare("&") == 0){
           int chkLen = inString.length();
           if((i+2) <= chkLen){
                string afterChar; 
                afterChar = inString[i+1];
                if(afterChar.compare("&") == 0){
                    inString.insert(i," ");
                    inString.insert(i+3," ");
                    i += 2;
                }
                else{
                    cout << "*ERROR using connector &&*" << endl;
                }
           }
           else{
               cout << "*ERROR using connector &&*" << endl;
           }
        
        }

        if(currChar.compare("#") == 0){
            inString.erase(i,-1);
            break;
        }
        strLen = inString.length();
    }
    return inString;
}

