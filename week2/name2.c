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
// 주의사항: 구조체 배열은 정렬 리스트(ordered list)이어야 함
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 2배씩 증가
void load_names( FILE *fp, int year_index, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// bsearch를 위한 비교 함수
int compare( const void *n1, const void *n2);

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

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
		
		fclose( fp);
	}
	
	// 이름 구조체를 화면에 출력
	print_names( names, num_year);

	// 이름 구조체 해제
	destroy_names(names);
	
	return 1;
}

void load_names( FILE *fp, int year_index, tNames *names){
	//문자열 읽는데 사용한 변수들
	char tmp[30], tmp_name[20], tmp_sex;
	int tmp_freq, idx = 0;
	
	while(!feof(fp)){
		// ,를 기준으로 문자열 split하기
		fscanf(fp, "%s\n", tmp);
		char* token = strtok(tmp, ",");
		strcpy(tmp_name, token);
		token = strtok(NULL, ",");
		tmp_sex = token[0];
		token = strtok(NULL, ",");
		tmp_freq = atoi(token);
		
		//삽입해야되는 key데이터 구조체 생성
		tName key;
		strcpy(key.name, tmp_name);
		key.sex = tmp_sex;

		//key데이터 구조체가 들어가야할 위치 찾기
		idx = binary_search(&key, names->data, names->len, sizeof(tName), compare);
		
		//데이터 공간이 부족한 경우 재할당
		if (names->len == names->capacity){
			names -> capacity *= 2;
			names->data = (tName *)realloc(names->data, names->capacity * sizeof(tName));
		}
		
		//해당 (이름,성별)이 존재할때
		if ( idx < 0 ) {
			idx = (idx + 1) * -1 ;
			names->data[idx].freq[year_index] = tmp_freq;
		}
		// 해당 (이름,성별)이 존재하지 않을때, memmove 함수를 이용해서 데이터 삽입
		else{
			memmove( &names->data[idx+1], &names->data[idx], sizeof(tName)*(names->len - idx));
			strcpy(names->data[idx].name,tmp_name);
			names->data[idx].sex = tmp_sex;
			for(int i = 0; i < 10; i++)  names->data[idx].freq[i] = 0;
			names->data[idx].freq[year_index] = tmp_freq;
			names->len += 1;
		}
	}
}

int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *)){
	if(nmemb == 0) return 0;
	
	int low = 0, high = nmemb - 1, mid;
	
	while(low <= high){
		mid = (low + high) / 2;
		if ( !compare( base + mid*size , key) ) return -mid -1;
		else if ( compare( base + mid*size, key) > 0 ) high = mid - 1;
		else low = mid + 1;
	}
	return low;
}

int compare( const void *n1, const void *n2){
	char* c1 = ((tName*)n1)->name;
	char* c2 = ((tName*)n2)->name;
	
	if(strcmp(c1,c2)) return strcmp(c1,c2);
	
	if( ((tName*)n1)->sex == ((tName*)n2)->sex ) return 0;
	else return ( ((tName*)n1)->sex > ((tName*)n2)->sex ) ? 1 : -1;
}

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
