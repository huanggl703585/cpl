#ifndef __TREE_H
#define __TREE_H

#define post_order_travel(root,recv,size,left,right) ({	\
      int respt=0;					\
      do{						\
	typeof(root) stack[size];			\
	int runstate[size];				\
	int bt[size];					\
	       stack[0]=root;				\
	       bt[0]=-1;				\
	       runstate[0]=0;				\
	       int pt=0;				\
	       while(pt!=-1){				\
		 int origin=pt;				\
		 typeof(root) curnode=stack[pt];			\
		 if(runstate[pt]==0){					\
		 runstate[pt]=1;					\
		 if(curnode->left!=NULL){				\
		     stack[++pt]=curnode->left;				\
		     bt[pt]=origin;					\
		     runstate[pt]=0;					\
		   }							\
		   }							\
		 else if(runstate[pt]==1){				\
		     runstate[pt]=2;					\
		     if(curnode->right!=NULL){				\
		       stack[++pt]=curnode->right;			\
		       bt[pt]=origin;					\
		       runstate[pt]=0;					\
		       }						\
		       }						\
		   else{						\
		       recv[respt++]=curnode;				\
		       pt=bt[pt];					\
		       }						\
		       }						\
		       }while(0);					\
		       respt;})
 
#endif
