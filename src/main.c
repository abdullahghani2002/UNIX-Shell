#define _GNU_SOURCE
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <glob.h>
#include <readline/readline.h>
#include <readline/history.h>

struct Entry {
    char *key;
    char *value;
};

struct Dictionary {

    struct Entry *data;
    int size;
    int capacity;
};

void initializer(struct Dictionary* dict,int cap)
{
    dict->size = 0;
    dict->capacity = cap;
    dict->data = (struct Entry*)malloc(cap * sizeof(struct Entry));

    if(dict->data == NULL)
    {
        LOG_PRINT("wurhgye");
        exit(1);
    }


}


void insert_entry(struct Dictionary* dict, char* k, char* val)
{
    // printf("%s\n",val);
    if(dict->size == dict->capacity)
    {
       dict->capacity = dict->capacity*2;
       dict->data = (struct Entry*)realloc(dict->data,dict->capacity * sizeof(struct Entry));
    }

    for(int i=0; i<dict->size; i++) // if entry already exists
    {
        if(strcmp(dict->data[i].key,k)==0)
        {
            dict->data[i].value = strdup(val);
            return;
        }
    }

    dict->data[dict->size].value = strdup(val);
    dict->data[dict->size].key = strdup(k);
    dict->size++;



}

void delete_entry(struct Dictionary* dict, char* key)
{

    if(dict->size <=0)
    {
        return;
    }
    for(int i=0; i<dict->size;i++)
    {
        if (strcmp(dict->data[i].key,key)==0)
        {
           dict->data[i].value= dict->data[dict->size-1].value;
           dict->data[i].key = dict->data[dict->size-1].key;
           dict->size--;
           return;
        }
    }

}

char* get_entry(struct Dictionary *dict, char *k)
{

    // LOG_PRINT(" GETTINGGG %d\n",dict->size);
     for(int i=0; i<dict->size;i++)
    {
        if (strcmp(dict->data[i].key,k)==0)
        {
            return dict->data[i].value;
        }
    }

    return NULL;
}

