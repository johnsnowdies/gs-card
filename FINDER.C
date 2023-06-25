#include <math.h>
#include <alloc.h>
#include <limits.h>
#include "structs.h"
#include "finder.h"
#include "ad.h"

int S;
unsigned char gotEnd;

int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

int GetWay(int ptrSize, SYSTEM *ptrList, WAYPOINT *wp)
{
	char *input;
	int start, end,i,status;

	for (i = 0; i < ptrSize; i++)
	{
		ptrList[i].rate = 0;
		ptrList[i].visited = 0;
	}

	wp->size = 0;

	S = 1;
	gotEnd = 0;
	input = (char*) questionWnd("GSCARD","Enter START system:");
	start = atoi(input);

	input = (char*) questionWnd("GSCARD","Enter END system:");
	end = atoi(input);

	if (start > 0 && end > 0 && start < ptrSize && end < ptrSize){

		/* DIRECT DIRECTION*/
		RunWave(ptrSize, ptrList, start, end);
		if (gotEnd){
			status = RestorePath(ptrSize,ptrList, wp, start, end,1);
		}

		if(!gotEnd || !status ){
			warningWnd("GSCARD", "No way!");
			getch();
			return 0;
		}

		return 1;

	}
	else
	{
		warningWnd("GSCARD","Wrong systems!");
		getch();
	}

	return 0;
}

int RestorePath(int ptrSize, SYSTEM *ptrList, WAYPOINT *wp, int start, int end, int reverse)
{
	int i,j,k,c,cnt=1,nbCnt,prev;
	int min;
	SYSTEM b,a;
	min=S;

	c = end;

	for (i = 0; i < 20; i++)
	{
		wp->way[i] = 0;
	}

	wp->way[0] = end;

	while(c != start && cnt < 20){
		prev = c;
		a = ptrList[c];

		if (ptrList[c].threadSize){

			for (i = 0; i < ptrList[c].threadSize ; i++)
			{
				k = ptrList[c].threads[i].value;

				if(k > ptrSize || k < 0)
				{
					printf("RP: Thread endpoint system out of index\n");
					printf("Thread size=%d\n",ptrList[k].threadSize);
					printf("Thread index=%d\n",i);
					printf("item=%d\n",k);
					exit(1);
				}

				b = ptrList[k];

				if (b.rate < min && b.rate !=0)
				{
						c = k;
						wp->way[cnt] = k;
						min = ptrList[k].rate;
				}
			}
		}

		if(prev == c)
		{
			/* cant move, error */
			return 0;
		}

		cnt++;
	}

	if (cnt > 1){
		int buf[50];
		wp->size = cnt;

		if (reverse){
			for (i = 0,j=(cnt-1); i < cnt; i++,j--)
			{
				buf[i] = wp->way[j];
			}

			for (i = 0; i < cnt; i++){
				wp->way[i] = buf[i];
			}
		}
	}

	return 1;
}

void RunWave(int ptrSize, SYSTEM *ptrList, int c, int end)
{
	int i,j,k;
	SYSTEM m;

	ptrList[c].rate = S;
	ptrList[c].visited = 1;
	S++;

	if (S > 900){
		return;
	}

	for(i = 0; i < ptrList[c].threadSize; i++)
	{
		k = ptrList[c].threads[i].value;
		m = ptrList[k];

		if ( m.rate == 0){
			m.rate = S;
		}
	}

	for(i = 0; i < ptrList[c].threadSize; i++)
	{
		k = ptrList[c].threads[i].value;
		m = ptrList[k];

		if ( k != end && m.visited == 0){
			RunWave(ptrSize, ptrList, k, end);
		}

		if (k == end){
			gotEnd = 1;
		}
	}
}

void CalculateHyperThreads(int ptrSize, SYSTEM *ptrList)
{
	int i=0,j=0,k=0,cnt=0,n,m,e;
	int error = 0;
	int buffer[15];
	double dx,dy,dz,S;

	SYSTEM a,b,c,d;

	for (i = 0; i < ptrSize; i++)
	{
		cnt = 0;
		a = ptrList[i];
		a.threadSize = 0;

		progressWnd("GSCARD","Processing hyper-threads calculation",i,ptrSize);

		for (j = 0; j < ptrSize; j++)
		{

			if (j == i)
				continue;

			b = ptrList[j];
			error = 0;

			dx = (double)(a.x - b.x);
			if(dx!=0)
				dx = pow(dx,2.0);

			dy = (double)(a.y - b.y);

			if(dy!=0)
				dy = pow(dy,2.0);

			dz = (double)(a.z - b.z);

			if(dz!=0)
			   dz = pow(dz,2.0);

			S = dx + dy + dz;
			S = sqrt(S);

			if (S > 130)
			{
				error = 1;
			}

			if (!error && cnt < 15){
				buffer[cnt] = j;
				cnt++;
			}
		}

		if (cnt){

			ptrList[i].threadSize = cnt;

			if( (ptrList[i].threads = (THREAD*) calloc(cnt,sizeof(THREAD))) == NULL)
			{
				printf("HT: Memory allocation error!");
				exit(1);
			}

			for (k=0; k < 15; k++){
				ptrList[i].threads[k].value = buffer[k];
				ptrList[i].threads[k].cost = 0;
			}
		}else{
			ptrList[i].threadSize = 0;
		}
	}
/*
	ClearThreadCosts(ptrSize, ptrList);
	CalculateThreadCosts(ptrSize, ptrList, 10);*/
}

