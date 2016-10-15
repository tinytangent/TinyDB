
#include<bitset>
#include<malloc.h>
#include<iostream>
#include<cmath>
#include<cstdlib>
#include<cstdio>
#include<algorithm>

#define uint_64 unsigned long long
using namespace std;
uint_64 initialize();
uint_64 find_son_left(uint_64 father);//loc2loc
uint_64 find_son_right(uint_64 father);//loc2loc
uint_64 find_father(uint_64 son);//loc2loc
uint_64 find_brother(uint_64 loc);
uint_64 compute_size(bitset<117448696> *p,uint_64 loc);//loc2(size-in-pow-type)
void set_size(bitset<117448696> *p,uint_64 loc,uint_64 size);
uint_64 no2loc(uint_64 number);
uint_64 loc2no(uint_64 location);
uint_64 loc2file(uint_64 loc);
void update_size(bitset<117448696> *p,uint_64 loc);
long long allocate(bitset<117448696> *p,uint_64 size);
bool free(uint_64 start,uint_64 size);

long long allocate(bitset<117448696> *p,uint_64 size)
{
	uint_64 loc=0;
	while(loc<83894264)
	{
		if(compute_size(p,loc)==0||(1<<(compute_size(p,loc)+4))<size)
		{
			if(loc==0)
			{
				return -1;
			}
			else
			{
				loc=find_son_right(find_father(loc));
			}
		}
		else
		{
			loc=find_son_left(loc);
			if((1<<(compute_size(p,loc)+4))<size)
			break;
		}
	}
	//cout<<loc2file(loc)<<endl;
	set_size(p,loc,0);
	update_size(p,loc);
	return loc2file(loc);
}
uint_64 find_brother(uint_64 loc)
{
	if(find_son_left(find_father(loc))==loc)
	{
		return find_son_right(find_father(loc));
	}
	else
	{
		return find_son_left(find_father(loc));
	}
}
void update_size(bitset<117448696> *p,uint_64 loc)
{
	if(loc)
	{
		set_size(p,find_father(loc),max(compute_size(p,loc),compute_size(p,find_brother(loc))));
		update_size(p,find_father(loc));
	}
}
uint_64 loc2file(uint_64 loc)
{
	while(loc<83894264)
	{
		loc=find_son_left(loc);
	}
	return (loc-83894264)*32;
}
uint_64 initialize()
{
	bitset<117448696> *p=new bitset<117448696>;
	p->reset();
	//(*p)[1]=100;
	cout<<loc2no(117448696)<<endl;
	//for(uint_64 i=0;i<67108863;i++)
	//{
	//	set_size(p,no2loc(i),1);
	//}
	uint_64 j=0;
	for(int i=0;i<26;i++)
	{
		for(;j<(1<<(i+1))-1;j++)
		{
			set_size(p,no2loc(j),26-i);
		}
	}
	cout<<"ok\n";
	cout<<find_son_left(0)<<find_son_right(0)<<endl;
	cout<<allocate(p,1234)<<endl ;
	cout<<allocate(p,32)<<endl ;
    return 0;
	//p->flip();
	//cout<log2(10)<<" "<<endl;
}
void set_size(bitset<117448696> *p,uint_64 loc,uint_64 size)
{
	uint_64 location=loc;
	if(location<2047*8)
	{
		if(location==8*(location/8))
		{
			for(int i=8;i>0;i--)
			{
				(*p)[loc+8-i]=size&(1<<i);
			}
		}
		else
		{
			exit(10);
		}
	}
	location-=2047*8;
	if(location<8386560*4)
	{
		if(location==4*(location/4))
		{
			for(int i=4;i>0;i--)
			{
				(*p)[loc+4-i]=size&(1<<i);
			}
		}
		else
		{
			exit(11);
		}
	}
	location-=8386560*4;
	if(location<25165824*2)
	{
		if(location==2*(location/2))
		{
			for(int i=2;i>0;i--)
			{
				(*p)[loc+2-i]=size&(1<<i);
			}
		}
		else
		{
			exit(12);
		}
	}
	(*p)[loc]=size;
}
uint_64 compute_size(bitset<117448696> *p,uint_64 loc)
{
	uint_64 location=loc;
	if(location<2047*8)
	{
		if(location==8*(location/8))
		{
			uint_64 num=0;
			for(int i=7;i>-1;i--)
			{
				num+=((*p)[loc+7-i])<<i;
			}
			return num;
		}
		else
		{
			exit(7);
		}
	}
	location-=2047*8;
	if(location<8386560*4)
	{
		if(location==4*(location/4))
		{
			uint_64 num=0;
			for(int i=3;i>-1;i--)
			{
				num+=((*p)[loc+3-i])<<i;
			}
			return num;
		}
		else
		{
			exit(8);
		}
	}
	location-=8386560*4;
	if(location<25165824*2)
	{
		if(location==2*(location/2))
		{
			uint_64 num=0;
			for(int i=1;i>-1;i--)
			{
				num+=((*p)[loc+1-i])<<i;
			}
			return num;
		}
		else
		{
			exit(9);
		}
	}
	return (*p)[loc];
}
uint_64 find_son_left(uint_64 father)
{
	return no2loc(loc2no(father)*2+1);
}
uint_64 find_son_right(uint_64 father)
{
	return no2loc(loc2no(father)*2+2);
}
uint_64 find_father(uint_64 son)
{
	return no2loc((loc2no(son)-1)/2);
}
uint_64 loc2no(uint_64 location)
{
	uint_64 num=0;
	//long long location=location1;
	if(location<0)
	{
		exit(3);
	}
	//if(location<2047*8&&(location!=8*(location/8))||location<(2047*8+8386560*4)&&(location!=4*(location/4))||location<(2047*8+8386560*4+25165824*2)&&(location!=2*(location/2)))
	//{
	//	exit(4);
	//}
	if(location<2047*8)
	{
		if(location==8*(location/8))
		{
			return location/8;
		}
		else
		{
			exit(4);
		}
	}
	location-=2047*8;
	num+=2047;
	if(location<8386560*4)
	{
		if(location==4*(location/4))
		{
			return location/4+num;
		}
		else
		{
			exit(5);
		}
	}
	location-=8386560*4;
	num+=8386560;
	if(location<25165824*2)
	{
		if(location==2*(location/2))
		{
			return location/2+num;
		}
		else
		{
			exit(6);
		}
	}
	location-=25165824*2;
	num+=25165824+location;
	return num;
}
uint_64 no2loc(uint_64 number1)//·µ»ØµÚn¸ö±ê¼ÇËùÔÚµÄÎ»ÖÃ
{
	uint_64 loc=0;
	long long number=number1;
	if(number<0)
	{
		exit(1);
	}
	loc+=(number<2047?number:2047)*8;
	if((number-=2047)<0)
	{
		return loc;
	}
	loc+=(number<8386560?number:8386560)*4;
	if((number-=8386560)<0)
	{
		return loc;
	}
	loc+=(number<25165824?number:25165824)*2;
	if((number-=25165824)<0)
	{
		return loc;
	}
	loc+=(number<33554432?number:33554432)*1;
	if((number-=33554432)<0)
	{
		return loc;
	}
	exit(2);
}
