#include<bitset>
#include<malloc.h>
#include<iostream>
#include<cmath>
#include<cstdlib>
#include<ctime>
#include<cstdio>
using namespace std;
uint64_t initialize();
uint64_t find_son_left(uint64_t father);//loc2loc
uint64_t find_son_right(uint64_t father);//loc2loc
uint64_t find_father(uint64_t son);//loc2loc 
uint64_t find_brother(uint64_t loc);
uint64_t compute_size(bitset<117448696> *p,uint64_t loc);//loc2(size-in-pow-type)
void set_size(bitset<117448696> *p,uint64_t loc,uint64_t size);
uint64_t no2loc(uint64_t number);
uint64_t loc2no(uint64_t location);
uint64_t loc2file(uint64_t loc);//表中位置到文件中的位置 
bool free(uint64_t file_loc,uint64_t size);//释放文件中位于file_loc、大小为size的地址 
void update_size(bitset<117448696> *p,uint64_t loc);
long long allocate(bitset<117448696> *p,uint64_t size);//分配到文件的位置 分配不了返回-1 
bool free(uint64_t start,uint64_t size);
int main()
{
	uint64_t a=0;
	initialize();
	//cout<<sizeof(bitset<117448696>)<<endl;
} 
long long allocate(bitset<117448696> *p,uint64_t size)
{
	uint64_t loc=0,size_raw=size;
	if(size<32)
	{
		size=32;
	}
	if((1<<(compute_size(p,loc)+4))<size)
	{
		return -1;
	}
	while((1<<(compute_size(p,loc)+4))>=size)
	{
		if(loc>83894263)
		{
			break;
		}
		if((1<<(compute_size(p,find_son_left(loc))+4))<size&&(1<<(compute_size(p,find_son_right(loc))+4))<size)
		{
			break;
		}
		if((1<<(compute_size(p,find_son_left(loc))+4))>=size)
		{
			loc=find_son_left(loc);
			continue;
		}
		if((1<<(compute_size(p,find_son_right(loc))+4))>=size)
		{
			loc=find_son_right(loc);
			continue;
		}
		return -2;
	}
	//cout<<loc2file(loc)<<endl; 
	set_size(p,loc,0);
	//cout<<loc<<" "<<compute_size(p,loc)<<endl;
	//cout<<"allocate size "<<size_raw<<" in file, loc "<<loc2file(loc)<<endl;
	update_size(p,loc);
	return loc2file(loc); 
}
uint64_t find_brother(uint64_t loc)
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
void update_size(bitset<117448696> *p,uint64_t loc)
{
	
	if(loc)
	{
		uint64_t size=0,size_loc=compute_size(p,loc),size_brother=compute_size(p,find_brother(loc));
		int max_loc_size=26-(int)log2(loc2no(loc)+1);
		if(size_loc==max_loc_size&&size_brother==max_loc_size)
		{
			size=max_loc_size+1;
		}
		else
		{
			size=max(size_loc,size_brother);
		} 
		set_size(p,find_father(loc),size);
		//cout<<"set_size"<<find_father(loc)<<"to"<<max(compute_size(p,loc),compute_size(p,find_brother(loc)))<<endl;
		update_size(p,find_father(loc));
	}
}
uint64_t loc2file(uint64_t loc)
{
	while(loc<83894264)
	{
		loc=find_son_left(loc);
	}
	return (loc-83894264)*32;
}
uint64_t initialize()
{
	bitset<117448696> *p=new bitset<117448696>;
	p->reset();
	//(*p)[1]=100;
	cout<<loc2no(117448696)<<endl;
	//for(uint64_t i=0;i<67108863;i++)
	//{
	//	set_size(p,no2loc(i),1);
	//}
	uint64_t j=0;
	for(int i=0;i<26;i++)
	{
		for(;j<(1<<(i+1))-1;j++)
		{
			set_size(p,no2loc(j),26-i);
		}
	}
	cout<<"ok\n";
	srand((int)time(0));
	int size_sum=0,size_now;
    for(int x=0;x<100000;x++)
	{
		size_now=rand()%10000;
		allocate(p,size_now);
		size_sum+=size_now; 
	} 
	cout<<size_sum<<endl;
		//cout<<find_son_left(0)<<find_son_right(0)<<endl;
	//p->flip();
	//cout<log2(10)<<" "<<endl;
}
void set_size(bitset<117448696> *p,uint64_t loc,uint64_t size)
{
	uint64_t location=loc;
	
	if(location<2047*8)
	{
		if(location==8*(location/8))
		{
			for(int i=7;i>=0;i--)
			{
				(*p)[loc+7-i]=size&(1<<i);
			}
			return;
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
			for(int i=3;i>=0;i--)
			{
				(*p)[loc+3-i]=size&(1<<i);
			}
			return;
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
			for(int i=1;i>=0;i--)
			{
				(*p)[loc+1-i]=size&(1<<i);
			}
			return;
		}
		else
		{
			exit(12);
		}
	}
	(*p)[loc]=size;
}
uint64_t compute_size(bitset<117448696> *p,uint64_t loc)
{
	uint64_t location=loc;
	if(location<2047*8)
	{
		if(location==8*(location/8))
		{
			uint64_t num=0;
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
			uint64_t num=0;
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
			uint64_t num=0;
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
uint64_t find_son_left(uint64_t father)
{
	return no2loc(loc2no(father)*2+1);
}
uint64_t find_son_right(uint64_t father)
{
	return no2loc(loc2no(father)*2+2);
}
uint64_t find_father(uint64_t son)
{
	return no2loc((loc2no(son)-1)/2);
}
uint64_t loc2no(uint64_t location)
{
	uint64_t num=0;
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
uint64_t no2loc(uint64_t number1)//返回第n个标记所在的位置 
{
	uint64_t loc=0; 
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
