//學號：F74042086
//姓名：郭泰佑
//編譯方式：gcc -std=c99 -o hw8 hw8.c
//執行方式：./hw8
//程式功能：create linked list data structure
//更新日期：2016.1.15
#include <stdio.h>
#include <stdlib.h>

struct node
{
	unsigned int ip;
	unsigned char length;
	struct node *next;
} default_prefix = {0, 0, NULL};


struct node* InsertSort(struct node*, unsigned int, unsigned char);//insert node as well as sort
struct node* LPM_Search(struct node*, unsigned int);//LPM search, if no match, return default prefix
struct node* DeleteNode(struct node*, unsigned int, unsigned char);//delete node in linked list

unsigned int Convert_IPaddress_32bits(unsigned char [4]);//compress ip array into an unsigned int
void Decode(unsigned int, unsigned char [4]);//convert ip address(unsigned int) itto array
void PrintResult(FILE *, struct node *); //print the matched prefix in result.txt
void FreeList(struct node*);//free all nodes in a linked list

//int count = 0;//uesd to checked how many prefix hasn't been deleted
int main()
{
	int i;
	unsigned char ip[4], length;  //ip array & length of prefix
	unsigned int ip_32bits;		  //ip_32bits is the compress result of ip array
	struct node *group25_32[4096],//sorted with first 12 significnat bits
		       *group16_24[4096],//sorted with first 12 significant bits
		       *group8_15[256];  //sorted with first 8  significant bits

	for (i = 0; i < 4096; i++)
	{
		group25_32[i] = NULL;
		group16_24[i] = NULL;

		if (i < 256)
			group8_15[i] = NULL;
	}


	FILE *prefix = fopen("IPv4_400k.txt", "r"),
	      *search = fopen("IPv4_search.txt", "r"),
	       *result = fopen("result.txt", "w"),
	        *insert = fopen("IPv4_insert.txt", "r"),
	         *delete = fopen("IPv4_delete.txt", "r");
	if (prefix == NULL) {printf("IPv4_400k.txt can't open\n");	 exit(1);}
	if (search == NULL) {printf("IPv4_search.txt can't open\n"); exit(1);}
	if (result == NULL) {printf("result.txt can't open\n"); 	 exit(1);}
	if (insert == NULL) {printf("IPv4_insert.txt can't open\n"); exit(1);}
	if (delete == NULL) {printf("IPv4_delete.txt can't open\n"); exit(1);}

//create data structure
	while (fscanf(prefix, "%hhu.%hhu.%hhu.%hhu/%hhu",
	              &ip[0], &ip[1], &ip[2], &ip[3], &length) != EOF)
	{
		int element;
		ip_32bits = Convert_IPaddress_32bits(ip);

		if (length >= 8 && length < 16) //store into group8_15
		{
			element = ip_32bits >> 24;
			group8_15[element] = InsertSort(group8_15[element], ip_32bits, length);
		}
		else if (length >= 16 && length < 25) //store into group16_24
		{
			element = ip_32bits >> 20;
			group16_24[element] = InsertSort(group16_24[element], ip_32bits, length);
		}
		else if (length >= 25 && length <= 32) //store into group25_32
		{
			element = ip_32bits >> 20;
			group25_32[element] = InsertSort(group25_32[element], ip_32bits, length);
		}
	}



//LPM Search
	while (fscanf(search, "%hhu.%hhu.%hhu.%hhu",
	              &ip[0], &ip[1], &ip[2], &ip[3]) != EOF)
	{
		int element;
		struct node *ip_search;//the possible match for ip in search.txt(it can be default prefix)

		ip_32bits = Convert_IPaddress_32bits(ip);

		//search from group25_32
		element = ip_32bits >> 20;
		if (group25_32[element] != NULL) //means first 12 significant bits match ip_32bits
		{
			ip_search = LPM_Search(group25_32[element], ip_32bits);
			if (ip_search->length != 0) //means it is not default prefix
			{
				PrintResult(result, ip_search);
				continue;
			}
		}

		//search from group16_24
		element = ip_32bits >> 20;
		if (group16_24[element] != NULL)
		{
			ip_search = LPM_Search(group16_24[element], ip_32bits);
			if (ip_search->length != 0)
			{
				PrintResult(result, ip_search);
				continue;
			}
		}

		//search from group8_15
		element = ip_32bits >> 24;
		if (group8_15[element] != NULL)
		{
			ip_search = LPM_Search(group8_15[element], ip_32bits);
			if (ip_search->length != 0)
			{
				PrintResult(result, ip_search);
				continue;
			}
		}

		PrintResult(result, &default_prefix);//no prefix matched
	}//LPM ends here


//Insert
	while (fscanf(insert, "%hhu.%hhu.%hhu.%hhu/%hhu",
	              &ip[0], &ip[1], &ip[2], &ip[3], &length) != EOF)
	{
		int element;
		ip_32bits = Convert_IPaddress_32bits(ip);

		if (length >= 8 && length < 16) //insert into group8_15
		{
			element = ip_32bits >> 24;
			group8_15[element] = InsertSort(group8_15[element], ip_32bits, length);
		}
		else if (length >= 16 && length < 25) //insert into group16_24
		{
			element = ip_32bits >> 20;
			group16_24[element] = InsertSort(group16_24[element], ip_32bits, length);
		}
		else if (length >= 25 && length <= 32) //insert into group25_32
		{
			element = ip_32bits >> 20;
			group25_32[element] = InsertSort(group25_32[element], ip_32bits, length);
		}
	}


//Delete Node
	while (fscanf(delete, "%hhu.%hhu.%hhu.%hhu/%hhu",
	              &ip[0], &ip[1], &ip[2], &ip[3], &length) != EOF)
	{
		int element;
		ip_32bits = Convert_IPaddress_32bits(ip);

		if (length >= 8 && length < 16)
		{
			element = ip_32bits >> 24;
			group8_15[element] = DeleteNode(group8_15[element], ip_32bits, length);
		}
		else if (length >= 16 && length < 25)
		{
			element = ip_32bits >> 20;
			group16_24[element] = DeleteNode(group16_24[element], ip_32bits, length);
		}
		else if (length >= 25 && length <= 32)
		{
			element = ip_32bits >> 20;
			group25_32[element] = DeleteNode(group25_32[element], ip_32bits, length);
		}
	}


	for (i = 0; i < 4096; i++)
	{
		FreeList(group25_32[i]);
		FreeList(group16_24[i]);

		if (i < 256)
			FreeList(group8_15[i]);
	}
	fclose(prefix);
	fclose(search);
	fclose(result);
	fclose(insert);
	fclose(delete);
	return 0;
}




