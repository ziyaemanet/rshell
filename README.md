RSHELL: 
    A basic shell attempting to recreate terminal.  

FILE LIST:
    rshell.cpp

RUNNING:
    cd to /rshell
    make
    bin/rshell

BUGS/KNOWN ISSUE/COMMENTS:
    
    1. Does not show username when script is run before rshell.
    
    2. If a connector stops the running of the connecting command any commands placed after ; will
       not run. EXs where ls will not run: (TRUE || pwd ; ls) (FALSE && pwd ; ls) and more. 
    
    3. If connectors other than ; are used at the end, rshell will notify of improper usage.

    4. Exit command may fail at times and need to be typed in more than once.

    5. Will abort if enter is pressed excessively on empty prompt.

    6. Entering only a comment will cause a seg fault, comments must be placed after input.

    7. ls -R parameter does not function.

    8. ls -l produces lstat errors on certain files

    9. ls will notify of undefined parameters
    
    10.ls output is not ordered
    
    11. Many more.
