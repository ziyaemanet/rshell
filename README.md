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
    
   10. ls output is not ordered
    
   11. "|" does not function, use will cause undefined behaviour and rshell may crash

   12. >, >>, and < redirections functionality present

   13. using redirections in the following structure will not function:
       cat < testfile > testfile2, although testfile2 will be created
   
   14. Failing to add spaces before or after >, >>, <, or | will give error messagesfrom exevp or called program.

   15. Misalignment with output after using > or >>
    
   16. Using any >, >>, <, or | then failing to add a following parameter will give error message to check usage.

   17. Using multiple > will cause each file specified to be created but only first file will recieve ouput.
   
   18. cd will ignore parameters other than the first

   19. cd only will give bad address error

   20. cd and the ; connector do not work

   21. cd and && connector work together

   22. cd will notify of invalid directories

   23. ctrl c seems to affect all background processes if many are open
   
   24. tee and ls commands are accessible via PATH variable

   25. tee and pipe connector do not function together, only tee will be turned on

   26. current directory is now printed 

   27. ctrl z may fail to function at times but will generally work

   28. fg,bg command may fail to function at times but will generally work
   
   29. fg,bg followed by a number tends not to work most of the time

   30. Many more.





