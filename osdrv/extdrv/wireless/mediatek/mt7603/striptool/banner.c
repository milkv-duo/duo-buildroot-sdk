/*
	Shiang: 
	This file just used for add some banner in front of the specific files,
	for other usage you may need to check if that's suitable for it.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

enum PATH_TYPE{
	TYPE_INVALID = 0,
	TYPE_FILE = 1,
	TYPE_DIR = 2,
	TYPE_OTHER = 3
};


void usage()
{
	printf("banner -i copyright_template_file -t target_source_file");
	printf("\t-b: banner templete file\n");
	printf("\t-s: source file/directory need to handle\n");
	printf("\t-d: destination file/directory\n");
	printf("\t-e: exclude all files with extension\n");
	printf("\t-i: include all files with extension\n");
	printf("\t-R: apply to all sub-directories\n");
}


void dump_banner(char *buf, int len)
{
	int offset; 
	char *ptr = buf;

	if (buf && (len > 0)){
		printf("\nDumpBanner: size=%d\n", len);
		for(offset = 0; offset < len; offset++){
			printf("%c", *ptr);
			ptr++;
		}
		printf("\n");
	}
	else
		printf("Invalid parameters(Buf=0x%p, len=%d)!\n", buf, len);
}


char *concat_path_file_str(char *fpath, char *fname)
{
	int len;
	char *full_path;

	if ((!fpath) || (!fname))
		return NULL;
	
	len = strlen(fpath) + strlen(fname) + 2;
	if ((full_path = malloc(len)) != NULL) {
		memset(full_path, 0, len);
		snprintf(full_path, len, "%s/%s\n", fpath, fname);
	}

	return full_path;
}


char *get_file_path(char *filepath)
{
	int left_len;
	char *abs_path, *fname = filepath;

	abs_path = malloc(4096);	
	if (abs_path == NULL) {
		printf("allocate memory for absolute path failed!\n");
		goto fail;
	}

	if (getcwd(abs_path, 4096) == NULL) {
		printf("get current work directory failed!\n");
		goto fail;
	}

	left_len = 4095 - strlen(abs_path);
	if (strncmp(filepath, "/", strlen("/")) == 0)
		strncpy(abs_path, fname, 4095);
	else {
		if (left_len > 1)
			strncat(abs_path, "/", strlen("/"));
		left_len = 4095 - strlen(abs_path);
		if (strncmp(filepath, "./", strlen("./")) == 0)
			fname += 2; /* remove the . and keep the slash */
		strncat(abs_path, fname, left_len);
	}
	
	//printf("After concat, abs_path=%s\n", abs_path);
	return abs_path;
	
fail:
	if (abs_path)
		free(abs_path);
	return NULL;
	
}


/*
	success: return the file size
	fail: return -1
*/
int is_valid_file(char *path)
{
	struct stat buf;

	if (stat(path, &buf) == 0) {
		if (S_ISREG(buf.st_mode))
			return buf.st_size;
	}
	else
		return -1;
}


/* 
	Currently, following file names will be ignored
	1. ".", "..", and all hidden files
	2. all files without extension
	3. all files with file extension not equal to ".c" or ".h"
	4. these files directly copy to destination if assigned.
*/
int is_target_file(char *full_path)
{
	char *fname, *ext_ptr;
	int isit;

	if ((fname = strrchr(full_path, '/')) != NULL)
		fname++;
	else
		fname = full_path;
	//printf("\tHandle File:%s!\n", fname);
	
	ext_ptr = strrchr(fname, '.');
	if ((strncmp(fname, ".", 1) == 0) || (ext_ptr == NULL))
		return 0;
	
	if((strcmp(ext_ptr, ".h") == 0) || (strcmp(ext_ptr, ".c") == 0))
		return 1;
	else
		return 0;
}


enum PATH_TYPE check_path_type(char *path)
{
	struct stat buf;
	int src_dir, dst_dir;

	if (stat(path, &buf) == 0) {
		if (S_ISREG(buf.st_mode))
			return TYPE_FILE;
		else if (S_ISDIR(buf.st_mode))
			return TYPE_DIR;
		else
			return TYPE_OTHER;
	}
	
	return TYPE_INVALID;
}


