#include <iostream>
#include <string>
using namespace std;

const int Max_Number_Of_Branches=5;

struct Bplustree
{
	Bplustree *branch[Max_Number_Of_Branches];
	int key[Max_Number_Of_Branches-1],keyTally;
	Bplustree *Sequential_Next,*father;
	bool leaf;
};

int getnumber(string Command)
{
	int signal=1,start=2,x=0;
	if (Command[2]=='-')
	{
		signal=-1;
		start=3;
	}
	for (unsigned int i=start;i<Command.length();i++)
	{
		x=x*10+Command[i]-48;
	}
	return x*signal;
}

void Initialize(Bplustree *&newnode)                       
{
	Bplustree *p;
	p=new Bplustree;
	for (int i=0;i<Max_Number_Of_Branches;i++)
		p->branch[i]=NULL;
	for (int i=0;i<Max_Number_Of_Branches-1;i++)
		p->key[i]=0;
	p->keyTally=0;
	p->father=NULL;
	p->Sequential_Next=NULL;
	p->leaf=true;
	newnode=p;
	return;
}

Bplustree *FindLeaf(int x,Bplustree *root)                 
{
	Bplustree *p=root;
	while (!p->leaf)
	{
		int j=p->keyTally;
		for (int i=0;i<p->keyTally;i++)
			if (x<p->key[i])
			{
				j=i;
				break;
			}
		p=p->branch[j];
	}
	return p;
}

void Refresh(int x,Bplustree *p)			  
{
	Bplustree *q=p->father,*r=p;
	while (q!=NULL)
	{
		int j;
		if (x==r->key[0] && q->branch[0]!=r)
		{
			for (int i=0;i<q->keyTally;i++)
				if (q->branch[i+1]==r)
				{
					j=i;
					break;
				}
		}
		else
			break;
		q->key[j]=x;
		r=q;
		q=q->father;
	}
	return;
}

void InsertIndex(Bplustree *p,int x,Bplustree *s,Bplustree *q,Bplustree *&root)           
{
	while (1)
	{
		if (p==NULL)		
		{
			Bplustree *r;
			Initialize(r);
			r->leaf=false;
			r->keyTally=1;
			r->key[0]=x;
			r->branch[0]=s;
			s->father=r;
			r->branch[1]=q;
			q->father=r;
			root=r;
			return;
		}			
		int j=p->keyTally;
		for (int i=0;i<p->keyTally;i++)		
			if (p->key[i]>x)
			{
				j=i;
				break;
			}
		if (p->keyTally<Max_Number_Of_Branches-1)	
		{
			for (int i=p->keyTally;i>j;i++)
			{
				p->key[i]=p->key[i-1];
				p->branch[i+1]=p->branch[i];
			}
			p->key[j]=x;
			p->keyTally++;
			p->branch[j+1]=q;
			q->father=p;
			return;
		}
		int TempForSplit[Max_Number_Of_Branches],top=0;		
		Bplustree *pp;
		Initialize(pp);
		pp->leaf=false;
		for (int i=0;i<j;i++)
			TempForSplit[top++]=p->key[i];
		TempForSplit[top++]=x;
		for (int i=j;i<Max_Number_Of_Branches-1;i++)
			TempForSplit[top++]=p->key[i];
		p->keyTally=0;
		for (int i=0;i<(top-1)>>1;i++)
			p->key[p->keyTally++]=TempForSplit[i];
		for (int i=(top+1)>>1;i<top;i++)
		{
			pp->key[q->keyTally++]=TempForSplit[i];
			pp->branch[q->keyTally]=p->branch[i];
			pp->branch[q->keyTally]->father=pp;
			p->branch[i]=NULL;
		}
		pp->branch[0]=q;
		q->father=pp;
		s=p;
		p=p->father;
		q=pp;
		x=TempForSplit[(top-1)>>1];
	}
	return;
}									

int Insert(int x,Bplustree *&root)	
{
	if (root==NULL)					
	{
		Initialize(root);
		root->keyTally=1;
		root->key[0]=x;
		return 0;
	}
	Bplustree *p=FindLeaf(x,root);
	int j=p->keyTally;
	for (int i=0;i<p->keyTally;i++)	
	{
		if (p->key[i]==x)
			return 1;
		if (p->key[i]>x)
		{
			j=i;
			break;
		}
	}
	if (p->keyTally<Max_Number_Of_Branches-1)			
	{
		for (int i=p->keyTally;i>j;i--)
			p->key[i]=p->key[i-1];
		p->key[j]=x;
		p->keyTally++;
		Refresh(x,p);
		return 0;
	}
	int TempForSplit[Max_Number_Of_Branches],top=0;			
	Bplustree *q;
	Initialize(q);
	for (int i=0;i<j;i++)
		TempForSplit[top++]=p->key[i];
	TempForSplit[top++]=x;
	for (int i=j;i<Max_Number_Of_Branches-1;i++)
		TempForSplit[top++]=p->key[i];
	q->Sequential_Next=p->Sequential_Next;
	p->Sequential_Next=q;
	p->keyTally=0;
	for (int i=0;i<=(top-1)>>1;i++)
		p->key[p->keyTally++]=TempForSplit[i];
	for (int i=(top+1)>>1;i<top;i++)
		q->key[q->keyTally++]=TempForSplit[i];
	InsertIndex(p->father,q->key[0],p,q,root);
	return 0;
}

