// ZIYA EMANET
// rShell: a basic shell

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
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
    
    string usrInput;

    cout << userName  << "@" << hostName << "$ ";
    getline(cin,usrInput);
    //perror("getline"); error check getline
   
    usrInput = strtokPrep(usrInput);
    
    char* cInput = new char[usrInput.length()+1];
    strcpy(cInput,usrInput.c_str());
    char* grab = strtok(cInput," ");
    
    deque<string> inputQ;
    
    while(grab != NULL){
        //argv[numTerms] = new char[10];
        //argv[numTerms] = grab;
        //cout << grab << endl;
        inputQ.push_back(string(grab));
        grab = strtok(NULL," ");
    }
 
    int numTerms;
    string chkInp;
    int delEnd = 0;

    for(deque<string>::iterator itr = inputQ.begin(); itr != inputQ.end(); itr++){
        //cout << *itr << endl;
        numTerms++;
        chkInp = *itr;
        bool conChk;

        if(chkInp == ";" || chkInp == "||" || chkInp == "&&"){
            conChk = true;

        }
        else{
            conChk = false;
        }

        if(numTerms == inputQ.size() || conChk){
            cout << "numterms: " << numTerms << endl;
            char* argv[numTerms+1];

            if(conChk){
                numTerms--;
                delEnd = 1;
            }
            
            for(int i = 0;i < numTerms; i++){
                argv[i] = new char[inputQ[i].size()+1];
                strcpy(argv[i],inputQ[i].c_str());
                //cout << argv[i] << endl;
            }
            
            argv[numTerms] = NULL;
          
            for(int j = 0; j < numTerms + delEnd; j++){
                inputQ.pop_front();
            }

            int pid = fork();
	
	        if(pid == -1){
		        perror("fork");
	        }
	        else if(pid == 0){
		        execvp(argv[0],argv);
		        perror("execvp");
	        }
	        else{
	    	    int eWait = wait(NULL);
		
		        if(eWait == -1){
			        perror("wait");
		        }   
	        }
            
            //cout << "0: " << argv[0] << endl;
            //cout << "1: " << argv[1] << endl;
            //cout << "2: " << argv[2] << endl;
            //cout << "3: " << argv[3] << endl;
            //cout << "4: " << argv[4] << endl;
            //cout << "5: " << argv[5] << endl;
            //cout << "6: " << argv[6] << endl;
            //cout << "7: " << argv[7] << endl;
            //cout << "8: " << argv[8] << endl;
            //cout << "9: " << argv[9] << endl;
            cout << "size: " << inputQ.size() << endl;

            for(int k = 0; k < inputQ.size(); k++){
                cout << inputQ[k] << endl;
            
            }

            //itr = inputQ.begin();
        }

    }


    return 0;
}

string strtokPrep(string inString){
    string currChar;
    //inString += " ";

    for(int i = 0; i < inString.length();i++){
        currChar = inString[i];
        if(currChar.compare(";") == 0){
            inString.insert(i," ");
            inString.insert(i+2," ");
            i += 2;
        }

        if(currChar.compare("|") == 0){
           if((i+2) <= inString.length()){
                string afterChar; 
                afterChar = inString[i+1];
                if(afterChar.compare("|") == 0){
                    inString.insert(i," ");
                    inString.insert(i+3," ");
                    i += 2;
                }
                else{
                    cout << "*ERROR using connector ||*" << endl;
                }
           }
           else{
               cout << "*ERROR using connector ||*" << endl;
           }
        }
        
        if(currChar.compare("&") == 0){
           if((i+2) <= inString.length()){
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
    }

    /*
    for(int i = 0; i < inString.length();i++){
        cout << i << ": '" << inString[i] << "'" << endl;
    }
    */

    return inString;
}

