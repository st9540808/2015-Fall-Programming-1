#include <stdio.h>
#include <stdlib.h>

struct node
{
	unsigned int ip;
	unsigned char length;
	struct node *next;
} default_prefix = {0, 0, NULL};


unsigned int Convert_IPaddress_32bits(unsigned char [4]);
int LocateElement_4096(struct node*[4096], unsigned int);
int LocateElement_256(struct node*[256], unsigned int);
struct node* InsertSort(struct node *head, unsigned int add, unsigned char len);
struct node* LPM_Search(struct node*, unsigned int);
struct node* DeleteNode(struct node*, unsigned int, unsigned char);
void Decode(unsigned int ip_32bits, unsigned char ip[4]);
void PrintResult(FILE *, struct node *);
void FreeList(struct node*);
void IPdecode(struct node *match, FILE *result);
void print_list(struct node *head, FILE *timing);

int count = 0;
int main()
{
	int i;
	unsigned char ip[4], length;  //length of prefix
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
	         *delete = fopen("IPv4_delete.txt", "r"),
	          *timing = fopen("timing.txt", "w");
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

	for (i = 0; i < 1 << 8; i++)
	{
		print_list(group8_15[i], timing);
		fprintf(timing, "==============================\n");
	}
	for (i = 0; i < 1 << 12; i++)
	{
		print_list(group16_24[i], timing);
		fprintf(timing, "==============================\n");
		print_list(group25_32[i], timing);
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
		//no prefix matched
		PrintResult(result, &default_prefix);
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
	fclose(timing);
	return 0;
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

int LocateElement_4096(struct node *group[4096], unsigned int ip_32bits)
{
	int element;
	for (element = 0; element < 4096; element++)
	{
		if (group[element] == NULL)
			break;
		if ((group[element]->ip >> 20) == (ip_32bits >> 20))
			break;
	}
	return element;
}

int LocateElement_256(struct node *group[256], unsigned int ip_32bits)
{
	int element;
	for (element = 0; element < 256; element++)
	{
		if (group[element] == NULL)
			break;
		if (group[element]->ip >> 24 == ip_32bits >> 24)
			break;
	}
	return element;
}

struct node* InsertSort(struct node *head, unsigned int add, unsigned char len)
{
	struct node *temp = head, *newnode = malloc(sizeof(struct node));
	newnode -> ip = add;
	newnode -> length = len;
	newnode -> next = NULL;
	if (head == NULL)
		return newnode;
	if (head -> length <= len)
	{
		newnode -> next = head;
		return newnode;
	}
	while (temp -> next != NULL)
	{
		if ((temp -> next) -> length <= len)
		{
			newnode -> next = temp -> next;
			temp -> next = newnode;
			return head;
		}
		else
			temp = temp -> next;
	}
	temp -> next = newnode;
	return head;
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
		struct node *temp = head;

		while (temp->next != NULL && length < (temp->next)->length)
			temp = temp->next;

		//case 4 : node inserted at the end of the linked list
		if (temp->next == NULL)
		{
			temp->next = new_node;
			return head;
		}

		new_node->next = temp->next;
		temp->next = new_node;
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
/*
struct node* DeleteNode(struct node *head, unsigned int ip_32bits, unsigned char length)
{
	struct node *cur  = head,
				*prev = NULL;

	while(cur != NULL)
	{
		if(cur->ip == ip_32bits && cur->length == length)
			break;
		prev = cur;
		cur = cur->next;
	}

	if(cur == NULL)
	{
		unsigned char ip[4];
		Decode(ip_32bits, ip);
		printf("%d : %u.%u.%u.%u/%u not deleted\n", ++count, ip[0], ip[1], ip[2], ip[3], length);
		return head;
	}
	if(prev == NULL)
		head = head->next;
	else
		prev->next = cur->next;

	free(cur);
	return head;
}
*/

struct node* DeleteNode(struct node *head, unsigned int ip_32bits, unsigned char length)
{
	struct node *temp = head, *to_free;
	if (head == NULL)
	{
		unsigned char ip[4];
		Decode(ip_32bits, ip);
		printf("%d : %u.%u.%u.%u/%u not deleted\n", ++count, ip[0], ip[1], ip[2], ip[3], length);
		return head;
	}
	if (head -> ip == ip_32bits && head -> length == length)
	{
		to_free = head;
		head = head -> next;
		free(to_free);
		return head;
	}
	while (temp -> next != NULL)
	{
		if ((temp -> next) -> ip == ip_32bits && (temp -> next) -> length == length)
		{
			to_free = temp -> next;
			temp -> next = (temp -> next) -> next;
			free(to_free);
			return head;
		}
		else
			temp = temp -> next;
	}

	unsigned char ip[4];
	Decode(ip_32bits, ip);
	printf("%d : %u.%u.%u.%u/%u not deleted\n", ++count, ip[0], ip[1], ip[2], ip[3], length);
	return head;
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

void IPdecode(struct node *match, FILE *result)
{
	if (match != NULL)
	{
		unsigned int bin = match -> ip;
		unsigned char ip[4], length;
		ip[0] = bin >> 24;
		bin &= 0x00FFFFFF;
		ip[1] = bin >> 16;
		bin &= 0x0000FFFF;
		ip[2] = bin >> 8;
		bin &= 0x000000FF;
		ip[3] = bin;
		length = match -> length;
		fprintf(result, "%u.%u.%u.%u/%u\n", ip[0], ip[1], ip[2], ip[3], length);
	}
	else   //match == NULL means there are no matched prefix, use default
		fprintf(result, "%s", "0.0.0.0/0\n");
	return;
}

void print_list(struct node *head, FILE *timing)
{
	struct node *temp = head;
	while (temp != NULL)
	{
		IPdecode(temp, timing);
		temp = temp -> next;
	}
	return;
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