/*
	Remove original banner 
		Rule 1. if we found the first pair of / and *, we think that's banner.
		Rule 2. if we didn't found the banner yet and meet the "#include". 
				"#define", #undef, or #ifndef, we think it as no banner
		Rule 3. if we found the "//" berfore the /* and * /, we think that's 
*/
int remove_original_banner(FILE *fp)
{
	int c, offset = 0, banner_end = 0, token_add, syntax_err = 0;
	int token_idx = 0;
	char token[4] = {0};

	while((c = fgetc(fp))!= EOF) {
		token_add = 0;
		switch (token_idx) {
			case 0:
				if (c == '/')
					token_add = 1;
				else if (!((c == 0x20) || (c == '\n') || (c == '\r')))
					banner_end = 1;
				break;
			case 1:
				if (c == '/')
					banner_end = 1;
				else if (c == '*')
					token_add = 1;
				else
					syntax_err = 1;
				break;
			case 2:
				if (c == '*')
					token_add = 1;
				break;
			case 3:
				if (c == '/')
					token_add = 1;
				else {
					token_idx--;
					token[token_idx] = 0;
					if (c == '*')
						token_add = 1;
				}
				break;
			default:
				printf("Invalid token_idx(%d), now token(len=%d)=%s\n", 
						token_idx, strlen(&token[0]), &token[0]);
				banner_end =1;
				break;
		}

		if (syntax_err) {
			printf("Syntax error for file, offset=%d, read char=%c," 
					"token_idx(%d), tokenString(len:%d)=%s\n", 
					offset, c, token_idx, strlen(&token[0]), &token[0]);
			banner_end = 1;
		}

		if (banner_end) {
			if (token_idx > 0)
				offset -= token_idx;
			break;
		}
		
		if (token_add) {
			token[token_idx] = c;
			token_idx++;
			if (token_idx == 4) {
				memset(&token[0], 0, sizeof(token));
				token_idx = 0;
			}
		}

		offset++;
	}

	return (banner_end ? offset : -1);
	
}


int copy_to_file(char *srcf, char *tgtf, char *buf, int buflen, int replaceit)
{
	FILE *fsrc = NULL, *fdst = NULL;
	char *tmpfile = tgtf, *tmpbuf;
	char *file_buf;
	int size, copy_len, finished = 0;
	
	if ((fsrc= fopen(srcf, "r")) == NULL)
		return -1;
	
	if (replaceit) {
		size = strlen(srcf) + 5;
		tmpfile = malloc(size);
		if (tmpfile == NULL) {
			printf("alloc memory(size=%d) for temp file failed, source:%s!\n", size, srcf);
			goto done;
		}
		memset(tmpfile, 0, size);
		strncpy(tmpfile, srcf, strlen(srcf));
		strncat(tmpfile, ".cpy", strlen(".cpy"));
	}

	if ((fdst = fopen(tmpfile, "w+")) == NULL) {
		printf("Create target file failed:%s\n", tmpfile);
		goto done;
	}

	size = 0;
	/* Add banner to the new file first */
	if (buf && (buflen > 0)) {
		copy_len = fwrite(buf, 1, buflen, fdst);
		fputc('\n', fdst);
		fputc('\n', fdst);

		/* append the original file to the new one */
		size = remove_original_banner(fsrc);
		//printf("remove banner size=%d\n", size);
	}
	fseek(fsrc, size, SEEK_SET);

	tmpbuf = malloc(1024);
	if (tmpbuf == NULL)
		goto done;
	
	do {
		memset(tmpbuf, 0, 1024);
		copy_len = fread(tmpbuf, 1, 1024, fsrc);
		if (copy_len)
			fwrite(tmpbuf, 1, copy_len, fdst);
	}while(copy_len == 1024);
	finished = 1;
	
done:
	if (!finished)
		printf("Error to copy banner to file(%s)\n", srcf);
	
	if (tmpbuf)
		free(tmpbuf);
	if (fsrc)
		fclose(fsrc);
	if (fdst)
		fclose(fdst);

	if (replaceit && tmpfile) {
		if (finished) {
			remove(srcf);
			rename(tmpfile, srcf);
		}
		free(tmpfile);
	}

			
	return 0;
}


