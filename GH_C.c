#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<malloc.h>
#define SIZE 201

struct BigInt
{
	int len;
	char* val;
};

struct BigInt getnum(void);
struct BigInt addnum(struct BigInt n1,struct BigInt n2,int s);
char pick_digit(struct BigInt a, int i);
struct BigInt subnum(struct BigInt n1, struct BigInt n2);
struct BigInt multinum(struct BigInt n1, struct BigInt n2,int s);
char get_first(void);
struct BigInt(*get_operator(void))(struct BigInt, struct BigInt, int);
void divide_big(struct BigInt x, int i, struct BigInt* xlow, struct BigInt* xhigh);
void xprintf(struct BigInt a);

int main(void)
{
	struct BigInt n1,n2;
	struct BigInt(*operator)(struct BigInt,struct BigInt,int);
	
	printf("Now, we have a powerful calculator that can help you get the answer of \naddition or multiplication between two large numbers less than %d digits.\n",SIZE-1);
	printf("Any key to continue or 'q'to quit.\n");
	while (get_first() != 'q')
	{
		printf("Please enter your first number.\n");
		while ((n1 = getnum()).len == 0) printf("Please mind your entering. Digits only and not more than %d digits.\n",SIZE-1);
		printf("Please enter your second number.\n");
		while ((n2 = getnum()).len == 0) printf("Please mind your entering. Digits only and not more than %d digits.\n",SIZE-1);
		if (n1.len<n2.len)
		{
			struct BigInt p = n1;
			n1 = n2;
			n2 = p;
		}
		printf("Please enter  your operator you choose.\n");
		operator = get_operator();
		xprintf(operator(n1, n2, 0));
		free(n1.val), free(n2.val);
		printf("Now you get the answer.\nAny key to continue or 'q'to quit.\n");
	}
	printf("Thank you for your using. Have a good day!\n");
	return 0;
}

char get_first(void)
{
	char ch;
	ch = getchar();
	while (getchar() != '\n')
		continue;
	return ch;
}

struct BigInt(*get_operator(void))(struct BigInt, struct BigInt, int)
{
	char ch;
	struct BigInt(*operator)(struct BigInt, struct BigInt, int)=NULL;
	while (!((ch = get_first()) == '+' || ch == '*'))
		printf("Please mind your entering. We only can calculate addition and mulitipulation.\n");
	if (ch == '+') operator = addnum;
	else if (ch == '*') operator = multinum;
	return operator;
}

struct BigInt getnum(void)
{
	int len,flage;
	char n[SIZE];
	struct BigInt x;
	flage = scanf_s("%s", n, sizeof(char)*SIZE);
	while (getchar() != '\n')
		continue;
	for (; n[0] == '0';)
	{
		int k;
		for ( k = 1; n[k] != '\0'; k++)
		{
			n[k - 1] = n[k];
		}
		n[k - 1] = '\0';
	}
	char* p = n;
	for (; flage == 1 && *p >= '0' && *p <= '9';p++);
	if (*p != '\0' || flage == 0)
	{
		x.len = 0;
		return x;
	}
	else
	{
		len = p - n;
		p -= 1;//now p is the last char
		len = len % 2 == 0 ? len / 2 : (len + 1) / 2; // add one for another digit if odd
		char* xp = (char*)calloc(len, sizeof(char));
		x.val = xp;
		x.len = len;
		while (p > n)
		{
			*xp++ = (*p - '0') + (*(p - 1) - '0') * 10;
			p -= 2;
		}
		if (p == n)
			*xp++ = *p - '0';
		return x;
	}
}

struct BigInt addnum(struct BigInt a, struct BigInt b, int n)
{
	struct BigInt x;
	int t;
	bool toadd = 0;
	if (a.len == 1 && *a.val == 0) return b; // if a == 0
	x.len = a.len + n > b.len ? a.len + n : b.len;
	x.val = (char*)calloc(x.len + 1,sizeof(char));
	for (int i = 0; i < n && b.len; i++) 
	{
		*(x.val + i) = *b.val++;
		b.len--;
	}
	for (int i = 0; i < a.len; i++)
	{
		t = pick_digit(a, i) + pick_digit(b, i) + toadd;
		if (t > 99) toadd = 1, t -= 100;
		else toadd = 0;
		*(x.val + i + n) = t;
	}
	if (toadd) *(x.val + x.len++) = toadd;// longer x here
	// don't free a and b because P1 P2 P3 need more use
	return x;
}

char pick_digit(struct BigInt a, int i)
{
	if (i < a.len) return *(a.val + i);
	else return 0;
}

struct BigInt subnum(struct BigInt a, struct BigInt b)
{
	struct BigInt x;
	bool tosub = 0;
	x.len = a.len;
	x.val = (char*)calloc(x.len,sizeof(char));
	for (int i = 0; i < a.len; i++) 
	{
		if (*(a.val + i) >=pick_digit(b, i) + tosub) 
		{
			*(x.val + i) = *(a.val + i) - pick_digit(b, i) - tosub;
			tosub = 0;
		}
		else 
		{
			*(x.val + i) = 100 + *(a.val + i) - pick_digit(b, i) - tosub;
			tosub = 1;
		}
	}
	// trim leading 0s
	for (int i = x.len - 1; i >= 0 && *(x.val + i) == 0; i--) x.len--;
	return x;
}

struct BigInt multinum(struct BigInt a, struct BigInt b,int s)
{
	int n;
	if (a.len < b.len)
	{
		struct BigInt p = a;
		a= b;
		b= p;
	}
	if (a.len == 1)
	{
		struct BigInt x;
		int w = *a.val * *b.val;
		if (w < 100)
		{
			x.len = 1;
			x.val = (char*)calloc(1, sizeof(char));
			*x.val = w;
		}
		else
		{
			x.len = 2;
			x.val = (char*)calloc(2, sizeof(char));
			*x.val = w % 100;
			*(x.val + 1) = w / 100;
		}
		return x;
	}
	struct BigInt alow, ahigh, blow, bhigh;
	n = (a.len +1)/ 2;
	divide_big(a, n, &alow, &ahigh);
	divide_big(b, n, &blow, &bhigh);
	struct BigInt P1, P2, P3, t;
	P1 = multinum(ahigh, bhigh,0);
	P2 = multinum(alow, blow,0);
	P3 = multinum(addnum(alow, ahigh,0), addnum(blow, bhigh,0),0);
	t = addnum(addnum(P1, subnum(subnum(P3, P1), P2), n), P2, n);
	free(P1.val); free(P2.val); free(P3.val);
	// Don't free alow, ahigh because they are pointing to a, b's val
	return t;
}

/* dividing x to xlow (lowest `0` to higher i digit exclusive)[len=i]
   and xhigh (i th inclusive to the highest `len`)[len=x.len-i]
   But... is it okay if one of them is empty? or i larger than x.len? */
void divide_big(struct BigInt x, int i, struct BigInt* xlow, struct BigInt* xhigh)
{
	xlow->val = x.val;
	if (i >= x.len) 
	{
		xlow->len = x.len;
		xhigh->len = 1;
		xhigh->val = (char*)calloc(1,sizeof(char));
		*xhigh->val = 0;
	}
	else 
	{
		xlow->len = i;
		xhigh->len = x.len - i;
		xhigh->val = x.val + i;
	}
}

void xprintf(struct BigInt x)
{
	// 0 is needed only in the later
	printf("%d", *(x.val + x.len - 1));
	for (int i = x.len - 2; i >= 0; i--) printf("%02d", *(x.val + i));
	printf("\n");
}
