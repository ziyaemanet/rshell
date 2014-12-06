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
#include <signal.h>
#include <limits.h>
#include <ctype.h>

using namespace std;

string strtokPrep(string);
int callProg(char**);
void sig_handler(int);

deque<string> progQ;
deque<int> pidQ;

deque<int>::iterator pidItr = pidQ.begin();
deque<string>::iterator progItr = progQ.begin();

string currProg = "";

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

    if(signal(SIGINT,sig_handler) == SIG_ERR){
        perror("signal");
    }
    
    if(signal(SIGTSTP,sig_handler) == SIG_ERR){
        perror("signal");
    }
   
    bool runMain = true;
    
    while(runMain){
        string usrInput = "";
        

        char bufDir[PATH_MAX+1];

        if(getcwd(bufDir,PATH_MAX) == NULL){
            perror("getcwd");
        }

        cout << userName << "@" << hostName << ":" << bufDir << "$ ";
        getline(cin,usrInput);
        
        if(usrInput.empty()){
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

        delete[] cInput;

        int numTerms = 0;
        string chkInp;
        int delEnd = 0;
        deque<string>::iterator itr = inputQ.begin();
        bool cont = true;
        bool cdChk = false;

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

                pidItr = pidQ.begin();
                progItr = progQ.begin();

                for(;pidItr != pidQ.end();pidItr++){
                     
                    int killPid = *pidItr;
                    string killProg = *progItr;
                    cout << killProg << " " <<  killPid << endl;

                    if(kill(killPid,SIGKILL) == -1){
                        perror("kill");
                    }

                    progItr++;
                }
    
 

                runMain = false;
                cont = false;
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
            else if(chkInp == "cd"){
                cdChk = true;
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
           
                    

                for(int i = 0;i < numTerms+1; i++){
                    if(i < numTerms){
                        argv[i] = (char*)malloc(inputQ[i].size()+1);
                        strcpy(argv[i],inputQ[i].c_str());
                    }
                    else{
                        argv[i] = '\0';
                    }
                }
                
                for(int j = 0; j < numTerms + delEnd; j++){
                    inputQ.pop_front();
                }
    
                if(cdChk){
                    cdChk = false;

                    if(chdir(argv[1]) != 0){
                        perror("chdir");
                    }
                    
                    for(int i = 0;i < numTerms+1;i++){
                        if(i<numTerms){
                            free((void*)argv[i]);
                        }
                        else{
                            delete argv[i];
                        }
                    }
                  
                    itr = inputQ.begin();
                    break;
                }
                
                string fgStrChk = "fg";
                string isFgBg = string(argv[0]);
                string bgStrChk = "bg";
                
                
                if(fgStrChk.compare(isFgBg) == 0 || bgStrChk.compare(isFgBg) == 0){
                    if(!pidQ.empty()){
                        if(argv[1] != NULL){
                            string numChk = string(argv[1]);
                            char toInt[10];
                            strcpy(toInt,numChk.c_str());

                            bool isValid = true;

                            for(int i = 0;toInt[i] != '\0'; i++){
                                if(!isdigit(toInt[i])){
                                    isValid = false;
                                }
                            }
                            
                            if(isValid){
                                
                                int goToProc = atoi(numChk.c_str());
                                int pidQsize = pidQ.size();

                                if(goToProc <= pidQsize){
                                    
                                    pidItr = pidQ.begin();
                                    progItr = progQ.begin();
                                    
                                    for(int i = 0; i < goToProc-1;i++){
                                        progItr++;
                                        pidItr++;
                                    }
                                }
                            
                            }
                        
                        }
                
                        int statusFg;
                        int killPid = *pidItr;
                        string killProg = *progItr;
                    
                        if(kill(killPid,SIGCONT) == -1){
                            perror("kill");
                        }

                        cout << killProg << endl;

                        
                        if(fgStrChk.compare(isFgBg) == 0){
                            if(waitpid(killPid,&statusFg,WUNTRACED) == -1){
                                perror("wait");
                            }

                            if(WIFEXITED(statusFg) || WIFSIGNALED(statusFg) || WIFSTOPPED(statusFg)){
                                pidQ.pop_back();
                                progQ.pop_back();
                                pidItr = pidQ.begin();
                                progItr = progQ.begin();
                            }
                        }
                    }

                    for(int i = 0;i < numTerms+1;i++){
                        if(i<numTerms){
                            free((void*)argv[i]);
                        }
                        else{
                            delete argv[i];
                        }
                    }

                    cont = false;
                    break;
                }


                int pid = fork();    
                int fd[2];
                int pipeChk = 0;
                
                currProg = string(argv[0]);

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
                    
                    callProg(argv);
                    exit(0); 
	            }
	            else{
                    int status;
                    
                    for(int i = 0;i < numTerms+1;i++){
                        if(i<numTerms){
                            free((void*)argv[i]);
                        }
                        else{
                            delete argv[i];
                        }
                    }
                   
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
                    

                    int eWait = waitpid(pid,&status,WUNTRACED);

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
                   

                    if(WIFSTOPPED(status)){
                        if(WSTOPSIG(status) == SIGTSTP){        
                            
                            deque<int>::iterator pidItrSave = pidItr;
                            deque<string>::iterator progItrSave = progItr;

                            bool pidExist = false;
                            pidItr = pidQ.begin();
                            progItr = progQ.begin();

                            for(;pidItr != pidQ.end();pidItr++){
            
                                int pidChk = *pidItr;
            
                                if(pidChk == pid){
                                    pidExist = true;
                                }

                                progItr++;
                            }
    
                            if(!pidExist){
                                pidQ.push_back(pid);
                                progQ.push_back(currProg);
                            }
                            else{
                                pidItr = pidItrSave;
                                progItr = progItrSave;
                            }
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
                currProg = "";
            }
        }
    }

    return EXIT_SUCCESS;
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

int callProg(char* argv[]){
    char* pathStr;
    char* str = getenv("PATH");
    
    pathStr = strtok(str,":");

    while(pathStr != NULL){
        
        char progCall[250];
        char progName[100];
        
        strcpy(progCall,pathStr);
        strcpy(progName,argv[0]);

        strcat(progCall,"/");
        strcat(progCall,progName);
        
        execv(progCall,argv);
        
        pathStr = strtok(NULL,":");
    }
    
    perror("execv");

    return -1;
}

void sig_handler(int signum){

}

