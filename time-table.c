/*
Program to create a timetable
</> with <3
Shubham Saxena
*/

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define MAX_OPT 30

int no_of_crs;
char timetable[6][9][20];
int *perms;
int score = 0;

int sat_toggle = 1;
int possible_opt[30][30];

int min_break = 3;
int first_hour_score = 10;
int break_score = 10;

typedef struct crs{
	char name[11];
	int no_of_opt;
	int hrs_per_week;
	char opt[MAX_OPT][21];
}COURSES;

COURSES *course;

int read_booklet();
int read_preferences();
int has_saturday_classes(int course_no, int opt_no);
int form_time_tables(int);
int preferentially_form_time_tables(int);
void set_null();
int fill_and_validate();
int partial_fill_and_validate(int);


//FILE HANDLER
void fprint_time_table();
int convert_perms_to_timetables(int x);
void insert_perms();

//SCORER
void scorer();

//DEBUGGER
void print_time_table();
void print_time_table_driver();
void read_booklet_driver();
void insert_perms_driver();
void driver();

int main(){

/*code to form all timetables and print to the screen
	read_booklet();
	perms = calloc(no_of_crs,sizeof(int));
	for(int i=0;i<no_of_crs;i++)perms[i]=0;
	form_time_tables(0);
/---------------------------------------------------*/
	
/*code to form all timetables preferentially and print to the screen
	read_booklet();
	perms = calloc(no_of_crs,sizeof(int));
	read_preferences();
	for(int i=0;i<no_of_crs;i++)perms[i]=0;
	preferentially_form_time_tables(0);
/*--------------------------------------------------*/
	
/*code to convert all timetables into visual form*/
	read_booklet();
	perms = calloc(no_of_crs,sizeof(int));
	read_preferences();
	for(int i=0;i<no_of_crs;i++)perms[i]=0;
	preferentially_form_time_tables(0);
	convert_perms_to_timetables(-1);
/*--------------------------------------------------*/

/*code to convert a few timetables into visual form
	read_booklet();
	perms = calloc(no_of_crs,sizeof(int));
	read_preferences();
	for(int i=0;i<no_of_crs;i++)perms[i]=0;
	convert_perms_to_timetables(20);
/*--------------------------------------------------*/

//	driver();
	return 0;
}

int read_booklet(){
	FILE *fin;
	char temp[21];
	fin = fopen("time_table_booklet.txt","r");
	if(!fin){
		printf("Error: file not found (time_table_booklet.txt)\n");
		return 1;
	}
	fscanf(fin,"%d",&no_of_crs);
	course = calloc(no_of_crs,sizeof(COURSES));
	for(int i=0 ; i<no_of_crs ; i++){
		fscanf(fin,"%s",course[i].name);
		fscanf(fin,"%d",&course[i].hrs_per_week);
		fscanf(fin,"%d",&course[i].no_of_opt);
		for(int j=0 ; j<course[i].no_of_opt ; j++){
			fscanf(fin,"%s",course[i].opt[j]);
		}
		fscanf(fin,"%s",temp);
		if(strcmp(temp,"course-end")){
			printf("Error: course-end sentinel not reached\n");
			return 1;
		}
	}
	fscanf(fin,"%s",temp);
	if(strcmp(temp,"file-end")){
		printf("Error: file-end sentinel not reached\n");
		return 1;
	}
	fclose(fin);
	return 0;
}

