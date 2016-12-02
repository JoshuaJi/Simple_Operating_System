#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sfs_api.h"
#include "disk_emu.h"

#define BLOCK_SIZE 1024
#define NUM_OF_BLOCK 2048
#define NUM_OF_INODE 100
#define NUM_OF_FILE NUM_OF_INODE
#define NUM_OF_INODE_BLOCK  6 //((sizeof(Inode)*NUM_OF_INODE)/BLOCK_SIZE) = 5.47
#define NUM_OF_ROOT_DIR_BLOCK 3 // sizeof(Root_Directory)*NUM_OF_FILE/BLOCK_SIZE = 2.24
#define NUM_OF_FREE_MAP_BLOCK 4 
#define FILE_MAX_SIZE 274432 // ((1024/4)+12)*1024
#define DATA_BLOCK_START_INDEX 14
#define NUM_OF_DATA_BLOCK 2034

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
	int occupied;
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

typedef struct File_Descriptor{
	int occupied;
	int id;
	int write_pos;
}File_Descriptor;

Super_Block *super_block;
Inode *inode_table;
Root_Directory *root_directory_table;
Free_Bit_Map *free_bit_map;
File_Descriptor *file_descriptor_table; 

void save_status_on_disk(){
	write_blocks(0, 1, super_block);
	write_blocks(1, NUM_OF_INODE_BLOCK, inode_table);
	write_blocks(1+NUM_OF_INODE_BLOCK, NUM_OF_ROOT_DIR_BLOCK, root_directory_table);
	write_blocks(1+NUM_OF_INODE_BLOCK+NUM_OF_ROOT_DIR_BLOCK, NUM_OF_FREE_MAP_BLOCK, free_bit_map);
}

void load_status_from_disk(){
	read_blocks(0, 1, super_block);
	read_blocks(1, NUM_OF_INODE_BLOCK, inode_table);
	read_blocks(1+NUM_OF_INODE_BLOCK, NUM_OF_ROOT_DIR_BLOCK, root_directory_table);
	read_blocks(1+NUM_OF_INODE_BLOCK+NUM_OF_ROOT_DIR_BLOCK, NUM_OF_FREE_MAP_BLOCK, free_bit_map);

}

void mksfs(int fresh){
	super_block = calloc(1, sizeof(Super_Block));
	inode_table = calloc(NUM_OF_INODE, sizeof(Inode));
	root_directory_table = calloc(NUM_OF_FILE, sizeof(Root_Directory));
	free_bit_map = calloc(NUM_OF_BLOCK, sizeof(int)); 
	file_descriptor_table = calloc(NUM_OF_FILE, sizeof(File_Descriptor));
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
		init_fresh_disk(disk_name, BLOCK_SIZE, NUM_OF_BLOCK);

		save_status_on_disk();
		
	}else{
		init_disk(disk_name, BLOCK_SIZE, NUM_OF_BLOCK);
		load_status_from_disk();
	}
}