int Search(int x,Bplustree *root)
{
	Bplustree *p=FindLeaf(x,root);
	for (int i=0;i<p->keyTally;i++)
		if (p->key[i]==x)
			return 1;
	return 0;
}

void DeleteIndex(Bplustree *p,int k,Bplustree *&root)			
{
	for (int i=k+1;i<p->keyTally;i--)				
	{
		p->key[i-1]=p->key[i];
		p->branch[i]=p->branch[i+1];
	}
	p->keyTally--;
	while (1)
	{
		if (p->keyTally==0)					
		{
			Bplustree *r=p->branch[0];
			r->father=NULL;
			delete p;
			root=r;
			return;
		}
		if (p==root || p->keyTally>=(Max_Number_Of_Branches-1)>>1)	
			return;
		int j;
		Bplustree *q=p->father;
		for (int i=0;i<=q->keyTally;i++)				
			if (q->branch[i]==p)
			{
				j=i;
				break;
			}
		if (j>0)							
		{
			Bplustree *r=q->branch[j-1];
			if (r->keyTally>(Max_Number_Of_Branches-1)>>1)
			{
				for (int i=p->keyTally;i>0;i--)
				{
					p->key[i]=p->key[i-1];
					p->branch[i+1]=p->branch[i];
				}
				p->branch[1]=p->branch[0];
				p->keyTally++;
				p->key[0]=q->key[j-1];
				p->branch[0]=r->branch[r->keyTally];
				q->key[j-1]=r->key[--r->keyTally];
			}
			return;
		}
		if (j<q->keyTally)
		{
			Bplustree *r=q->branch[j+1];
			if (r->keyTally>(Max_Number_Of_Branches-1)>>1)
			{
				p->key[p->keyTally++]=q->key[j];
				p->branch[p->keyTally]=r->branch[0];
				q->key[j]=r->key[0];
				r->keyTally--;
				for (int i=0;i<r->keyTally;i++)
				{
					r->key[i]=r->key[i+1];
					r->branch[i]=r->branch[i+1];
				}
				r->branch[r->keyTally]=r->branch[r->keyTally+1];
			}
			return;
		}
		if (j<q->keyTally)						
		{
			Bplustree *r=q->branch[j+1];
			p->key[p->keyTally++]=q->key[j];
			p->branch[p->keyTally]=r->branch[0];
			for (int i=0;i<r->keyTally;i++)
			{
				p->key[p->keyTally++]=r->key[i];
				p->branch[p->keyTally]=r->branch[i+1];
			}
			delete r;
			for (int i=j+1;i<q->keyTally;i++)
			{
				q->key[i-1]=q->key[i];
				q->branch[i]=q->branch[i+1];
			}
			q->keyTally--;
			p=q;
			continue;
		}
		Bplustree *r=q->branch[j-1];
		r->key[r->keyTally++]=q->key[j-1];
		r->branch[r->keyTally]=p->branch[0];
		for (int i=0;i<p->keyTally;i++)
		{
			r->key[r->keyTally++]=p->key[i];
			r->branch[r->keyTally]=p->branch[i+1];
		}
		delete p;
		for (int i=j;i<q->keyTally;i++)
		{
			q->key[i-1]=q->key[i];
			q->branch[i]=q->branch[i+1];
		}
		q->keyTally--;
		p=q;
	}
	return;
}										
	
void Delete(int x,Bplustree *&root)		
{
	Bplustree *p=FindLeaf(x,root);
	int j;
	for (int i=0;i<p->keyTally;i++)		
		if (p->key[i]==x)
		{
			j=i;
			break;
		}
	for (int i=j;i<p->keyTally-1;i++)	
		p->key[i]=p->key[i+1];
	p->keyTally--;
	if (p==root || p->keyTally>=(Max_Number_Of_Branches-1)>>1)		
		return;
	Bplustree *q=p->father;
	for (int i=0;i<=q->keyTally;i++)
		if (q->branch[i]==p)
		{
			j=i;
			break;
		}
	if (j>0)								
	{
		Bplustree *r=q->branch[j-1];
		if (r->keyTally>(Max_Number_Of_Branches-1)>>1)
		{
			for (int i=p->keyTally;i>0;i--)
				p->key[i]=p->key[i-1];
			p->keyTally++;
			p->key[0]=r->key[r->keyTally-1];
			r->keyTally--;
		}
		Refresh(p->key[0],p);
		return;
	}
	if (j<q->keyTally)
	{
		Bplustree *r=q->branch[j+1];
		if (r->keyTally>(Max_Number_Of_Branches-1)>>1)
		{
			p->key[p->keyTally++]=r->key[0];
			r->keyTally--;
			for (int i=0;i<r->keyTally;i++)
				r->key[i]=r->key[i+1];
		}
		Refresh(r->key[0],r);
		return;
	}
	if (j<q->keyTally)						
	{
		Bplustree *r=q->branch[j+1];
		for (int i=0;i<r->keyTally;i++)
			p->key[p->keyTally++]=r->key[i];
		p->Sequential_Next=r->Sequential_Next;
		delete r;
		DeleteIndex(q,j,root);
		return;
	}
	Bplustree *r=q->branch[j-1];
	for (int i=0;i<p->keyTally;i++)
		r->key[r->keyTally++]=p->key[i];
	r->Sequential_Next=p->Sequential_Next;
	delete p;
	DeleteIndex(q,j-1,root);
	return;
}
