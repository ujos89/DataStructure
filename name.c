#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_YEAR_DURATION	10	// 기간

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

// 함수 원형 선언

// 연도별 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// names->capacity는 2배씩 증가
void load_names( FILE *fp, int year_index, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// qsort를 위한 비교 함수
int compare( const void *n1, const void *n2);

// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
	
	int num = 0;
	FILE *fp;
	int num_year = 0;
	
	if (argc == 1) return 0;

	// 이름 구조체 초기화
	names = create_names();

	// 첫 연도 알아내기 "yob2009.txt" -> 2009
	int start_year = atoi( &argv[1][3]);
	
	for (int i = 1; i < argc; i++)
	{
		num_year++;
		fp = fopen( argv[i], "r");
		assert( fp != NULL);
		
		int year = atoi( &argv[i][3]); // ex) "yob2009.txt" -> 2009
		
		fprintf( stderr, "Processing [%s]..\n", argv[i]);
		
		// 연도별 입력 파일(이름 정보)을 구조체에 저장
		load_names( fp, year-start_year, names);
		
		fclose(fp);
	}
	
	// 정렬 (이름순 (이름이 같은 경우 성별순))
	qsort( names->data, names->len, sizeof(tName), compare);
	
	// 이름 구조체를 화면에 출력
	print_names(names, num_year);

	// 이름 구조체 해제
	destroy_names(names);
	
	return 1;
}

// names->capacity는 2배씩 증가
void load_names( FILE *fp, int year_index, tNames *names){
	//문자열 읽는데 사용한 변수들
	char tmp[30], tmp_name[20], tmp_sex;
	int tmp_freq;
	
	while(!feof(fp)){
		//사용변수들 초기화
		int i=0, check = -1, len = names->len, capacity = names -> capacity;
		
		// ,를 기준으로 문자열 split하기
		fscanf(fp, "%s\n", tmp);
		char* token = strtok(tmp, ",");
		strcpy(tmp_name, token);
		token = strtok(NULL, ",");
		tmp_sex = token[0];
		token = strtok(NULL, ",");
		tmp_freq = atoi(token);

		//추가하려는 (이름,성별)이 이미 존재하는지 확인
		for(i = 0; i < len; i++){
			if(!strcmp(tmp_name, names->data[i].name) && (tmp_sex == names->data[i].sex)){
				check = i;
				break;
			}
		}
		
		//해당 (이름,성별)이 존재하지 않을때
		if(check == -1){
			//capacity가 부족할때 2배로 들리기
			if(len == capacity){
				names->capacity *= 2;
				names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
			}
			//새로운 객체 추가하기
			strcpy(names->data[len].name,tmp_name);
			names->data[len].sex = tmp_sex;
			names->data[len].freq[year_index] = tmp_freq;
			names->len += 1;
		}
		//해당 (이름,성별)이 존재할때
		else{
			names->data[check].freq[year_index] = tmp_freq;
		}
	}
}

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year){
	int i=0, j=0;
	for(i; i < names->len; i++){
		printf("%s\t%c\t", names->data[i].name, names->data[i].sex);
		for (j=0; j < MAX_YEAR_DURATION; j++){
			if(!names->data[i].freq[j]) printf("%d\t",0);
			else printf("%d\t",names->data[i].freq[j]);
		}
		printf("\n");
	}
}

// qsort를 위한 비교 함수
int compare( const void *n1, const void *n2){
	char* c1 = ((tName*)n1)->name;
	char* c2 = ((tName*)n2)->name;
	
	if(strcmp(c1,c2)) return strcmp(c1,c2);
	
	if( ((tName*)n1)->sex == ((tName*)n2)->sex ) return 0;
	else return ( ((tName*)n1)->sex > ((tName*)n2)->sex ) ? 1 : -1;
}