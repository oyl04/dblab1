#pragma once

struct Participant {
	int id;
	char country[32];
	char t_size[8];
	char name[64];
	char email[32];
	int solutionCount;
	long firstSolutionAddress;
};

struct Solution {
    int solutionId;
	int participantId;
	int problemId;
	int systemId;
	char verdict[48];
	int solutionSize;
	char solutionTime[32];
	int exists;
	long selfAddress;
    long nextAddress;
};

struct Indexer {
	int id;
	int address;
	int exists;
};
