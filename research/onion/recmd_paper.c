/**
 * ./run data/movielens/movielens_2c 1 -0.8 -0.75 0.2
 * ./run data/netflix/netflix_2c 1 -0.7 -0.75 0.2 
 *
 */

//#define NDEBUG  //for assert
//#include <stdio.h>
//#include <assert.h>
//#include <time.h>
#include <stdlib.h>
#include "base.h"
#include "linefile.h"
#include "bip.h"
#include "net.h"
#include "mtprand.h"

int main(int argc, char **argv)
{
	print_time();

	set_timeseed_MTPR();
	
	char *netfilename;
	//double HNBI_param, RENBI_param, hybrid_param; 
	int loopNum;
	if (argc == 1) {
		netfilename = "../Downloads/movielens_2c.txt";
		loopNum = 2;
		//HNBI_param = -0.8;
		//RENBI_param= -0.75;
		//hybrid_param = 0.2;
	}
	if (argc == 6) {
		netfilename = argv[1];
		char *pEnd;
		loopNum = strtol(argv[2], &pEnd, 10);
		//HNBI_param = strtod(argv[3], &pEnd);
		//RENBI_param= strtod(argv[4], &pEnd);
		//hybrid_param = strtod(argv[5], &pEnd);
	}

	//divide file into two part: train and test.
	//train will contain every user and every item.
	//but test maybe not.
	struct LineFile *net_file = create_LineFile(netfilename, 1, 1, -1);
	struct Bip *set = create_Bip(net_file);

	struct MetricsBip *mass_result = create_MetricsBip();
	//struct MetricsBip *heats_result = create_MetricsBip();
	//struct MetricsBip *HNBI_result = create_MetricsBip();
	//struct MetricsBip *RENBI_result = create_MetricsBip();
	//struct MetricsBip *hybrid_result = create_MetricsBip();

	int i;
	for (i=0; i<loopNum; ++i) {
		struct LineFile *testfile, *trainfile;
		divide_Bip(set, 0.1, &testfile, &trainfile);

		struct Bip *train = create_Bip(trainfile);
		struct Bip *test = create_Bip(testfile);
		free_LineFile(trainfile);
		free_LineFile(testfile);

		//the similarity is get from traini1
		struct LineFile *similarity_file = similarity_CF_HalfBip(train->right, train->left->maxId);
		struct Net *train_right_Sim = create_Net(similarity_file);
		free_LineFile(similarity_file);

		//recommendation
		struct MetricsBip *r1 = mass_Bip(train, test, train_right_Sim);
		//struct MetricsBip *r11= heats_Bip(traini1, traini2, testi1, testi2, trainSim);
		//struct MetricsBip *r2 = HNBI_Bip(traini1, traini2, testi1, testi2, trainSim, HNBI_param);
		//struct MetricsBip *r3 = RENBI_Bip(traini1, traini2, testi1, testi2, trainSim, RENBI_param);
		//struct MetricsBip *r4 = hybrid_Bip(traini1, traini2, testi1, testi2, trainSim, hybrid_param);

		mass_result->R += r1->R;
		mass_result->PL += r1->PL;
		mass_result->HL += r1->HL;
		mass_result->IL += r1->IL;
		mass_result->NL += r1->NL;
		//heats_result->R += r11->R;
		//heats_result->PL += r11->PL;
		//heats_result->HL += r11->HL;
		//heats_result->IL += r11->IL;
		//heats_result->NL += r11->NL;
		//HNBI_result->R +=  r2->R;
		//HNBI_result->PL += r2->PL;
		//HNBI_result->HL += r2->HL;
		//HNBI_result->IL += r2->IL;
		//HNBI_result->NL += r2->NL;
		//RENBI_result->R +=  r3->R;
		//RENBI_result->PL += r3->PL;
		//RENBI_result->HL += r3->HL;
		//RENBI_result->IL += r3->IL;
		//RENBI_result->NL += r3->NL;
		//hybrid_result->R +=  r4->R;
		//hybrid_result->PL += r4->PL;
		//hybrid_result->HL += r4->HL;
		//hybrid_result->IL += r4->IL;
		//hybrid_result->NL += r4->NL;

		free_Net(train_right_Sim);
		free_Bip(train);
		free_Bip(test);
		free_MetricsBip(r1);
		//free_MetricsBip(r11);
		//free_MetricsBip(r2);
		//free_MetricsBip(r3);
		//free_MetricsBip(r4);
	}
	
	printf("\tmass\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, mass_result->R/loopNum, mass_result->PL/loopNum, mass_result->IL/loopNum, mass_result->HL/loopNum, mass_result->NL/loopNum);
	//printf("\tHNBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, HNBI_result->R/loopNum, HNBI_result->PL/loopNum, HNBI_result->IL/loopNum, HNBI_result->HL/loopNum, HNBI_result->NL/loopNum);
	//printf("\tRENBI\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, RENBI_result->R/loopNum, RENBI_result->PL/loopNum, RENBI_result->IL/loopNum, RENBI_result->HL/loopNum, RENBI_result->NL/loopNum);
	//printf("\thybrid\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, hybrid_result->R/loopNum, hybrid_result->PL/loopNum, hybrid_result->IL/loopNum, hybrid_result->HL/loopNum, hybrid_result->NL/loopNum);
	//printf("\theats\tloopNum: %d, R: %f, PL: %f, IL: %f, HL: %f, NL: %f\n", loopNum, heats_result->R/loopNum, heats_result->PL/loopNum, heats_result->IL/loopNum, heats_result->HL/loopNum, heats_result->NL/loopNum);

	free_LineFile(net_file);
	free_Bip(set);
	free_MetricsBip(mass_result);
	//free_MetricsBip(heats_result);
	//free_MetricsBip(HNBI_result);
	//free_MetricsBip(RENBI_result);
	//free_MetricsBip(hybrid_result);

	print_time();
	return 0;
}