void ClearThreadCosts(int ptrSize, SYSTEM *ptrList)
{
	int i,k;

	for (i = 0; i < ptrSize; i++)
	{
		for (k = 0; k < ptrList[i].threadSize; k++){
			ptrList[i].threads[k].cost = 0;
		}
	}
}

void CalculateThreadCosts(int ptrSize, SYSTEM *ptrList, int topCost)
{

	int i,j,n,m,k,e;
	SYSTEM a,b,c,d;
	char wndLabel[50] = "";

	sprintf(wndLabel,"Processing hyper-threads crossing: %d", topCost);

	for (j = 0; j < ptrSize; j++)
	{
		a = ptrList[j];

		progressWnd("GSCARD",wndLabel,j,ptrSize);

		/* All threads for 'a' */
		for (i = 0; i < a.threadSize; i++)
		{
			k = a.threads[i].value;
			b = ptrList[k];

			/* a -> b */

			for (n = 0; n < ptrSize; n++)
			{
				c = ptrList[n];

				/* All threads for 'c' */
				for (m = 0; m < c.threadSize; m++)
				{
					int p[3][3];
					int cross;

					e = c.threads[m].value;
					d = ptrList[e];

					p[0][0] = (b.x - a.x);
					p[0][1] = (b.y - a.y);
					p[0][2] = (b.z - a.z);

					p[1][0] = (d.x - c.x);
					p[1][1] = (d.y - c.y);
					p[1][2] = (d.z - c.z);

					p[2][0] = (b.x - c.x);
					p[2][1] = (b.y - c.y);
					p[2][2] = (b.z - c.z);

					cross = p[0][0] * p[1][1] * p[2][2] +
							p[0][1] * p[1][2] * p[2][0] +
							p[0][2] * p[1][0] * p[2][1] -
							p[0][0] * p[1][2] * p[2][1] -
							p[0][1] * p[1][0] * p[2][2] -
							p[0][2] * p[1][1] * p[2][0];

					if (cross == 0)
					{
						double dt,abxy, abz, cdxy, cdz;
						double t,checkLeft,checkRight;

						SYSTEM target;

						t = ((d.x - c.x)*(a.y - c.y) - (d.y - c.y)*(a.x - c.x));
						dt =((d.y - c.y)*(b.x - a.x) - (b.y - a.y)*(d.x - c.x));

						if (dt != 0.0)
						{
							t = (int)t/dt;

							target.x = (b.x - a.x) * t + a.x;
							target.y = (b.y - a.y) * t + a.y;
							target.z = (b.z - a.z) * t + a.z;

							checkLeft =	((d.y - c.y)*((b.x - a.x)*t + a.x - c.x));
							checkRight = ((d.x - c.x)*((b.y - a.y)*t + a.y - c.y));

							abxy = (target.x - a.x)*(b.y - a.y) - (target.y - a.y)*(b.x - a.x);

							if (b.z - a.z != 0)
								abz = (target.z - a.z)/(b.z - a.z);
							else
								abz = 0;


							cdxy = (target.x - c.x)*(d.y - c.y) - (target.y - c.y)*(d.x - c.x);

							if (d.z - c.z != 0)
								cdz = (target.z - c.z)/(d.z - c.z);
							else
								cdz = 0;

							if (abxy == abz && cdxy == cdz){
								int iq;

								a.threads[i].cost++;
								ptrList[j] = a;

								for(iq = 0; iq < c.threadSize; iq++)
								{
									if (c.threads[iq].value == e && c.threads[iq].cost == 0)
									{
										c.threads[iq].cost--;
										ptrList[n] = c;
									}


								}

								for(iq = 0; iq < d.threadSize; iq++)
								{
									if (d.threads[iq].value == n && d.threads[iq].cost == 0)
									{
										d.threads[iq].cost--;
									}


								}
							}
						}
					}
				}
			}
		}
	}


    for (j = 0; j < ptrSize; j++)
	{
		int newSize=0;
		a = ptrList[j];

		progressWnd("GSCARD","Reallocating memory",j,ptrSize);

		for (i = 0; i < a.threadSize; i++)
		{
			if (a.threads[i].cost < topCost)
			{
				newSize++;
			}
		}

		if (newSize < a.threadSize && newSize != 0)
		{
			THREAD *newThreads;
			int index = 0;

			if ( (newThreads = (THREAD*) malloc(newSize * sizeof(THREAD))) == NULL)
			{
				printf("HT: Cant allocate memory!");
				exit(1);
			}

			for (i = 0; i < newSize; i++)
			{
				while(a.threads[index].cost > topCost){
					index++;
				}

				newThreads[i] = a.threads[index];
				index++;
			}

			free(a.threads);

			a.threads = newThreads;
			a.threadSize = newSize;
		}

		ptrList[j] = a;
	}
}