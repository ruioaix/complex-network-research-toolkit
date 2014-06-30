/**
 * ./run data/movielens/movielens_2c 1 -0.8 -0.75 0.2
 * ./run data/netflix/netflix_2c 1 -0.7 -0.75 0.2 
 *
 */

//#define NDEBUG  //for assert
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "base.h"
#include "linefile.h"
#include "bip.h"
#include "iidnet.h"
#include "mtprand.h"

int main(int argc, char **argv)
{
	print_time();
	set_timeseed_MTPR();
	
	char *netfilename;
	double HNBI_param, RENBI_param, hybrid_param; 
	int loopNum;
	if (argc == 1) {
		netfilename = "movielens_2c";
		loopNum = 2;
		HNBI_param = -0.8;
		RENBI_param= -0.75;
		hybrid_param = 0.2;
	}
	if (argc == 6) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
		HNBI_param = strtod(argv[3], &pEnd);
		RENBI_param= strtod(argv[4], &pEnd);
		hybrid_param = strtod(argv[5], &pEnd);
	}

	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct LineFile *net_file = create_LineFile(netfilename, 1, 1, -1);
	struct Bip *seti1 = create_Bip(net_file, 1);
	struct Bip *seti2 = create_Bip(net_file, 2);
	int i;

	struct Metrics_Bip *mass_result = create_MetricsBip();
	struct Metrics_Bip *heats_result = create_MetricsBip();
	struct Metrics_Bip *HNBI_result = create_MetricsBip();
	struct Metrics_Bip *RENBI_result = create_MetricsBip();
	struct Metrics_Bip *hybrid_result = create_MetricsBip();

	for (i=0; i<loopNum; ++i) {
		printf("\n");
		struct LineFile *testfile, *trainfile;
		divide_Bip(seti1, seti2, 0.1, &testfile, &trainfile);

		struct Bip *traini1= create_Bip(trainfile, 1);
		struct Bip *traini2 = create_Bip(trainfile, 2);
		struct Bip *testi1 = create_Bip(testfile, 1);
		struct Bip *testi2 = create_Bip(testfile, 2);
		free_LineFile(trainfile);
		free_LineFile(testfile);

		//the similarity is get from traini1
		struct LineFile *similarity = similarity_Bip(traini1, traini2, 2);
		struct iidNet *trainSim = create_iidNet(similarity);
		free_LineFile(similarity);

		//recommendation
		struct Metrics_Bip *r1 = mass_Bip(traini1, traini2, testi1, testi2, trainSim);
		struct Metrics_Bip *r11= heats_Bip(traini1, traini2, testi1, testi2, trainSim);
		struct Metrics_Bip *r2 = HNBI_Bip(traini1, traini2, testi1, testi2, trainSim, HNBI_param);
		struct Metrics_Bip *r3 = RENBI_Bip(traini1, traini2, testi1, testi2, trainSim, RENBI_param);
		struct Metrics_Bip *r4 = hybrid_Bip(traini1, traini2, testi1, testi2, trainSim, hybrid_param);

		mass_result->R += r1->R;
		mass_result->PL += r1->PL;
		mass_result->HL += r1->HL;
		mass_result->IL += r1->IL;
		mass_result->NL += r1->NL;
		heats_result->R += r11->R;
		heats_result->PL += r11->PL;
		heats_result->HL += r11->HL;
		heats_result->IL += r11->IL;
		heats_result->NL += r11->NL;
		HNBI_result->R +=  r2->R;
		HNBI_result->PL += r2->PL;
		HNBI_result->HL += r2->HL;
		HNBI_result->IL += r2->IL;
		HNBI_result->NL += r2->NL;
		RENBI_result->R +=  r3->R;
		RENBI_result->PL += r3->PL;
		RENBI_result->HL += r3->HL;
		RENBI_result->IL += r3->IL;
		RENBI_result->NL += r3->NL;
		hybrid_result->R +=  r4->R;
		hybrid_result->PL += r4->PL;
		hybrid_result->HL += r4->HL;
		hybrid_result->IL += r4->IL;
		hybrid_result->NL += r4->NL;

		free_iidNet(trainSim);
		free_Bip(traini1);
		free_Bip(traini2);
		free_Bip(testi1);
		free_Bip(testi2);
		free_MetricsBip(r1);
		free_MetricsBip(r11);
		free_MetricsBip(r2);
		free_MetricsBip(r3);
		free_MetricsBip(r4);
	}
	
	printf("RESULT:\n");
	printf("\tmass\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, mass_result->R/loopNum, mass_result->PL/loopNum, mass_result->IL/loopNum, mass_result->HL/loopNum, mass_result->NL/loopNum);
	printf("\tHNBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, HNBI_result->R/loopNum, HNBI_result->PL/loopNum, HNBI_result->IL/loopNum, HNBI_result->HL/loopNum, HNBI_result->NL/loopNum);
	printf("\tRENBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, RENBI_result->R/loopNum, RENBI_result->PL/loopNum, RENBI_result->IL/loopNum, RENBI_result->HL/loopNum, RENBI_result->NL/loopNum);
	printf("\thybrid\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum);
	printf("\theats\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, heats_result->R/loopNum, heats_result->PL/loopNum, heats_result->IL/loopNum, heats_result->HL/loopNum, heats_result->NL/loopNum);

	free_LineFile(net_file);
	free_Bip(seti1);
	free_Bip(seti2);
	free_MetricsBip(mass_result);
	free_MetricsBip(heats_result);
	free_MetricsBip(HNBI_result);
	free_MetricsBip(RENBI_result);
	free_MetricsBip(hybrid_result);

	print_time();
	return 0;
}