int read_preferences(){
	FILE *fin;
	int temp_opt;
	char opt_toggle[3];
	char course_name[11];
	int i,j;
	fin = fopen("preferences.txt","r");
	if(!fin){
		printf("Error: file not found (preferences.txt)\n");
		return 1;
	}
	fscanf(fin,"%d",&sat_toggle);
	for(i=0 ; i<no_of_crs ; i++){
		fscanf(fin,"%s",course_name);
		for(j=0 ; j<no_of_crs ; j++){
			if(!strcmp(course_name,course[j].name)){
				break;
			}
		}
		fscanf(fin,"%s",opt_toggle);
		if(!strcmp(opt_toggle,"+")){
			fscanf(fin,"%d",&temp_opt);
			temp_opt--;
			int opt_no = 0;
			while(temp_opt!=-1){
				if(sat_toggle||(!has_saturday_classes(j,temp_opt))){
					possible_opt[j][opt_no] = temp_opt;
					opt_no++;
				}
				fscanf(fin,"%d",&temp_opt);
				temp_opt--;
			}
			possible_opt[j][opt_no] = -1;
		}
		else if(!strcmp(opt_toggle,"-")){
			fscanf(fin,"%d",&temp_opt);
			temp_opt--;
			int filled_opt = 0;
			int opt_no = 0;
			while(temp_opt!=-1){
				while(filled_opt!=temp_opt&&filled_opt<course[j].no_of_opt){
					if(sat_toggle||(!has_saturday_classes(j,filled_opt))){
						possible_opt[j][opt_no] = filled_opt;
						opt_no++;
					}
					filled_opt++;
				}
				filled_opt++;
				fscanf(fin,"%d",&temp_opt);
				temp_opt--;
			}
			while(filled_opt<course[j].no_of_opt){
				if(sat_toggle||(!has_saturday_classes(j,filled_opt))){
					possible_opt[j][opt_no] = filled_opt;
					opt_no++;
				}
				filled_opt++;
			}
			possible_opt[j][opt_no] = -1;
		}
		else {
			printf("Error: opt_toggle not read.");
			return 1;
		}
	}
	char temp[20];
	fscanf(fin,"%s",temp);
	if(strcmp(temp,"file-end")){
		printf("Error: file-end sentinel not reached\n");
		return 1;
	}
	fclose(fin);
	return 0;
}

int has_saturday_classes(int course_no, int opt_no){
	for(int i=0;course[course_no].opt[opt_no][i]!='\0';i++){
		if(course[course_no].opt[opt_no][i]=='f')return 1;
	}
	return 0;
}

int form_time_tables(int perm_from){
	if(perm_from<no_of_crs-1){
		//for all courses
		for(int i=0 ; i<course[perm_from].no_of_opt ; i++){
			perms[perm_from] = i;
			set_null();
			if(!partial_fill_and_validate(perm_from))form_time_tables(perm_from+1);
		}
	}
	else{
		//for last course
		for(int i=0 ; i<course[perm_from].no_of_opt ; i++){
			perms[perm_from] = i;
			set_null();
			if(!fill_and_validate()){
				for(int k=0 ; k<no_of_crs ; k++)printf("%2d ",perms[k]);
				printf("\n");
			}
		}
	}
	return 0;
}

int preferentially_form_time_tables(int perm_from){
	if(perm_from<no_of_crs-1){
		//for all courses
		for(int i=0 ; possible_opt[perm_from][i]!=-1 ; i++){
			perms[perm_from] = possible_opt[perm_from][i];
			set_null();
			if(!partial_fill_and_validate(perm_from))preferentially_form_time_tables(perm_from+1);
		}
	}
	else{
		//for last course
		for(int i=0 ; possible_opt[perm_from][i]!=-1 ; i++){
			perms[perm_from] = possible_opt[perm_from][i];
			set_null();
			if(!fill_and_validate()){
//				for(int k=0 ; k<no_of_crs ; k++)printf("%2d ",perms[k]);
				scorer();
				insert_perms();
//				printf("\n");
			}
		}
	}
	return 0;
}

void set_null(){		//sets timetable to null
	for(int i=0;i<6;i++){
		for(int j=0;j<9;j++){
			timetable[i][j][0]='\0';
		}
	}
}

int fill_and_validate(){	//fills timetable according to current perms, and validates
	char temp[21];
	char course_name_and_perm[14];
	for(int i=0 ; i<no_of_crs ; i++){
		strcpy(temp,course[i].opt[perms[i]]);
		for(int j=0 ; temp[j]!='\0' ; j+=2){
			if(timetable[temp[j]-97][temp[j+1]-49][0]=='\0'){
				sprintf(course_name_and_perm,"%10s:%2d",course[i].name,perms[i]+1);
				strcpy(timetable[temp[j]-97][temp[j+1]-49],course_name_and_perm);
			}
			else {
				return 1;
			}
		}
	}
	for(int i=0 ; i<6 ; i++){
		if(timetable[i][3][0]!='\0'&&timetable[i][4][0]!='\0'&&timetable[i][5][0]!='\0')return 1;
	}
	return 0;
}