// I miss Python.
char ***op_parser(char *input)
{
    size_t number_of_programs = 0;

    // printf("here is the input: %s and here the length : %d",input,strlen(input));

    for(size_t i=0;i<strlen(input)-1;i++)
    {
        if(input[i] == '|' && input[i+1] == '|')
        {
            number_of_programs+=2;
            i++;
        }
        else if (input[i] == '|')
        {
            number_of_programs+=2;

        }
        else if(input[i] == '&' && input[i+1] == '&')
        {
            number_of_programs+=2;
        }
        else if(input[i] == '>' && input[i] == '>')
        {
            number_of_programs+=2;
            i++;
        }        
        else if(input[i] == '>')
        {
            number_of_programs+=2;
        }
        else if(input[i] == '<' && input[i] == '<')
        {
            number_of_programs+=2;
            i++;
        }        
        else if(input[i] == '<')
        {
            number_of_programs+=2;
        }
        else if(input[i] == ';')
        {
            number_of_programs+=2;
        }


    }
    number_of_programs++;
    
    char ***programs_array = (char ***)malloc((number_of_programs+1) * sizeof(char**));

    size_t i = 0;
    size_t j = 0;
    size_t program_start = 0;
    bool operator_found = false;
    char last_op;

    // printf("numba prog %d\n",number_of_programs);


    while(i!=number_of_programs) // setting each command
    {
        if(operator_found)
        {
            char **program = (char**)malloc(2 * sizeof(char*)); // for each program, how many commands/args it has
            char *op = (char*) malloc(3* sizeof(char));

            i++;
            operator_found = false;

            if(last_op == 'o')
            {
                op[0] = '|';
                op[1] = '|';
                op[2] = '\0';
            }
            else if (last_op == 'r')
            {
                op[0] = '>';
                op[1] = '>';
                op[2] = '\0';
            }
            else if(last_op == 'l')
            {
                op[0] = '<';
                op[1] = '<';
                op[2] = '\0';
            }
            else if(last_op == '&')
            {
                op[0] = '&';
                op[1] = '&';
                op[2] = '\0';
            }
            else
            {
                // printf("HEre is iam hahaHA %c\n",last_op);
                op[0] = last_op;
                op[1] = '\0';
            }

            program[0] = op;
            program[1] = NULL;
            programs_array[i-1] = program;

            // printf("Size of i is %d and word is: %s \n", i-1, program[0]);
            continue;
        }

        program_start = j;

        size_t cmd_size = 0;
        
        while(j < strlen(input)) // finding every word
        {


            // printf("RUNNING %d %d\n",i,j);
            if(input[j] == '\"')
            {
                while(input[j+1]!= '\"')
                {
                    j++;
                }
                j++;
            }
            else if(input[j] == '\'')
            {
                while(input[j+1]!= '\'')
                {
                    j++;
                }
                j++;
            }
            else if(input[j] == ' ')
            {

                if(j==0)
                {
                    while(input[j]==' ') // spaces at start so no command to add
                    {
                        j++;
                    }
                    continue;
                }
                else
                { 
                    while(input[j]==' ') // if extra spaces
                    {
                        j++;
                    }

                    if(j == strlen(input)) // means spaces at the end so no command to add
                    {break;}
                    else
                    {         
                        cmd_size++;
                        continue;
                    }
                }

            }
            else if(input[j] == '|' && input[j+1] == '|')
            {
                j+=2;

                while(j < strlen(input) && input[j]==' ') // ensure no space 
                {
                    j++;
                }
                operator_found = true;
                last_op = 'o';
                break;
            }
            else if (input[j] == '|')
            {
                j++;
                while(j < strlen(input)  && input[j]==' ')
                {
                    j++;
                }
                operator_found = true;
                last_op = '|';
                break;

            }
            else if(input[j] == '&' && input[j+1] == '&')
            {
                j+=2;
                while(j < strlen(input)  && input[j]==' ')
                {
                    j++;
                }                
                operator_found = true;
                last_op = '&';

                break;
            }
             else if(input[j] == '>' && input[j+1] == '>')
            {
                j+=2;
                while(j < strlen(input)  && input[j]==' ')
                {
                    j++;
                }
                operator_found = true;
                last_op = 'r';

                break;
            }        
            else if(input[j] == '>')
            {
                j++;
                while(j < strlen(input)  && input[j]==' ')
                {
                    j++;
                }
                last_op = '>';
                operator_found = true;
                break;
            }
            else if(input[j] == '<' && input[j+1] == '<')
            {
                j+=2;
                while(j < strlen(input)  && input[j]==' ')
                {
                    j++;
                }
                operator_found = true;
                last_op = 'l';
                break;
            }        
            else if(input[j] == '<')
            {
                j++;
                while(j < strlen(input)  && input[j]==' ')
                {
                    j++;
                }
                operator_found = true;
                last_op = '<';

                break;
            }
            else if(input[j] == ';')
            {
                // printf("eliiii\n");
                j++;
                while(j < strlen(input)   && input[j]==' ')
                {
                    j++;
                }
                operator_found = true;
                last_op = ';';

                break;
            }
            j++;
  
        }

        if(j == strlen(input)) // incase no chain operator was found
        {
            cmd_size++;
        }

        char **program = (char**)malloc((cmd_size+1) * sizeof(char*)); // for each program, how many commands/args it has
        programs_array[i] = program;
        

        // printf("")
        size_t k=0;

        
        // REDUCED TO 1 PROGRAM
        while(k < cmd_size) // while k is less than the number of commands in the program
        {
            // printf("Check size: %d\n",cmd_size);
            int word_size = 0;
            char* temp_array = (char *)malloc((word_size+1) * sizeof(char));

            while(program_start < j) // for one program only. Each word should end either with space or with chain operator, or \n
            {

                
                if(program_start == 0)
                {
                    while(input[program_start] == ' ')
                    {
                        program_start++;
                    }
                }

                if(input[program_start] == '\"')
                {
                    while(input[program_start + 1] != '\"')
                    {
                        temp_array[word_size] = input[program_start];
                        word_size++;
                        program_start++;
                        temp_array = (char*)realloc(temp_array,(word_size+1) * sizeof(char));
                    }

                    temp_array[word_size] = input[program_start];
                    word_size++;
                    program_start++;
                    temp_array = (char*)realloc(temp_array,(word_size+1) * sizeof(char));
                }
                else if(input[program_start] == '\'')
                {
                    while(input[program_start + 1] != '\'')
                    {
                        temp_array[word_size] = input[program_start];
                        word_size++;
                        program_start++;
                        temp_array = (char*)realloc(temp_array,(word_size+1) * sizeof(char));
                    }

                    temp_array[word_size] = input[program_start];
                    word_size++;
                    program_start++;
                    temp_array = (char*)realloc(temp_array,(word_size+1) * sizeof(char));
                }
                
                else if (input[program_start] == ' ')
                {
                    while(input[program_start] == ' ')
                    {
                        program_start++;
                    }
                    break;

                }
                else if( input[program_start]== '|' && input[program_start+1] == '|') // signifies end anyway so no need to change p_start
                {
                    break;
                }
                else if (input[program_start] == '|')
                {
                    break;
                }
                else if(input[program_start]== '&' && input[program_start+1] == '&')
                {
                    break;
                }
                else if(input[program_start] == '>' && input[program_start+1]== '>')
                {
                    break;
                }        
                else if(input[program_start]== '>')
                {
                    break;
                }
                else if(input[program_start] == '<' && input[program_start+1] == '<')
                {
                    break;
                }        
                else if(input[program_start] == '<')
                {
                    break;
                }
                else if(input[program_start]== ';')
                {
                    break;
                }

                temp_array[word_size] = input[program_start];

                word_size++;
                program_start++;
                temp_array = (char*)realloc(temp_array,(word_size+1) * sizeof(char));
            }

            temp_array[word_size] = '\0';
            program[k] = temp_array;

            // printf("here! tempura : %s\n",temp_array);
            k++;
        }

        program[cmd_size] = NULL;
        cmd_size = 0;
        
        i++;
    }

    programs_array[number_of_programs] = NULL;
    return programs_array;
}