struct node* InsertSort(struct node *head, unsigned int ip_32bits, unsigned char length)
{
	struct node *new_node;
	new_node = malloc(sizeof(struct node));
	if (new_node == NULL) {printf("malloc failure\n"); exit(1);}

	new_node->ip = ip_32bits;
	new_node->length = length;
	new_node->next = NULL;

	//case 1 : insert directly at the beginning (the list is empty)
	if (head == NULL)
		return new_node;

	//case 2 : length is greater than any other that stored in linked list
	else if (length >= head->length)
	{
		new_node->next = head;
		return new_node;
	}

	//case 3 : node need to be inserted somewhere in the linked list
	else
	{
		struct node *cur  = head,
			             *prev = NULL;

		while (cur != NULL && length < cur->length)
		{
			prev = cur;
			cur = cur->next;
		}
		//case 4 : node inserted at the end of the linked list
		if (cur == NULL)
		{
			prev->next = new_node;
			return head;
		}

		new_node->next = prev->next;
		prev->next = new_node;
		return head;
	}
}

struct node* LPM_Search(struct node *head, unsigned int ip_32bits)
{
	while (head != NULL)
	{
		if (head->ip >> (32 - head->length) == ip_32bits >> (32 - head->length))
			return head;
		else
			head = head->next;
	}
	return &default_prefix;
}

struct node* DeleteNode(struct node *head, unsigned int ip_32bits, unsigned char length)
{
	struct node *cur  = head,
		             *prev = NULL;

	while (cur != NULL)
	{
		if (cur->ip == ip_32bits && cur->length == length)
			break;
		prev = cur;
		cur = cur->next;
	}

	if (cur == NULL)
	{
		/*		to test how many prefix has not been deleted*/
//		unsigned char ip[4];
//		Decode(ip_32bits, ip);
//		printf("%d : %u.%u.%u.%u/%u not deleted\n", ++count, ip[0], ip[1], ip[2], ip[3], length);
		return head;
	}
	if (prev == NULL)
		head = head->next;
	else
		prev->next = cur->next;

	free(cur);
	return head;
}



unsigned int Convert_IPaddress_32bits(unsigned char ip[4])
{
	unsigned int ip_32bits = 0;
	for (int i = 0; i < 4; i++)
	{
		ip_32bits <<= 8;
		ip_32bits |= ip[i];
	}
	return ip_32bits;
}

void Decode(unsigned int ip_32bits, unsigned char ip[4])
{
	unsigned int ip_segment;
	for (int i = 0; i < 4; i++)
	{
		ip_segment = ip_32bits & 255 << (24 - i * 8);
		ip_segment = ip_segment >> (24 - i * 8);
		ip[i] = (unsigned char)ip_segment;
	}
}

void PrintResult(FILE *result, struct node *ip_search)
{
	unsigned char ip[4];
	Decode(ip_search->ip, ip);
	fprintf(result, "%u.%u.%u.%u/%u\n",	ip[0], ip[1], ip[2], ip[3], ip_search->length);
}



void FreeList(struct node* head)
{
	struct node *temp;

	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp);
	}
}