int partial_fill_and_validate(int index){	//fills timetable according to current perms, and validates
	char temp[21];
	char course_name_and_perm[14];
	for(int i=0 ; i<=index ; i++){
		strcpy(temp,course[i].opt[perms[i]]);
		for(int j=0 ; temp[j]!='\0' ; j+=2){
			if(timetable[temp[j]-97][temp[j+1]-49][0]=='\0'){
				sprintf(course_name_and_perm,"%s:%2d",course[i].name,perms[i]+1);
				strcpy(timetable[temp[j]-97][temp[j+1]-49],course_name_and_perm);
			}
			else {
				return 1;
			}
		}
	}
	for(int i=0 ; i<6 ; i++){
		if(timetable[i][3][0]!='\0'&&timetable[i][4][0]!='\0'&&timetable[i][5][0]!='\0')return 1;
	}
	return 0;
}




							/*FILE HANDLER*/




void fprint_time_table(){
	FILE *fout;
	fout = fopen("timetables-output.txt","r");
	if(!fout){
		fout = fopen("timetables-output.txt","w");
	}
	else {
		fclose(fout);
		fout = fopen("timetables-output.txt","a");
	}
	fprintf(fout,"\n\n\n\t\t\t\t\t\t\tScore: %d\n",score);
	fprintf(fout,"\n\t __________________________________________________________________________________________________________________________\n\n");
	fprintf(fout,"\t|  %-10s  |  %-13s  |  %-13s  |  %-13s  |  %-13s  |  %-13s  |  %-13s  "," ","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday");
	fprintf(fout,"|\n\t __________________________________________________________________________________________________________________________\n\n");
	for(int i=0 ; i<9 ; i++){
		fprintf(fout,"\t");
		int time = (i>4)?(i-4):(i+8);
		char time_string[11];
		sprintf(time_string,"%d:00 %s",time,((i<4)?"am":"pm"));
		fprintf(fout,"|  %10s  ",time_string);
		for(int j=0 ; j<6 ; j++){
			fprintf(fout,"|  %-13s  ",timetable[j][i]);
		}
		fprintf(fout,"|\n\t __________________________________________________________________________________________________________________________\n\n");
	}
	fprintf(fout,"\nx------------x------------x---------------x---------------x---------------x---------------x----------------x----------------x-----------------x\n\n\n");
	fclose(fout);
}

int convert_perms_to_timetables(int x){		//returns 0 if runs correctly
	FILE *fin;
	fin = fopen("permutations.txt","r");
	if(!fin){
		printf("Error: file not found (permutations.txt)");
		return 1;
	}
	if(x==-1){
		while(fscanf(fin,"%d",&score)!=EOF){
			for(int i=0 ; i<no_of_crs ; i++){
				fscanf(fin,"%d",&perms[i]);
			}
			set_null();
			fill_and_validate();
			fprint_time_table();
		}
	}
	else {
		while(x>=0&&(fscanf(fin,"%d",&score)!=EOF)){
			for(int i=0 ; i<no_of_crs ; i++){
				fscanf(fin,"%d",&perms[i]);
			}
			set_null();
			fill_and_validate();
			fprint_time_table();
		}
	}
	fclose(fin);
}