int append_ban_to_tgt(char *srcf, char *tgtf, char *buf, int buflen, int replaceit, int recursive)
{
	enum PATH_TYPE src_type, dst_type;

	if (srcf == NULL)
		return -1;

	src_type = check_path_type(srcf);
	if (src_type == TYPE_DIR) {
		DIR *dirs;
		struct dirent *dir_ent;
		char *cur_path;
		dirs = opendir(srcf);

		//printf("%s\n", srcf);
		if (!replaceit) {
			dst_type = check_path_type(tgtf);
			if (dst_type == TYPE_INVALID) {
				if (mkdir(tgtf, 0777) != 0) {
					printf("Create target directory failed:%s!\n", tgtf);
					return -1;
				}
			}
			else if (dst_type != TYPE_DIR) {
				printf("Invalid destination path!\n", tgtf);
				return -1;
			}
		}

		/* traversal all files and sub-dirs to handle each file */
		if (dirs) {
			while((dir_ent = readdir(dirs)) != NULL) {
				char *src_path, *dst_path;

				if (strcmp(dir_ent->d_name, ".") == 0 || 
					strcmp(dir_ent->d_name, "..") == 0)
					continue;
				
				src_path = concat_path_file_str(srcf, dir_ent->d_name);
				if (src_path && (!recursive) && 
					(check_path_type(src_path) == TYPE_DIR))
				{
					free(src_path);
					continue;
				}
				dst_path = concat_path_file_str(tgtf, dir_ent->d_name);

				append_ban_to_tgt(src_path, dst_path, buf, 
									buflen, replaceit, recursive);

				if (src_path)
					free(src_path);
				if (dst_path)
					free(dst_path);
			}

			closedir(dirs);
		}
	}
	else if (src_type == TYPE_FILE) {
		if (is_target_file(srcf))
			copy_to_file(srcf, tgtf, buf, buflen, replaceit);
		else {
			if (!replaceit)
				copy_to_file(srcf, tgtf, NULL, 0, 0);
		}
	}
	else
		printf("Invalid file format, filename=%s\n", srcf);
	
	return 0;
}


int get_buf_from_file(char *path, char **buf)
{
	int fsize, size, c;
	char *buf_ptr, *str;
	FILE *fp;

	//printf("banner template=%s\n", path);
	fsize = is_valid_file(path);
	if (fsize <= 0) {
		printf("Invalid ban_file!\n");
		return -1;
	}
	if ((*buf = malloc(fsize)) == NULL)
		return -1;
	buf_ptr = *buf;
	/*printf("banner file(%s) size=%d, buf(0x%p)\n", path, fsize, *buf);*/
	
	if ((fp = fopen(path, "r"))== NULL) {
		printf("fopen %s failed\n", path);
		free(*buf);
		return -1;
	}

	memset(buf_ptr, 0, fsize);
	size = 0;
	while((str = fgets(buf_ptr + size, fsize - size, fp)) != NULL) {
		if (strlen(str) > 0)
			size += strlen(str);
		else
			break;
	}
	fclose(fp);
	
	return size;
}


int main(int argc, char *argv[])
{
	char *ban_file, *srcf, *tgtf, *ban_buf;
	int c, apply_all = 0, ban_len, replaceit;

	ban_file = srcf = tgtf = ban_buf = NULL;
	
	while(1) {
		c = getopt(argc, argv, "b:s:d:e:i:R");
		if (c == -1)
			break;
		switch (c) {
			case 'b':
				if (optarg)
					ban_file = get_file_path(optarg);
				break;
				
			case 's':
				if (optarg)
					srcf = get_file_path(optarg);
				break;
			case 'd':
				if (optarg)
					tgtf = get_file_path(optarg);
				break;
			case 'R':
				apply_all = 1;
				printf("option:Recursive=%d\n", apply_all);
				break;
				
			default:
				printf("Unkown parameter: %c\n", c);
				break;
		}
	}

	if (!(ban_file && srcf)) {
		usage();
		goto done;
	}

	/* get the banner template and check if it is valid or not */
	ban_len = get_buf_from_file(ban_file, &ban_buf);
	//dump_banner(ban_buf, ban_len);
	if (ban_len <= 0) {
		printf("Invalid banner file\n");
		goto done;
	}

	/* Check if the target is a directory or a file */
	replaceit = ((tgtf == NULL) ? 1 : 0);
	if (replaceit)
		printf("Directly change the original source!\n");

	append_ban_to_tgt(srcf, tgtf, ban_buf, ban_len, replaceit, apply_all);

done:	
	/* free all allocated resources */
	if (ban_buf)
		free(ban_buf);
	if (ban_file)
		free(ban_file);
	if (srcf)
		free(srcf);
	if (tgtf)
		free(tgtf);

	return 0;
}