int sfs_get_next_file_name(char *fname){
	if ((root_directory_table+next_file_index)->file_name){
		fname = strdup((root_directory_table+next_file_index)->file_name);
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
		if ((root_directory_table+i)->file_name){
			current_fname = strdup((root_directory_table+i)->file_name);
			strcat(current_fname, ".");
			strcat(current_fname, (root_directory_table+i)->extension);
		} 
		if (current_fname != NULL){
			if (strcmp(current_fname, fname) == 0){
				int inode_i = (root_directory_table+i)->inode_index;
				return inode_i;
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
	printf("sfs_get_file_size: no such file name\n");
	return -1;
}

int split_name(char *fname, char *file_name, char *extension){
	int index = -1;
	
	for (int i = 0; i < strlen(fname); i++){
		if (fname[i] == '.' && index == -1){
			index = i;
		}else if(index == -1 && i <16){
			file_name[i] = fname[i];
		}else if (index != -1 && (i-index-1)<3){
			extension[i-index-1] = fname[i];
		}else{
			printf("split_name: file name is not valid\n");
			return -1;
		}
	}
	return index;
}

int add_file_to_root_dir(char *file_name, char *extension, int inode_i){
	for (int i = 0; i < NUM_OF_FILE; i++){
		if (root_directory_table[i].file_name == NULL){
			root_directory_table[i].file_name = file_name;
			root_directory_table[i].extension = extension;
			root_directory_table[i].inode_index = inode_i;
			inode_table[inode_i].occupied = 1;
			return 0;
		}
	}
	printf("int add_file_to_root_dir: file capicity reached maximum\n");
	return -1;
}

int get_avaliable_inode_index(){
	for (int i = 0; i < NUM_OF_INODE; i++){
		if (inode_table[i].occupied == 0){
			return i;
		}
	}
	printf("get_avaliable_inode_index: no inode avaliable currently\n");
	return -1;
}

int add_file_to_file_descripor(int inode_i, int size){
	for (int i = 0; i < NUM_OF_FILE; i++){
		if (file_descriptor_table[i].occupied == 0){
			file_descriptor_table[i].id = inode_i;
			file_descriptor_table[i].write_pos = size;
			file_descriptor_table[i].occupied = 1;
			return 0;
		}
	}
	printf("no avaliable space in file descriptor table\n");
	return -1;
}

int sfs_fopen(char *name){

	if (strlen(name) > 20){
		printf("sfs_fopen: file name exceed length limit\n");
		return -1;
	}

	int inode_i = search_file_in_root(name);

	if (inode_i == -1){
		char *file_name = malloc(16);
		char *extension = malloc(3);

		inode_i = get_avaliable_inode_index();

		if (inode_i == -1){
			return -1;
		}
		if (split_name(name, file_name, extension) == -1){
			return -1;
		}else{
			if(add_file_to_root_dir(file_name, extension, inode_i) == -1)
				return -1;
			else{
				inode_table[inode_i].index = inode_i;
				inode_table[inode_i].size = 0;
				if (add_file_to_file_descripor(inode_i, 0) == -1)
					return -1;
			}
		}
	}else{
		for (int i = 0; i < NUM_OF_FILE; i++){
			if (file_descriptor_table[i].occupied){
				if (file_descriptor_table[i].id == inode_i){
					return inode_i;
				}
			}
		}
		add_file_to_file_descripor(inode_i, inode_table[inode_i].size);
	}
	save_status_on_disk();
	return inode_i;
}
int sfs_fclose(int fileID){
	for (int i = 0; i < NUM_OF_FILE; i++){
		if (file_descriptor_table[i].id == fileID && file_descriptor_table[i].occupied == 1){
			file_descriptor_table[i].occupied = 0;
			file_descriptor_table[i].write_pos = 0;
			file_descriptor_table[i].id = 0;
			return 0;
		}
	}
	return -1;
}
int sfs_frseek(int fileID, int loc){
	return 0;
}
int sfs_fwseek(int fileID, int loc){
	return 0;
}

int retrive_last_block_data(char *last_block, int fileID){
	int last_block_index = -1;
	int i = 0;
	
	for (i = 0; i < 12; i++){
		if (inode_table[fileID].dir_pointer[i] == 0){
			break;
		}else{
			last_block_index = inode_table[fileID].dir_pointer[i];
		}
	}

	if (last_block_index == -1){
		strcpy(last_block, "");
		return 0;
	}
	// direct pointer
	else if(i != 12){
		read_blocks(last_block_index, 1, last_block);
		return last_block_index;
	}
	// indirect pointer
	else{
		int indirect_block_index = inode_table[fileID].indir_pointer;
		int indir_block_pointers[256];
		read_blocks(indirect_block_index, 1, indir_block_pointers);
		for (int i = 0; i < 256; i++){
			if (indir_block_pointers[i] != 0){
				last_block_index = indir_block_pointers[i];
			}else{
				break;
			}
		}

		read_blocks(last_block_index, 1, last_block);
		return last_block_index;
	}
}

int find_empty_data_block(){
	for (int i = DATA_BLOCK_START_INDEX; i < NUM_OF_DATA_BLOCK; i++){
		if (free_bit_map->free_block[i] == 0){
			return i;
		}
	}
	return -1;
}

int sfs_fwrite(int fileID, char *buf, int length){
	int file_descriptor_index = -1;
	for (int i = 0; i < NUM_OF_FILE; i++){
		if (file_descriptor_table[i].occupied == 1 && file_descriptor_table[i].id == fileID){
			file_descriptor_index = i;
			break;
		}
	}

	if (file_descriptor_index == -1){
		printf("File is not currently opened\n");
		return -1;
	}

	if (inode_table[file_descriptor_index].size + length > FILE_MAX_SIZE){
		printf("Exceed file maximum size 274432 Bytes\n");
		return -1;
	}

	char *last_block = malloc(BLOCK_SIZE*sizeof(char)); 
	int last_block_index = retrive_last_block_data(last_block, fileID);
	int buf_position = 0;
	while(buf_position < strlen(buf)){

		if (strlen(last_block) == BLOCK_SIZE){
			write_blocks(last_block_index, 1, last_block);
			strcpy(last_block, "");
			last_block_index = find_empty_data_block();
			if (last_block_index == -1){
				printf("no empty data block left\n");
				return -1;
			}
		}

		*(last_block+strlen(last_block)) = *(buf+buf_position);
		free_bit_map->free_block[last_block_index] = 1;
		buf_position++;
	}
	if (strlen(last_block)!=0){
		write_blocks(last_block_index, 1, last_block);
	}


	file_descriptor_table[fileID].write_pos += length;
	inode_table[fileID].size = file_descriptor_table[fileID].write_pos;
	return length;
}
int sfs_fread(int fileID, char *buf, int length){
	return 0;
}
int sfs_remove(char *file){
	return 0;
}
