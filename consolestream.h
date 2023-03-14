#pragma once
#include <string.h>
#include <stdio.h>
#include "Participant.h"
#include "Structures.h"

void inputParticipant(struct Participant* participant) {
	char country[32];
	char t_size[8];
	char name[64];
	char email[32];
	while (getchar()!='\n'){}
	printf("Enter participant\'s name: ");
	gets(participant->name);
	printf("Enter participant\'s email: ");
	gets(participant->email);
	printf("Enter participant\'s country: ");
	gets(participant->country);
	printf("Enter participant\'s size of T-shirt: ");
	gets(participant->t_size);
}

void printParticipant(struct Participant participant) {
	printf("Participant\'s name: %s\n", participant.name);
	printf("Participant\'s email: %s\n", participant.email);
	printf("Participant\'s country: %s\n", participant.country);
	printf("Participant\'s t_size: %s\n", participant.t_size);
}

void inputSolution(struct Solution* solution) {
	int problemId;
	int systemId;
	char verdict[48];
	int solutionSize;
	char solutionTime[32];
	printf("Enter solution problem ID: ");
	scanf("%d", &problemId);
	solution->problemId = problemId;
	printf("Enter solution system ID: ");
	scanf("%d", &systemId);
	solution->systemId = systemId;
	while (getchar()!='\n'){}
	printf("Enter solution verdict: ");
	gets(solution->verdict);
	printf("Enter solution size (KB): ");
	scanf("%d", &solutionSize);
	solution->solutionSize = solutionSize;
	while (getchar()!='\n'){}
	printf("Enter solution time in format (DD/MM/YY HH:MM): ");
	gets(solution->solutionTime);
}

void printSolution(struct Solution solution, struct Participant participant) {
	printf("Participant name: %s\n", participant.name);
	printf("Solution problem ID: %d\n", solution.problemId);
	printf("Solution system ID: %d\n", solution.systemId);
	printf("Solution verdict: %s\n", solution.verdict);
	printf("Solution size (KB): %d\n", solution.solutionSize);
	printf("Solution time: %s\n", solution.solutionTime);
}
