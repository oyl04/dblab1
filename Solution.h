#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include "Info.h"
#include "Participant.h"

#define SOLUTION_DATA "solution.fl"
#define SOLUTION_GARBAGE "solution_garbage.txt"
#define SOLUTION_SIZE sizeof(struct Solution)

struct Solution linkLoop(FILE *database, struct Participant* participant, struct Solution *previous);

int updateParticipant(struct Participant participant, char* error);

struct Solution linkLoop(FILE *database, struct Participant *participant, struct Solution *previous) {
    for (int i = 0; i < (*participant).solutionCount; i++) {
        fread(previous, SOLUTION_SIZE, 1, database);
        fseek(database, (*previous).nextAddress, SEEK_SET);
    }
    return (*previous);
}

void reopenDatabase(FILE* database) {
	fclose(database);
	database = fopen(SOLUTION_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Participant participant, struct Solution solution) {
	reopenDatabase(database);
	struct Solution previous;
	fseek(database, participant.firstSolutionAddress, SEEK_SET);
    previous = linkLoop(database, &participant, &previous);
    previous.nextAddress = solution.selfAddress;
	fwrite(&previous, SOLUTION_SIZE, 1, database);
}

void relinkAddresses(FILE* database, struct Solution previous, struct Solution solution, struct Participant* participant) {
	if (solution.selfAddress == participant->firstSolutionAddress) {
		if (solution.selfAddress == solution.nextAddress) {
            participant->firstSolutionAddress = -1;
		} else {
            participant->firstSolutionAddress = solution.nextAddress;
		}
	} else {
		if (solution.selfAddress == solution.nextAddress) {
			previous.nextAddress = previous.selfAddress;
		}
		else {
			previous.nextAddress = solution.nextAddress;
		}
		fseek(database, previous.selfAddress, SEEK_SET);
		fwrite(&previous, SOLUTION_SIZE, 1, database);
	}
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Solution* record) {
	long* deletedIds = (long * )malloc(garbageCount * sizeof(long));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%ld", deletedIds + i);
	}

	record->selfAddress = deletedIds[0];
	record->nextAddress = deletedIds[0];

	fclose(garbageZone);
	fopen(SOLUTION_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %ld", deletedIds[i]);
	}

	free(deletedIds);
	fclose(garbageZone);
}

void noteDeletedSolution(long address) {
    FILE* garbageZone = fopen(SOLUTION_GARBAGE, "rb");
    int garbageCount;
    fscanf(garbageZone, "%d", &garbageCount);
    long* deletedAddresses = (long * )malloc(garbageCount * sizeof(long));

    for (int i = 0; i < garbageCount; i++) {
        fscanf(garbageZone, "%ld", deletedAddresses + i);
    }

    fclose(garbageZone);
    garbageZone = fopen(SOLUTION_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount + 1);

    for (int i = 0; i < garbageCount; i++) {
        fprintf(garbageZone, " %ld", deletedAddresses[i]);
    }

    fprintf(garbageZone, " %ld", address);
    free(deletedAddresses);
    fclose(garbageZone);
}

int getSolution(struct Participant participant, struct Solution* solution, int solutionId, char* error) {
    if (!participant.solutionCount) {
        strcpy(error, "The participant is empty");
        return 0;
    }
    FILE* database = fopen(SOLUTION_DATA, "rb");
	fseek(database, participant .firstSolutionAddress, SEEK_SET);
    fread(solution, SOLUTION_SIZE, 1, database);

    for (int i = 0; i < participant.solutionCount; i++) {
        if (solution->solutionId == solutionId) {
            fclose(database);
            return 1;
        }
        fseek(database, solution->nextAddress, SEEK_SET);
        fread(solution, SOLUTION_SIZE, 1, database);
    }
    strcpy(error, "No such solution in database");
    fclose(database);
    return 0;
}

int insertSolution(struct Participant participant, struct Solution solution, char* error) {
    solution.exists = 1;
	FILE* database = fopen(SOLUTION_DATA, "a+b");
	FILE* garbageZone = fopen(SOLUTION_GARBAGE, "a+b");
	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);
	if (garbageCount) {
		overwriteGarbageAddress(garbageCount, garbageZone, &solution);
		reopenDatabase(database);
		fseek(database, solution.selfAddress, SEEK_SET);
	} else {
		fseek(database, 0, SEEK_END);
		int dbSize = ftell(database);
        solution.selfAddress = dbSize;
		solution.nextAddress = dbSize;
	}
	fwrite(&solution, SOLUTION_SIZE, 1, database);
	if (!participant.solutionCount) {
		participant.firstSolutionAddress = solution.selfAddress;
	} else {
		linkAddresses(database, participant, solution);
	}
	fclose(database);
	participant.solutionCount++;
    updateParticipant(participant, error);
	return 1;
}

int updateSolution(struct Solution solution, int solutionId) {
	FILE* database = fopen(SOLUTION_DATA, "r+b");
	fseek(database, solution.selfAddress, SEEK_SET);
	fwrite(&solution, SOLUTION_SIZE, 1, database);
	fclose(database);
	return 1;
}

void deleteSolution(struct Participant participant, struct Solution solution, int solutionId, char* error) {
	FILE* database = fopen(SOLUTION_DATA, "r+b");
	struct Solution previous;
	fseek(database, participant.firstSolutionAddress, SEEK_SET);
	do {
		fread(&previous, SOLUTION_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}
	while (previous.nextAddress != solution.selfAddress && solution.selfAddress != participant.firstSolutionAddress);

	relinkAddresses(database, previous, solution, &participant);
    noteDeletedSolution(solution.selfAddress);
	solution.exists = 0;

	fseek(database, solution.selfAddress, SEEK_SET);
	fwrite(&solution, SOLUTION_SIZE, 1, database);
	fclose(database);

	participant.solutionCount--;
    updateParticipant(participant, error);
}
