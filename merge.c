node* mergeSort(node*head,int length){
	if(head == NULL){
		//do we throw exception?
		return;
	}
	//nothing to sort so just returns
	if(length == 1){
		return head;
	}
	
	//splits list into 2
	int count = length/2;
	int i = 0;
	node* ptr = head;
	node* prev;
	while(i < count){
		prev = ptr;
		ptr = ptr->next;
		i++;
	}
	prev->next = NULL;
	
	//sorts both halves of function
	node* llFirst =  mergeSort(ptr, length-count); 
	node* llSecond = mergeSort(head,count);
	
	node* temp = NULL;
	node* end = NULL;
	
	while(llFirst!=NULL && llSecond!=NULL){
		//will sort in ascending order. Equal values is resolved by putting whichever value was read in first, first in the list.
		if (llFirst->val > llSecond->val){
			if(temp == NULL){
				temp = llSecond;
				end = temp;
			}
			else{
				end->next = llSecond;
				end = end->next;
			}
			llSecond = llSecond->next;
			//end.next = NULL; not necessary. 
		}
		else{
			if(temp == NULL){
				temp = llFirst;
				end = temp;
			}
			else{
				end->next = llFirst;
				end = end->next;
			}
			llFirst = llFirst->next;
			//end.next = NULL; not necessary.
		}
		
	}
	//puts in the remaining values
	while(llFirst != NULL){
			end->next = llFirst;
			end = end->next;
			llFirst = llFirst->next;
	}
	while(llSecond != NULL){
			end->next = llSecond;
			end = end->next;
			llSecond = llSecond->next;
	}
//	pthread_mutex_lock(&tree_mutex);
//		finalTreeInt(temp,function);
//	pthread_mutex_unlock(&tree_mutex);
	//printf("gets to end of mergesort int \n");
	return temp;
}
