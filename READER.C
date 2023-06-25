#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "structs.h"
#include "reader.h"

int getFileSize(char *filename)
{
	FILE * fp;
	char c;
	int counter = 0;

	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("ERROR: NO FILE\n");
		exit(1);
	}

	while ( (c = fgetc(fp)) != EOF)
	{
		if (c == '\n'){
			counter++;
		}
	}

	fclose(fp);

	return counter;
}

void loadSolarFile(SYSTEM *list)
{
	FILE * fp;
	char c[100];
	int ptrSolar;
	int counter = 0;

	if ((fp = fopen("system.sol", "r")) == NULL)
	{
		printf("ERROR: NO SOLAR FILE\n");
		exit(1);
	}

	while (fgets(c, sizeof c, fp) != NULL)
	{
		int len = strlen(c);
		int delimiter_pos = 0;
		int i,valueCnt = 0;

		char str_x[10] = "";
		char str_y[10] = "";
		char str_z[10] = "";

		for(i=0; i < len; i++)
		{
		  if(c[i] == DELIMITER_CHAR)
		  {
			delimiter_pos = i;
			valueCnt++;
			continue;
		  }

		  if (valueCnt == 0)
		  {
			str_x[i] = c[i];
		  }

		  if (valueCnt == 1)
		  {
			if(i < (len-1))
				str_y[i-(delimiter_pos+1)] = c[i];
		  }

		  if (valueCnt == 2)
		  {
			if(i < (len-1))
				str_z[i-(delimiter_pos+1)] = c[i];
		  }

		}

		list[counter].x = atoi(str_x);
		list[counter].y = atoi(str_y);
		list[counter].z = atoi(str_z);

		counter++;
	}


	fclose(fp);

}

void saveFile(int ptrSize, SYSTEM *ptrList)
{
	FILE *fp;
	int i,limit;
	long x,y,z,dz;

	if( (fp = fopen("new.sol","w")) == NULL){
		printf("IO ERROR");
		exit(1);
	}


	for (i=0; i<ptrSize; i++)
	{
		limit = 1000/25;

		x = ptrList[i].x;
		y = ptrList[x].y;

		if (x <= 500 && y <=500 && x >= -500 && y >= -500) limit = 1400/20;
		if (x <= 400 && y <=400 && x >= -400 && y >= -400) limit = 1400/16;
		if (x <= 300 && y <=300 && x >= -300 && y >= -300) limit = 1400/12;
		if (x <= 200 && y <=200 && x >= -200 && y >= -200) limit = 1400/8;
		if (x <= 100 && y <=100 && x >= -100 && y >= -100) limit = 1400/6;

		z =  rand()  % (limit + 1 + limit) -limit;

		fprintf(fp,"%ld;%ld;%ld\n",ptrList[i].x, ptrList[i].y, z);
		ptrList[i].y = z;

	}

	getch();
	fclose(fp);
}