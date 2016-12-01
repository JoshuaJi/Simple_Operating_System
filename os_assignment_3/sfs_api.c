#include "sfs_api.h"
#include "disk_emu.h"

#define BLOCK_SIZE 1024
#define NUM_OF_BLOCK 4096
#define NUM_OF_INODE 100
#define NUM_OF_FILE NUM_OF_INODE
#define NUM_OF_INODE_BLOCK  6 //((sizeof(Inode)*NUM_OF_INODE)/BLOCK_SIZE) = 5.47
#define NUM_OF_ROOT_DIR_BLOCK 3 // sizeof(Root_Directory)*NUM_OF_FILE/BLOCK_SIZE = 2.24
#define NUM_OF_FREE_MAP_BLOCK 4 

char* disk_name = "sys_xu_ji";

typedef struct Super_Block{
	int magic_number;
	int block_size;
	int file_system_size;
	int inode_table_length;
	int root_directory;
}Super_Block;

typedef struct Inode{
	int index;
	int dir_pointer[12];
	int indir_pointer;
}Inode;

typedef struct Root_Directory{
	char *file_name;
	char *exttension;
	int inode_index;
	
}Root_Directory;



Super_Block *super_block;
Inode[NUM_OF_INODE] *inode_table;
Root_Directory[NUM_OF_FILE] *root_directory_table;
int *free_bit_map;
free_bit_map = malloc(sizeof(int)*NUM_OF_BLOCK); 

void mksfs(int fresh){
	super_block = calloc(1, sizeof(Super_Block));
	inode_table = calloc(1, sizeof(inode_table));
	root_directory_table = calloc(1, sizeof(root_directory_table));
	free_bit_map = malloc(sizeof(int)*NUM_OF_BLOCK); 

	if (fresh){
		super_block->magic_number = 0xACBD0005;
		super_block->block_size = BLOCK_SIZE;
		super_block->file_system_size = BLOCK_SIZE*NUM_OF_BLOCK;
		super_block->inode_table_length = NUM_OF_INODE;
		super_block->root_directory = 0;

		for (int i = 0; i < NUM_OF_BLOCK; i++){
			if (i < 10){
				free_bit_map[i] = 1;
			}else{
				free_bit_map[i] = 0;
			}
		}
		init_fresh_disk(disk_name, block_size, num_block);
		write_blocks(0, 1, super_block);
		write_blocks(1, NUM_OF_INODE_BLOCK, inode_table);
		write_blocks(1+NUM_OF_INODE_BLOCK, NUM_OF_ROOT_DIR_BLOCK, root_directory_table);
		write_blocks(1+NUM_OF_INODE_BLOCK+NUM_OF_ROOT_DIR_BLOCK, NUM_OF_BLOCK-(1+NUM_OF_INODE_BLOCK+NUM_OF_ROOT_DIR_BLOCK), free_bit_map);

	}else{
		init_disk(disk_name, block_size, num_block);
	}

}
int sfs_get_next_file_name(char *fname){
  return 0;
}
int sfs_get_file_size(char* path){
  return 0;
}
int sfs_fopen(char *name){
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
