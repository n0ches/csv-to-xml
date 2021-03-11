#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef struct{
    char *column_name;
}Headers;


typedef struct 
{
    char *lines;
}Line;


typedef struct
{
    char *command;
    char *source;
    char *destination;
    char *seper;
    int sepNum;
    char *ops;
    int opsNum;
}Input;

int cnt_space(char *token) 
{ 
    // input sentence 
    int k=strlen(token);
    char buf[k];
    strcpy(buf,token);
    char ch = buf[0]; 
    int i=0;
    int count=0;
    // counting spaces 
    while (ch != '\0') { 
        ch = buf[i]; 
        if (isspace(ch)) 
            count++; 
  
        i++; 
    } 
  
    // returning number of spaces 
    return (count); 
} 

Input takeInput(Input input, char *str){
    char space[]=" ";
    input.command= (char*)malloc(sizeof(char)*(strlen(str)+1));
    input.source= (char*)malloc(sizeof(char)*strlen(str+1));
    input.destination= (char*)malloc(sizeof(char)*strlen(str+1));
    input.seper= (char*)malloc(sizeof(char)*strlen(str+1));
    input.ops= (char*)malloc(sizeof(char)*strlen(str+1));
    input.command=strdup(strtok(str,space));
    input.source=strdup(strtok(NULL,space));
    input.destination=strdup(strtok(NULL,space));
    input.seper=strdup(strtok(NULL,space));
    input.sepNum=atoi(strtok(NULL,space));
    input.ops=strdup(strtok(NULL,space));
    input.opsNum=atoi(strtok(NULL,space));
    
    
    return input;
}

void menu(){
    printf("###############   CSV2XML CONVERTER   ###############\n");
    printf("enter '-h' command to see help page\n");
}

void help(){
    printf("\n\nThis is a CSV to XML Converter. You must enter a command containing the source file name, destination file name, seperator type and operating system.\n");
    printf("Your command line should be like this:\n");
    printf("\n\tCSV2XML inputfilename.csv outputfilename.xml -seper P1 -opsys P2\n\n");
    printf("-seper\ndefines seperator between each tatum (P1=1 -> comma, P1=2 -> tab, P1=3 -> semicolon)\n");
    printf("-opsys\ndefines the end of line character format (P2=1 -> windows(CR+LF), P2=2 -> linux(LF), P2=3 -> macos(CR))\n");
    printf("-h\nprint information to how arguments can be used\n");
    printf("-exit\nclose the program\n");
}