char **parser(char* input,char *delim)
{
    char *cpy = (char*)malloc((strlen(input)+1) * sizeof(char));
    strcpy(cpy,input);

    int size = 0;

    for(size_t i=0;i<strlen(cpy)-1;i++)
    {
        if(cpy[i] == ' ')
        {
            size++;
        }
    }
    size++;

    char *token = strtok(cpy,delim);
    char **argv = (char **)malloc((size + 1) * sizeof(char *)); 

    int i=0;
    while(token)
    {
        argv[i] = (char *)malloc((strlen(token) + 1) * sizeof(char));
        strcpy(argv[i], token);
        token = strtok(NULL,delim);

        i++;
    }
    return argv;
}




int main(int argc, char**argv)
{

 

    char *input = NULL;
    char line[4096];
    FILE *file;
    bool hist_read = false;
    int hist_index;
    HIST_ENTRY **hist_list;


   
    if(argc == 2)
    {
        file = fopen(argv[1],"rb");
    }

    // LOG_PRINT("Hello World!\n");
    // LOG_DEBUG("A debug statement\n");
    // LOG_ERROR("An error statement\n");


    struct Dictionary alias_store;

    initializer(&alias_store,1000);

    pid_t pid;

    rl_bind_key ('\t', rl_complete );

    using_history ();

    int std_out = dup(1);
    int std_in = dup(0);

    int new_pipe = 0;

    size_t input_length = 0;


    while (1) {


        dup2(std_in,0);
        dup2(std_out,1);


        if(hist_read)
        {
            hist_list = history_list();
            if(hist_list[hist_index])
            {
                input = hist_list[hist_index]->line;
            }

            input_length = strlen(input);

            hist_read = false;
        }

        else if(argc==1)
        {
            input = readline ("$ ");
            add_history(input);
            input_length = strlen(input);


        }
        else if(argc == 2)
        {
            input = fgets(line, sizeof(line), file);
            if(input == NULL)
            {
                break;
            }

            // printf("HERE: %s\n",input);

            add_history(input);
            input_length = strlen(input);
        }


        if(input==NULL || input[0] == '\0')
        {
            break;
        }

        
        for(size_t i=0; i< input_length; i++)
        {
            if(input[i] == '\n') // one line is one program
            {
                input[i] = '\0';
                break;
            }
        }

         
        char ***full_tokens = op_parser(input);
        char *command;
        char **new_args;
        char **args = (char**)malloc((1)*sizeof(char*));


        bool file_input = false;
        bool file_output = false;

        int pipe_in;
        int pipe_out;

        bool last_status = true;
        bool input_pipe = false;
        int status = 0;

        




        for(int i=0; full_tokens[i]!= NULL ; i++) // every program
        {

            command = full_tokens[i][0];
            new_args = (char **)malloc(2 * sizeof(char*));
            new_args[0] = command;
            int alias_size = 0;
            int k = 0;


            char* alias = get_entry(&alias_store,command);

            if(alias)
            {
                char ***alias_commands = op_parser(alias);
                for(int i=0; alias_commands[0][i]!=NULL;i++)
                {

                    if(i==0)
                    {
                        command = alias_commands[0][i];
                        new_args[0] = command;//replace commands
                    }
                    else
                    {
                        new_args[i] = (alias_commands[0][i]);
                        new_args = (char**)realloc(new_args, (i+2)*sizeof(char*));
                        alias_size++;
                    }
                }
            }


            for (int j=0; full_tokens[i][j] != NULL ; ++j) // every command in the program
            {

                // printf("Im ova here ova here! %d\n ",j);
                
                if(j!=0)
                {
                    new_args[j+alias_size] = full_tokens[i][j];
                    new_args = (char**)realloc(new_args,(j + alias_size +2) * sizeof(char*));
                }

                k = j;
            }


            new_args[k+alias_size+1] = NULL;

            int a = 0;
            int curr_size = 0;
            while(new_args[a]!=NULL)
            {
                glob_t glob_result;
               
                glob(new_args[a],0,NULL,&glob_result);

                    if(glob_result.gl_pathc>=1)
                    {
                        // printf("HERE");
                        for(size_t g=0; g<glob_result.gl_pathc;g++)
                        {

                            args[curr_size] = glob_result.gl_pathv[g];
                            args = (char**)realloc(args,(curr_size+2) * sizeof(char *));
                            curr_size++;
                        }
                    }
                    else
                    {
                        args[curr_size] = new_args[a];
                        args = (char**)realloc(args,(curr_size+2) * sizeof(char *));
                        curr_size++;
                    }
                
                a++;
            }



            args[curr_size] = NULL;



            int fd[2];
            if(pipe(fd) < 0)
            {
                exit(1);
            }

            

            //  need to see how to set true
            if(strcmp(full_tokens[i][0],"&&")==0)
            {
                new_pipe = i;
                if(last_status == true)
                {
                    continue;
                }
                else
                {
                      while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                    continue;
                }   
            }
            else if(strcmp(full_tokens[i][0],"||")==0)
            {
                new_pipe = i;
                if(last_status == false)
                {
                    continue;
                }
                else
                {
                      while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                    continue;
                }  
            }
            else if(strcmp(full_tokens[i][0],";")==0)
            {
                // printf("gotemmm");
                new_pipe = i;
                continue;
            }
           
            if(i!=0)
            {
                if(strcmp(full_tokens[i-1][0],"|")==0)
                {
                        dup2(pipe_in,0);
                        close(pipe_in);
                        dup2(std_out,1);
                }
            }

            //Check for an IO operator

            if(full_tokens[i+1]!= NULL) // checking if there is an io operator next
            {
                // printf("%s\n",full_tokens[i+1][0]);
                if(strcmp(full_tokens[i+1][0],">")==0)
                {
                    // printf("hello gggg\n");
                    if(full_tokens[i+3]!= NULL)
                    {
                        if(strcmp(full_tokens[i+3][0],"&&") != 0 && strcmp(full_tokens[i+3][0],"||") && strcmp(full_tokens[i+3][0],";"))
                        {
                            last_status = false;
                            while(1)
                            {
                                if(full_tokens[i+1]==NULL)
                                {
                                    break;
                                }
                                else if (strcmp(full_tokens[i+1][0],"&&")==0)
                                {
                                    break;
                                }
                                else if (strcmp(full_tokens[i+1][0],"||")==0)
                                {
                                    break;
                                }
                                else if(strcmp(full_tokens[i+1][0],";")==0)
                                {
                                    break;
                                }
                                i++;
                                    
                            }
                            continue;

                        }


                    }
                    int f1 = open(full_tokens[i+2][0],O_RDWR | O_CREAT | O_TRUNC, 0644);
                    dup2(f1,STDOUT_FD); // replacing the stdout entry with the file
                    close(f1); // closing the extra descriptor for the file
                    file_output = true;

                    
                }
                else if(strcmp(full_tokens[i+1][0],">>")==0)
                {
                    if(full_tokens[i+3]!= NULL)
                    {
                        if(strcmp(full_tokens[i+3][0],"&&") != 0 && strcmp(full_tokens[i+3][0],"||") && strcmp(full_tokens[i+3][0],";"))
                        {
                            last_status = false;
                            while(1)
                            {
                                if(full_tokens[i+1]==NULL)
                                {
                                    break;
                                }
                                else if (strcmp(full_tokens[i+1][0],"&&")==0)
                                {
                                    break;
                                }
                                else if (strcmp(full_tokens[i+1][0],"||")==0)
                                {
                                    break;
                                }
                                else if(strcmp(full_tokens[i+1][0],";")==0)
                                {
                                    break;
                                }
                                i++;
                                    
                            }
                            continue;

                        }

                        
                    }
                    int f1 = open(full_tokens[i+2][0],O_RDWR | O_APPEND| O_CREAT,0644);
                    dup2(f1,STDOUT_FD);
                    close(f1);
                    file_output = true;
                }
                else if(strcmp(full_tokens[i+1][0],"<")==0)
                {
                    if((i!=0) && (i!=new_pipe+1))
                    {
                        last_status = false;
                          while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        continue;
                    }
                    int f1 = open(full_tokens[i+2][0],O_RDWR | O_APPEND| O_CREAT,0644);
                    if(f1)
                    {
                        dup2(f1,STDIN_FD);
                        close(f1);
                        file_input = true;  


                         if(full_tokens[i+2] != NULL && full_tokens[i+3]!= NULL)
                        {
                            if(strcmp(full_tokens[i+3][0],"|")==0)
                            {

                                input_pipe = true;
                                pipe_in = fd[0];

                                pipe_out = fd[1];
                                dup2(pipe_out,1);
                                close(pipe_out);

                                i+=2;
                            }
                        }
                        //if pipe, send it ahead
                    }
                    else
                    {
                        last_status = false;
                         while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        close(f1);
                        continue;
                    }
                }
                else if(strcmp(full_tokens[i+1][0],"|")==0 )
                {
                    //if pipe is coming, open the write pipe. Even though we will soon close this, it will be inherited by the child / filled up by built-ins
                    pipe_out = fd[1];
                    dup2(pipe_out,1);
                    close(pipe_out);

                    pipe_in = fd[0];

                    
                }
                else if(strcmp(full_tokens[i][0],"|")==0)
                {
                    continue;
                }
          
                
            }

   

            int size = 0;


            while(args[size]!= NULL)
            {
                // printf("%s | ",args[size]);
                size++;
            }
            // printf("|size: %d|||\n",size);


            // printf("I am %s and status rn is : %d \n",args[0],last_status);


            if(strcmp(command,"exit") == 0)
            {
                return 0;
            }    


            else if (strcmp(command,"pwd")==0)
            {
                char cwd[1024];

                if(getcwd(cwd,sizeof(cwd)))
                {
                    printf("%s",cwd);
                    printf("\n");
                }
                else
                {
                      while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        last_status = false;
                    continue;

                }
            }
            else if (strcmp(command,"cd") == 0)
            {
                if(size >2)
                {
                    last_status = false;
                      while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        continue;
                    printf("Incorrect number of arguments\n");
                    
                }

                if(size == 2)
                {
                    char *directory = args[1];
                    if(chdir(directory) == 0)
                    {}
                    else
                    {
                        last_status = false;

                          while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        continue;
                        printf("Failed");
                    }
                }

                else if(size == 1)
                {
                    if(chdir("/home") == 0)
                    {}
                    else
                    {
                        last_status = false;
                        while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        continue;
                        printf("Failed");
                        
                     }
                }

            }
            else if (strcmp(command,"alias") == 0)
            {

                if(size == 1)
                {
                    for(int i=0; i<alias_store.size;i++)
                    {
                        printf("%s='%s'\n",alias_store.data[i].key,alias_store.data[i].value);
                    }
                }
                else if(size == 2)
                {
                    char* alias_name = get_entry(&alias_store,args[1]);

                    if(alias_name)
                        printf("%s='%s'\n",args[1],alias_name);

                }
                else if(size == 3)
                {
                    char* alias_name = *(parser(args[2],"\""));
                    insert_entry(&alias_store,args[1],alias_name);
                }
                else
                {
                    last_status = false;
                      while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                        }
                        continue;
                    printf("Incorrect number of arguments provided\n");
                }

            }
            else if (strcmp(command,"unalias") == 0)
            {
                if(size == 2)
                {
                    delete_entry(&alias_store,args[1]);
                }
                else
                {
                    last_status = false;
                      while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++; 
                        }
                        continue;
                }


            }
            else if (strcmp(command,"echo") == 0)
            {
                // printf("Input : %s\n",input);
                int k = 1;
                while(args[k]!=NULL)
                {
                    char *dequote = *(parser(args[k],"\""));
                    printf("%s ",dequote);
                    k++;
                }
                printf("\n");
            }
            else if (strcmp(command,"history") == 0)
            {

                hist_list = history_list();

                if(size == 1)
                {
                    if(hist_list != NULL)
                    {
                        for(int i=0; hist_list[i]!= NULL; i++)
                        {
                            printf("%s\n",hist_list[i]->line);
                        }
                    }
                }
                else if(size == 2)
                {
                    hist_index = atoi(args[1]) - 1;
                    hist_read = true;
                }
                else
                {
                    last_status = false;
                      while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        continue;
                }
                
            }
            
            else
            {

             


                pid = fork();
                // printf("Just checkinggg %d ",pid);
                if(i==12)
                {
                    // printf("Now it is %d\n", last_status);
                }


                    if (pid == 0)
                    {

                                // int s = 0;
                                // printf("\nFor i = %d ||Args: ",i);
                                // while(args[s]!= NULL)
                                // {
                                //     printf("%s | ",args[s]);
                                //     s++;
                                // }
                                // printf("|size: %d|||\n",s);


                        int i = 0;

                        while(args[i]!= NULL)
                        {
                            args[i] = *parser(*(parser(args[i],"\"")),"\'");
                            i++;
                        }
                        
                        int result = execvpe(command,args,NULL);
                        if(i==12)
                        {
                            // printf("Now it is %d\n", last_status);
                        }



                        if (result == -1)
                        {
                            dup2(std_out,1);
                            dup2(std_in,0);
                            close(fd[0]);
                            close(fd[1]);

                            _exit(EXIT_FAILURE);
        
                            fprintf(stderr, "An error message on stderr\n");
                            
                             while(1)
                            {
                                if(full_tokens[i+1]==NULL)
                                {
                                    break;
                                }
                                else if (strcmp(full_tokens[i+1][0],"&&")==0)
                                {
                                    break;
                                }
                                else if (strcmp(full_tokens[i+1][0],"||")==0)
                                {
                                    break;
                                }
                                else if(strcmp(full_tokens[i+1][0],";")==0)
                                {
                                    break;
                                }
                                i++;
                            }
                            break;

                        }
                    }
                    if(pid == -1)
                    {
                        last_status = false;
                          while(1)
                        {
                            if(full_tokens[i+1]==NULL)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"&&")==0)
                            {
                                break;
                            }
                            else if (strcmp(full_tokens[i+1][0],"||")==0)
                            {
                                break;
                            }
                            else if(strcmp(full_tokens[i+1][0],";")==0)
                            {
                                break;
                            }
                            i++;
                                
                        }
                        continue;
                        printf("faced an error\n");
                    }

                    else
                    {
                        status = 0;
                        waitpid(pid,&status,0);
                        //  if (WIFEXITED(status)) 
                        //     printf("OK: Child exited with exit status %d.\n", WEXITSTATUS(status));
                        // else
                        //     printf("ERROR: Child has not terminated correctly.\n");

                        // printf("HERE at i:%d for command:%s and status: %d \n",i,full_tokens[i][0],status);
                        if(status !=0)
                        {
                           
                            if( (strcmp(args[0],"||")==0) || (strcmp(args[0],"&&")==0) || (strcmp(args[0],">>")==0) || (strcmp(args[0],">")==0) || (strcmp(args[0],"<")==0) || (strcmp(args[0],"|")==0) || (strcmp(args[0],";")==0))
                            {
                                // printf("goteeem\n\n");
                            }
                            else if(i>=1)
                            {
                                if((strcmp(full_tokens[i-1][0],"<")==0) || (strcmp(full_tokens[i-1][0],">")==0) || (strcmp(full_tokens[i-1][0],">>")==0))
                                {
                                    // printf("innit\n\n");
                                }
                                else
                                {
                                    // printf("bhai ho kia raha hai \n");;
                                    last_status = false;
                                    while(1)
                                    {
                                        if(full_tokens[i+1]==NULL)
                                        {
                                            break;
                                        }
                                        else if (strcmp(full_tokens[i+1][0],"&&")==0)
                                        {
                                            break;
                                        }
                                        else if (strcmp(full_tokens[i+1][0],"||")==0)
                                        {
                                            break;
                                        }
                                        else if(strcmp(full_tokens[i+1][0],";")==0)
                                        {
                                            break;
                                        }
                                        i++;
                                    }
                                    continue;
                                }
                            }
                            else
                            {
                                last_status = false;
                                while(1)
                                {
                                    if(full_tokens[i+1]==NULL)
                                    {
                                        break;
                                    }
                                    else if (strcmp(full_tokens[i+1][0],"&&")==0)
                                    {
                                        break;
                                    }
                                    else if (strcmp(full_tokens[i+1][0],"||")==0)
                                    {
                                        break;
                                    }
                                    else if(strcmp(full_tokens[i+1][0],";")==0)
                                    {
                                        break;
                                    }
                                    i++;
                                }
                                continue;
                            }

                            dup2(std_out,1);
                            dup2(std_in,0);

                            
                            // close(pipe_in);
                            // close(pipe_out);
                            // close(fd[0]);
                            // close(fd[1]);
                        }
                    }
                

            }

            if(file_output == true) // means last output file was detected
            {

                dup2(std_out,1);
                dup2(std_in,0);
                file_output = false;
            }
            if(input_pipe == true)
            {

                dup2(std_in,0);
                input_pipe = false;
                file_input = false;
            }
            else if (file_input)
            {
                dup2(std_in,0);
                dup2(std_out,1);
                file_input = false;
            }
            last_status = true;

            if(strcmp(full_tokens[i][0],"true")==0)
            {
                last_status = true;
            }
            if(strcmp(full_tokens[i][0],"false")==0)
            {
                last_status = false;
            }



        }

        // if(returnn)
        // {
        //                     LOG_PRINT("1");

        //     close(std_in);
        //     close(std_out);
        //     return;
        // }

           
        dup2(std_in,0);
        dup2(std_out,1);
    

    }
        dup2(std_in,0);
        dup2(std_out,1);

        fclose(file);

    return 0;
}
