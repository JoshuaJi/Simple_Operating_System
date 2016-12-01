#include <stdlib.h>
#include <stdio.h>
#include "sfs_api.h"
#include "disk_emu.h"

#define BLOCK_SIZE 1024
#define NUM_OF_BLOCK 2048
#define NUM_OF_INODE 100
#define NUM_OF_FILE NUM_OF_INODE
#define NUM_OF_INODE_BLOCK  6 //((sizeof(Inode)*NUM_OF_INODE)/BLOCK_SIZE) = 5.47
#define NUM_OF_ROOT_DIR_BLOCK 3 // sizeof(Root_Directory)*NUM_OF_FILE/BLOCK_SIZE = 2.24
#define NUM_OF_FREE_MAP_BLOCK 4 

char* disk_name = "sys_xu_ji";
int next_file_index;

typedef struct Super_Block{
	int magic_number;
	int block_size;
	int file_system_size;
	int inode_table_length;
	int root_directory;
}Super_Block;

typedef struct Inode{
	int index;
	int size;
	int dir_pointer[12];
	int indir_pointer;
}Inode;

typedef struct Root_Directory{
	char *file_name;
	char *extension;
	int inode_index;
	
}Root_Directory;

typedef struct Free_Bit_Map{
	int free_block[NUM_OF_BLOCK];
}Free_Bit_Map;

Super_Block *super_block;
Inode *inode_table;
Root_Directory *root_directory_table;
Free_Bit_Map *free_bit_map;

void mksfs(int fresh){
	super_block = calloc(1, sizeof(Super_Block));
	inode_table = calloc(NUM_OF_INODE, sizeof(Inode));
	root_directory_table = calloc(NUM_OF_FILE, sizeof(Root_Directory));
	free_bit_map = calloc(NUM_OF_BLOCK, sizeof(int)); 
	next_file_index = 0;
	if (fresh){
		super_block->magic_number = 0xACBD0005;
		super_block->block_size = BLOCK_SIZE;
		super_block->file_system_size = BLOCK_SIZE*NUM_OF_BLOCK;
		super_block->inode_table_length = NUM_OF_INODE;
		super_block->root_directory = 0;
		for (int i = 0; i < NUM_OF_BLOCK; i++){
			if (i < 14){
				free_bit_map->free_block[i] = 1;
			}else{
				free_bit_map->free_block[i] = 0;
			}
		}
		printf("a\n");
		init_fresh_disk(disk_name, BLOCK_SIZE, NUM_OF_BLOCK);
		printf("b\n");
		write_blocks(0, 1, super_block);
		write_blocks(1, NUM_OF_INODE_BLOCK, inode_table);
		write_blocks(1+NUM_OF_INODE_BLOCK, NUM_OF_ROOT_DIR_BLOCK, root_directory_table);
		write_blocks(1+NUM_OF_INODE_BLOCK+NUM_OF_ROOT_DIR_BLOCK, NUM_OF_FREE_MAP_BLOCK, free_bit_map);
		printf("c\n");
	}else{
		init_disk(disk_name, BLOCK_SIZE, NUM_OF_BLOCK);
		read_blocks(0, 1, super_block);
		read_blocks(1, NUM_OF_INODE_BLOCK, inode_table);
		read_blocks(1+NUM_OF_INODE_BLOCK, NUM_OF_ROOT_DIR_BLOCK, root_directory_table);
		read_blocks(1+NUM_OF_INODE_BLOCK+NUM_OF_ROOT_DIR_BLOCK, NUM_OF_FREE_MAP_BLOCK, free_bit_map);

	}
}

int sfs_get_next_file_name(char *fname){
	if ((root_directory_table+next_file_index)->file_name){
		strdup(fname, (root_directory_table+next_file_index)->file_name);
		strcat(fname, ".");
		strcat(fname, (root_directory_table+next_file_index)->extension);
		next_file_index = next_file_index+1;
		return (root_directory_table+next_file_index)->inode_index;
	}
	return 0;
}

int search_file_in_root(char *fname){
	for (int i = 0; i < NUM_OF_FILE; i++){
		char *current_fname = NULL;
		if ((root_directory+i)){
			strdup(current_fname, (root_directory+i)->file_name);
			strcat(current_fname, ".");
			strcat(current_fname, (root_directory+i)->extension)
		} 
		if (current_fname != NULL){
			if (strcmp(current_fname, fname)){
				int inode_i = (root_directory+i)->inode_index;
				return inode_i
			}
		}
	}
	return -1;
}

int sfs_get_file_size(char* path){
	int inode_i = search_file_in_root(path);
	if (inode_i != -1){
		return (inode_table+inode_i)->size;
	}
	return -1;
}

int sfs_fopen(char *name){
	int inode_i = search_file_in_root(name);
	if (inode_i == -1){
		/*




		
		from here





		*/
	}
	return 0;
}
int sfs_fclose(int fileID){
	return 0;
}
int sfs_frseek(int fileID, int loc){
	return 0;
}
int sfs_fwseek(int fileID, int loc){
	return 0;
}
int sfs_fwrite(int fileID, char *buf, int length){
	return 0;
}
int sfs_fread(int fileID, char *buf, int length){
	return 0;
}
int sfs_remove(char *file){
	return 0;
}