int main(){

    menu();//first screen

    // program loop \\

    while(1){

    printf("\nEnter a command: ");
    char *str=(char*)malloc(sizeof(char)*100); 
    fgets(str, 100, stdin);//taking command line from user

    //command line control, if command line is true, it must has 7 whitespaces. I control it in there
    if(cnt_space(str)<7){
        if(strncmp(str,"exit",4)==0){//if command is 'exit', pogram will close
            printf("Goodbye!\n");
            break;
        }
        if(strncmp(str,"-h",2)==0){//if command is '-h', display help page
            help();
            continue;
        }
        printf("\n\n!!Invalid command line. Please enter again according to command format. If you need help please enter '-h' command!!\n\n");
        continue;
    }
    //command line control, if command line is true, it must has 7 whitespaces. I control it in there
    
    
    Input input;
    input = takeInput(input,str);//I splitted input according to whitespaces to use its arguments.
    FILE *fp = fopen(input.source, "r");//file open. fp is file pointer
    char buf[1024];//using in the reading file
    char *delim= (char*)malloc(sizeof(char)*3);//defines delimeters
    char *end_of_file= (char*)malloc(sizeof(char)*4);//defines eof


    // ERROR HANDLING \\

    //error handling for CSV2XML argument
    if(strcmp(input.command,"CSV2XML")!=0){
        printf("Invalid command. You should enter 'CSV2XML'. Please enter again!\n");
        continue;
    }
    
    //error handling for file name argument
    if (!fp || fp==NULL) {
        printf("Invalid file name. Can't open file. Please enter command again with correct file name!\n");
        continue;
    }

    //error handling for file name formats (csv or xml)
    char *tempSource=strdup(input.source);
    char *tempDest=strdup(input.destination);
    char *sourceFormat=strtok(tempSource,".");
    sourceFormat=strtok(NULL,".");
    char *destFormat=strtok(tempDest,".");
    destFormat=strtok(NULL,".");
    
    if(strcmp(sourceFormat,"csv")!=0 ){
        printf("Invalid source file format. It must be '.csv' format. Please enter command again!\n");
        continue;
    }
    if(strcmp(destFormat,"xml")!=0){
        printf("Invalid destination file format. It must be '.xml' format. Please enter command again!\n");
        continue;
    }

    //error handling for -seper argument and defines seperator type
    if(strcmp(input.seper,"-seper")!=0){
        printf("Invalid seperator argument. It must be '-seper'. Please enter command again!\n");
        continue;
    }

    if(input.sepNum == 1){//if sepNum is 1, seperator is comma
        delim=",:";//I used ':' after the seperator for handling of empty places in csv file
    }
    else if(input.sepNum ==2){//if sepNum is 2, seperator is tab
        delim="\t:";
    }
    else if(input.sepNum==3){//if sepNum is 3, seperator is semicolon
        delim=";:";
    }
    else{
        printf("Invalid seperator type. It must be 1,2 or 3. Please enter the command again!\n");
        continue;
    }

    //error handling for -opsys argument and defines operating system type
    if(strcmp(input.ops,"-opsys")!=0){
        printf("Invalid operating system argument. It must be '-oper'. Please enter command again!\n");
        continue;
    }

    if(input.opsNum == 1){//if opsys is windows EOF is CR+LF
        end_of_file="\r\n";
    }
    else if(input.opsNum ==2){//if opsys is linux EOF is LF
        end_of_file="\n:";
    }
    else if(input.opsNum==3){//if opsys is macos EOF is CR
        end_of_file="\r";
    }
    else{
        printf("Invalid operating system type. It must be 1,2 or 3. Please enter command again!\n");
        continue;
    }

    // ERROR HANDLING \\

    //finding row count and column count in csv
    int row_count=-1;
    int column_count=0;
    char *ptr; //pointer to use for splitting lines.
    while (fgets(buf, 1024, fp) && strcmp(&buf[strlen(buf)-1],end_of_file)!=0) { // loop until end of file character
        if(row_count==-1){
            ptr=strtok(buf,delim);
            while(ptr!=NULL){
                ptr=strtok(NULL,delim);
                column_count++;
            }
        }
        else{
            //printf("%s", buf);
            
        }
        row_count++;
        
    }
    row_count++;
    //finding row count and column count in csv
    fclose(fp);//closing file.

    // Take the lines from csv \\

    Line lines[row_count];//define a line array
    FILE *fp2 = fopen(input.source, "r");//file open for putting lines into line array
    char buf2[1024];
    int count=0;
    while (fgets(buf2, sizeof(buf2), fp2) && strcmp(&buf2[strlen(buf2)-1],end_of_file)!=0) {
      lines[count].lines=strdup(buf2);
      count++;
    }
    fclose(fp2);//file close

    // Take the lines from csv \\
   
    //taking root name from outputfilename
    char *temp=strdup(input.destination);
    char *xmlPTR=strtok(temp,".");
    char *xmlName=strdup(xmlPTR);
    //taking root name from outputfilename
    FILE *fp3=fopen(input.destination,"w");//create xml file
    fputc('<',fp3);fputs(xmlName, fp3);fputs(">\n",fp3); //writing root into xml file


     // Splitting linnes and Writing into XML Operations \\

    char *original;//pointer to take original line
    char *token;//pointer to store splitted line
    Headers head[column_count];//array for taking head names(first_name, last_name,...)
    int head_count=0;//counting head number
    int counter=0;//counting row number
    for (int i = 0; i < row_count; i++)
    {   
        if(i>0){ // i>0 because first loop take headers(first_name,...). that's why program writes row id after first loop.
            counter++;
            fputc('\t',fp3);fputc('<',fp3);fputs("row id= '", fp3);fprintf(fp3,"%d",counter);fputs("'",fp3);fputs(">\n",fp3);//write root id into xml file
        } 
        head_count=0;
        original = strdup(lines[i].lines);//take original line
        while( (token = strsep(&original,delim))!=NULL){//split original line according to seperator type
            head_count++;
            if(i==0){//in this statement. I convert the columns names to header (First Name -> first_name)
                if(cnt_space(token)!=0){//if column name has two or more words, add '_' between them
                   for (int j = 0; j < token[j]; j++)
                   {
                       if(isspace(token[j]) && token[j]!='\0'){//if index j is space, add '_'
                           if(head_count==column_count ){//last elements of line is problem, it has two more '_' character if it has not two or more whitespace (address__ like this)
                               if(cnt_space(token)>=4){//last elements of line has extra 2 whitespace. That's why, if its space count is more than 3, there is no problem.
                                   token[j]='_';
                               }
                               else{
                                   if(cnt_space(token)==3){//if there is a problem, and token has extra 1 whitespace at the end. ı used memmove to clear this whitespace.
                                       token[j]='_';
                                       memmove(&token[strlen(token)-2],&token[strlen(token)],strlen(token));
                                   }
                                   if(cnt_space(token)==2){//if there is a problem, and token has extra 1 whitespace at the end. ı used memmove to clear this whitespace.
                                       memmove(&token[strlen(token)-2],&token[strlen(token)],strlen(token));
                                   }
                               }
                           }
                           else{//if index j is space, add '_'
                               token[j]='_';
                           }
                       }
                       else{//tolower
                           token[j]=tolower(token[j]);
                       }
                   }
                }
                else{//tolower
                   for (int j = 0; j < token[j]; j++)
                   {
                        token[j]=tolower(token[j]);
                   }
                }
                head[head_count-1].column_name=strdup(token);
            }
            else{
                if(strlen(token) == 0 || strlen(token)==2){//if line elements is empty. write just headers.
                    fputs("\t\t<",fp3);fputs(head[head_count-1].column_name,fp3);fputs(">",fp3); fputs("</",fp3);fputs(head[head_count-1].column_name,fp3);fputs(">\n",fp3);
                    //writing <firstname></firstname> into xml file
                }
                else{
                    if(head_count==column_count){//if last element of line. solving the whitespace problem that i mentioned before.
                        if(i==row_count-1){
                            strcpy(&token[strlen(token)-2],"");
                        }
                        else{
                            strcpy(&token[strlen(token)-2], "");
                        }//if last element of line. solving the whitespace problem that i mentioned before.
                        
                    }
                    fputs("\t\t<",fp3);fputs(head[head_count-1].column_name,fp3);fputs(">",fp3);fputs(token,fp3);fputs("</",fp3);fputs(head[head_count-1].column_name,fp3);fputs(">\n",fp3);
                    //writing <first_name>Ridvan</first_name> into xml file
                }
            }
        }
        if(i>0){//i>0 because i took header in the first turn.
            fputc('\t',fp3);fputs("</",fp3);fputs("row", fp3);fputs(">\n",fp3);
            //writing </root> into xml
        }
    }
    
    fputs("</",fp3);fputs(xmlName, fp3);fputc('>',fp3);//writing root into xml
    fclose(fp3);//close the file
    printf("Convertion is completed succesfully!\n");
    // Splitting linnes and Writing into XML Operations \\
    
    break;
  }
    
    return 0;
   
    
}