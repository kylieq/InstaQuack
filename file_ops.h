#ifndef FILE_OPS_H
#define FILE_OPS_H

struct FileLines {
	char *FileName;
	char **Lines;
	int LineCount;
};

struct LineArguments {
	int count;
	char **args;
};

struct FileLines *LoadAFile(char *filename);

void FreeFile(struct FileLines *file);

#endif
