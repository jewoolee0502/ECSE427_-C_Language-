int exec( char* argument[] , int numberArgs )
{
	int errorCode = 0 ;
	int sameProgram = 0 ;
	char* program ;

	if ( numberArgs < 2 || numberArgs > 4 )
	{
		errorCode = 7 ;
		return errorCode ;
	}

	if ( numberArgs == 3 )
	{
		if( strcmp( argument[1] , argument[2]) == 0 )
		{
			sameProgram = 2 ;
			program = argument[2] ;
		}
	}

	if ( numberArgs == 4 )
    {
		if ((strcmp(argument[1], argument[3]) == 0) && (strcmp(argument[2], argument[3]) == 0))
		{
			sameProgram = -1 ;
			program = argument[1] ;

		} else if (strcmp(argument[1], argument[3]) == 0)
        {
			sameProgram = 3;
			program = argument[1];

		} else if (strcmp(argument[2], argument[3]) == 0 )
        {
			sameProgram = 3;
			program = argument[3];

		} else if ( strcmp(argument[1], argument[2]) == 0)
        {
			sameProgram = 2;
			program = argument[1];
		}
	}

	for( int i = 0 ; i < numberArgs - 1 ; i++ )
    {
		if ( sameProgram != 0 && i == sameProgram - 1 )
        {
			continue ;

		} else
        {
			errorCode = myinit( argument[ i+1 ] ) ;
			if ( errorCode != 0 )
            {
				return errorCode;
			}
		}
		if( sameProgram == -1 )
        {
			break;
		}
	}

	errorCode = scheduler() ;

	if( sameProgram == -1)
	{
		printf ( "Error: Script %s already loaded\n" , program ) ;

	} else if (sameProgram != 0)
	{
		printf ( "Error: Script %s already loaded\n" , program ) ;
	}
	return errorCode;
}



///////////HAIPENG GIVEN CODE///////////////////////////////////


int exec(char* f1, char* f2, char* f3){
    FILE *src1;
    FILE *src2;
    FILE *src3;

    //if there is potentially more than one file
    if(f2){
        //if f1 and f2 are the same
        if(checkSameFile(f1, f2) == TRUE){
            printf("Error: %s is already being loaded!", f2);
            return 1;
        }
        else{
            //if there is a third file
            if(f3){
                //if f3 is the same as one of the other files
                if(checkSame3File(f1, f2, f3) == TRUE){
                    printf("Error: %s is already being loaded!", f3);
                    return 1;
                }

                else{
                        src1 = fopen(f1, "rt");
                        src2 = fopen(f2, "rt");
                        src3 = fopen(f3, "rt");
                        if(!src1 || !src2 || !src3){
                            printf("Error: One or more of the files can't be found\n");
                            return 1;
                        }

                        //if nothing went wrong in initializing kernel for each file
                        if(initialize_kernel(src1) == 0 && initialize_kernel(src2) == 0 && initialize_kernel(src3) == 0){
                            scheduler();
                            printf("three files have been successfully loaded!\n");
                            return 0;
                        }
                        else
                            return 1;
                }
            }
            //if there is only two files
            else{
                src1 = fopen(f1, "rt");
                src2 = fopen(f2, "rt");
                if(!src1 || !src2){
                    printf("Error: One of the files can't be found!\n");
                    return 1;
                }

                //if nothing went wrong in initializing kernel of each file
                if(!initialize_kernel(src1) && !initialize_kernel(src2)){
                    scheduler();
                    printf("two files have successfully been loaded!\n");
                    return 0;
                }
                else
                    return 1;
            }
        }
    }

    //if there is only one file
    else{
        src1 = fopen(f1, "rt");
        //if the file can't be found
        if(!src1){
            printf("Error: %s cannot be found", f1);
            return 1;
        }

        //if error occured when initializing the kernel on the file
        if(initialize_kernel(src1) == 0)
            scheduler();
        else
            return 1;

        printf("one file is successfully loaded\n");
        return 0;
    }
}