void insert_perms(){
	FILE *fin;
	FILE *fout;	
	int file_exists = 1;
	int cur_score, temp;
	long pos = 0;
	fin = fopen("permutations.txt","r");
	if(!fin)file_exists = 0;
	if(file_exists){
		fout = fopen("temp.txt","w");
		pos = ftell(fin);
		while((fscanf(fin,"%d",&cur_score)!=EOF)&&score<cur_score){
			fprintf(fout,"%d ",cur_score);
			for(int i=0 ; i<no_of_crs ; i++){
				fscanf(fin,"%d",&temp);
				fprintf(fout,"%d ",temp);
			}
			fprintf(fout,"\n");
			pos = ftell(fin);
		}
		fprintf(fout,"%d ",score);
		for(int i=0 ; i<no_of_crs ; i++){
			fprintf(fout,"%d ",perms[i]);
		}
		fprintf(fout,"\n");
		fseek(fin,pos,SEEK_SET);
		while(fscanf(fin,"%d",&cur_score)!=EOF){
			fprintf(fout,"%d ",cur_score);
			for(int i=0 ; i<no_of_crs ; i++){
				fscanf(fin,"%d",&temp);
				fprintf(fout,"%d ",temp);
			}
			fprintf(fout,"\n");
		}
		fclose(fin);
		fclose(fout);
		remove("permutations.txt");
		rename("temp.txt","permutations.txt");
	}
	else {
		fout = fopen("permutations.txt","w");
		fprintf(fout,"%d ",score);
		for(int i=0 ; i<no_of_crs ; i++){
			fprintf(fout,"%d ",perms[i]);
		}
		fprintf(fout,"\n");
		fclose(fout);
	}
}





								/*SCORER*/


void scorer(){		//returns score
	score = 0;
	int break_time = 0;
	for(int i=0 ; i<6 ; i++){
		if(timetable[i][0][0]=='\0')score += first_hour_score;
	}
	for(int i=0 ; i<6 ; i++){
		break_time = 0;
		int j = 0;
		for( ; timetable[i][j][0]=='\0' ; j++);
		for( ; j<9 ; j++){
			if(timetable[i][j][0]=='\0')break_time++;
			else {
				if(break_time>=min_break)score += break_score;
				break_time = 0;
			}
		}
	}
}







								/*DEBUGGER*/

void print_time_table(){
	printf("\n\n\n\t\t\t\t\t\t\tScore: %d\n",500);
	printf("\n\t ________________________________________________________________________________________________________\n\n");
	printf("\t|  %-10s  |  %-10s  |  %-10s  |  %-10s  |  %-10s  |  %-10s  |  %-10s  "," ","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday");
	printf("|\n\t ________________________________________________________________________________________________________\n\n");
	for(int i=0 ; i<9 ; i++){
		printf("\t");
		int time = (i>4)?(i-4):(i+8);
		char time_string[11];
		sprintf(time_string,"%d:00 %s",time,((i<4)?"am":"pm"));
		printf("|  %10s  ",time_string);
		for(int j=0 ; j<6 ; j++){
			printf("|  %-10s  ",timetable[j][i]);
		}
		printf("|\n\t ________________________________________________________________________________________________________\n\n");
	}
	printf("\nx------------x------------x---------------x---------------x---------------x---------------x----------------x----------------x\n\n\n");
}

void print_time_table_driver(){
	set_null();
	strcpy(timetable[3][4],"M1-Lec");
	strcpy(timetable[1][2],"Meow-Lec");
	strcpy(timetable[5][5],"CP-Lab");
	strcpy(timetable[0][0],"Pata Nahi");
	print_time_table();
	print_time_table();
}

void read_booklet_driver(){
	read_booklet();
	for(int i=0 ; i<no_of_crs ; i++){
		printf("course name : %s\n",course[i].name);
		printf("hours per week : %d\nno of options : %d\n",course[i].hrs_per_week,course[i].no_of_opt);
		for(int j=0 ; j<course[i].no_of_opt ; j++){
			printf("option %d : %s\n",j+1,course[i].opt[j]);
		}
	}
}


void driver(){
	printf("Running tests...\n");
}


/*
						JUNKYARD... :(




void insert_perms_driver(){
	no_of_crs = 3;
	perms = (int *)calloc(no_of_crs,sizeof(int));
	*perms=2;
	*(perms+1)=2;
	*(perms+2)=2;
	score=450;
	insert_perms();
}


*/

