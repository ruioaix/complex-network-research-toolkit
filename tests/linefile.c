#include <check.h>
#include "linefile.h"
#include "mtprand.h"
#include "base.h"
#include <openssl/md5.h>
#include <stdlib.h>
#include <math.h>

#define LINESNUM 10000

static void test_linefile_generate_random_data(int **ia, double **da, char ***sa) {
	int i,j,z;
	for (i = 0; i < 9; ++i) {
		ia[i] = smalloc(LINESNUM * sizeof(int));
		da[i] = smalloc(LINESNUM * sizeof(double));
		sa[i] = smalloc(LINESNUM * sizeof(char *));
	}
	set_timeseed_MTPR();
	for (j = 0; j < 9; ++j) {
		for (i = 0; i < LINESNUM; ++i) {
			ia[j][i] = get_i31_MTPR();
			da[j][i] = get_i31_MTPR() + get_d01_MTPR();
			int length = get_i31_MTPR()%30 + 3;
			sa[j][i] = smalloc(length * sizeof(char));
			for (z = 0; z < length - 1; ++z) {
				sa[j][i][z] = get_i31_MTPR()%26+65;
			}
			sa[j][i][length - 1] = '\0';
		}
	}
}

static void test_linefile_generate_random_data_Num(int **ia, double **da, char ***sa, int ai, int di, int si) {
	int i,j,z;
	for (i = 0; i < ai; ++i) {
		ia[i] = smalloc(LINESNUM * sizeof(int));
	}
	for (i = 0; i < di; ++i) {
		da[i] = smalloc(LINESNUM * sizeof(double));
	}
	for (i = 0; i < si; ++i) {
		sa[i] = smalloc(LINESNUM * sizeof(char *));
	}
	set_timeseed_MTPR();
	for (i = 0; i < LINESNUM; ++i) {
		for (j = 0; j < ai; ++j) {
			ia[j][i] = get_i31_MTPR();
		}
		for (j = 0; j < di; ++j) {
			da[j][i] = get_i31_MTPR() + get_d01_MTPR();
		}
		for (j = 0; j < si; ++j) {
			int length = get_i31_MTPR()%30 + 3;
			sa[j][i] = smalloc(length * sizeof(char));
			for (z = 0; z < length - 1; ++z) {
				sa[j][i][z] = get_i31_MTPR()%26+65;
			}
			sa[j][i][length - 1] = '\0';
		}
	}
}

static void test_linefile_free_random_data(int **ia, double **da, char ***sa) {
	int i,j;
	for (i = 0; i < 9; ++i) {
		free(ia[i]);
		free(da[i]);
		for (j = 0; j < LINESNUM; ++j) {
			free(sa[i][j]);	
		}
		free(sa[i]);
	}
}

START_TEST (int_double_cstring_verify_27columns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	char *filename = "/tmp/cnrt_test_linefile_create";
	FILE *fp = sfopen(filename, "w");
	for (i = 0; i < LINESNUM; ++i) {
		for (j = 0; j < 9; ++j) {
			fprintf(fp, "%d\t", ia[j][i]);
			fprintf(fp, "%.19f\t", da[j][i]);
			fprintf(fp, "%s\t", sa[j][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	struct LineFile *lf = create_LineFile(filename,
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, -1);

	ck_assert_int_eq(lf->linesNum, LINESNUM);
	ck_assert_int_ge(lf->memNum, lf->linesNum);
	ck_assert_str_eq(lf->filename, filename);
	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,ia[0][i]);
		ck_assert_int_eq(lf->i2[i] ,ia[1][i]);
		ck_assert_int_eq(lf->i3[i] ,ia[2][i]);
		ck_assert_int_eq(lf->i4[i] ,ia[3][i]);
		ck_assert_int_eq(lf->i5[i] ,ia[4][i]);
		ck_assert_int_eq(lf->i6[i] ,ia[5][i]);
		ck_assert_int_eq(lf->i7[i] ,ia[6][i]);
		ck_assert_int_eq(lf->i8[i] ,ia[7][i]);
		ck_assert_int_eq(lf->i9[i] ,ia[8][i]);

		ck_assert(fabs(lf->d1[i] -da[0][i]) < TE);
		ck_assert(fabs(lf->d2[i] -da[1][i]) < TE);
		ck_assert(fabs(lf->d3[i] -da[2][i]) < TE);
		ck_assert(fabs(lf->d4[i] -da[3][i]) < TE);
		ck_assert(fabs(lf->d5[i] -da[4][i]) < TE);
		ck_assert(fabs(lf->d6[i] -da[5][i]) < TE);
		ck_assert(fabs(lf->d7[i] -da[6][i]) < TE);
		ck_assert(fabs(lf->d8[i] -da[7][i]) < TE);
		ck_assert(fabs(lf->d9[i] -da[8][i]) < TE);

		ck_assert_str_eq(lf->s1[i] ,sa[0][i]);
		ck_assert_str_eq(lf->s2[i] ,sa[1][i]);
		ck_assert_str_eq(lf->s3[i] ,sa[2][i]);
		ck_assert_str_eq(lf->s4[i] ,sa[3][i]);
		ck_assert_str_eq(lf->s5[i] ,sa[4][i]);
		ck_assert_str_eq(lf->s6[i] ,sa[5][i]);
		ck_assert_str_eq(lf->s7[i] ,sa[6][i]);
		ck_assert_str_eq(lf->s8[i] ,sa[7][i]);
		ck_assert_str_eq(lf->s9[i] ,sa[8][i]);
	}

	free_LineFile(lf);
	test_linefile_free_random_data(ia, da, sa);
}
END_TEST

START_TEST (first_argument_is_NULL)
{
	struct LineFile *lf = create_LineFile(NULL);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	int i;
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile(NULL, "xxx", 1, 2, -199);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);
}
END_TEST

START_TEST (wrong_arguments)
{
	struct LineFile *lf = create_LineFile("/tmp/noexist", 2);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	int i;
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile("/tmp/noexist", 2, -2);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile("/tmp/noexist", 4, -1);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile("/tmp/noexist", "xxx", -1);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile("/tmp/noexist", NULL, -1);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile("/tmp/noexist", 1, 3, 4, -1);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile("/tmp/noexist", 1, 3, 2, 3, 1, 3);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);

	lf = create_LineFile("/tmp/noexist",
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, \
			1, 2, 3, -1);
	ck_assert_int_eq(lf->linesNum, 0);
	ck_assert_int_eq(lf->memNum, 0);
	ck_assert_int_eq(lf->iNum, 9);
	ck_assert_int_eq(lf->dNum, 9);
	ck_assert_int_eq(lf->sNum, 9);
	ck_assert_str_eq(lf->filename, "free-valid");
	for (i = 0; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 0; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 0; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);
}
END_TEST

START_TEST (only_read_part_columns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	char *filename = "/tmp/cnrt_test_linefile_create";
	FILE *fp = sfopen(filename, "w");
	for (i = 0; i < LINESNUM; ++i) {
		for (j = 0; j < 9; ++j) {
			fprintf(fp, "%d\t", ia[j][i]);
			fprintf(fp, "%.19f\t", da[j][i]);
			fprintf(fp, "%s\t", sa[j][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	struct LineFile *lf = create_LineFile(filename, 1, 2, 3, 1, -1);

	ck_assert_int_eq(lf->linesNum, LINESNUM);
	ck_assert_int_ge(lf->memNum, lf->linesNum);
	ck_assert_str_eq(lf->filename, filename);
	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,ia[0][i]);
		ck_assert_int_eq(lf->i2[i] ,ia[1][i]);
		ck_assert(fabs(lf->d1[i] -da[0][i]) < TE);
		ck_assert_str_eq(lf->s1[i] ,sa[0][i]);
	}

	test_linefile_free_random_data(ia, da, sa);
	for (i = 2; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 1; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 1; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);
}
END_TEST

START_TEST (read_more_columns_then_valid)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	char *filename = "/tmp/cnrt_test_linefile_create";
	FILE *fp = sfopen(filename, "w");
	for (i = 0; i < LINESNUM; ++i) {
		for (j = 0; j < 2; ++j) {
			fprintf(fp, "%d\t", ia[j][i]);
			fprintf(fp, "%.19f\t", da[j][i]);
			fprintf(fp, "%s\t", sa[j][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	struct LineFile *lf = create_LineFile(filename, 1, 2, 3, 1, 2, 3, 1, 2, 3, -1);

	ck_assert_int_eq(lf->linesNum, LINESNUM);
	ck_assert_int_ge(lf->memNum, lf->linesNum);
	ck_assert_str_eq(lf->filename, filename);
	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,ia[0][i]);
		ck_assert_int_eq(lf->i2[i] ,ia[1][i]);
		ck_assert_int_eq(lf->i3[i] ,-1);
		ck_assert(fabs(lf->d1[i] -da[0][i]) < TE);
		ck_assert(fabs(lf->d2[i] -da[1][i]) < TE);
		ck_assert(fabs(lf->d3[i] + 1) < TE);
		ck_assert_str_eq(lf->s1[i] ,sa[0][i]);
		ck_assert_str_eq(lf->s2[i] ,sa[1][i]);
		//ck_assert_ptr_eq(lf->s3[i] ,NULL);
		ck_assert(lf->s3[i] == NULL);
	}

	test_linefile_free_random_data(ia, da, sa);
	for (i = 3; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
	}
	for (i = 3; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
	}
	for (i = 3; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
	}
	free_LineFile(lf);
}
END_TEST

START_TEST (use_md5_verify_two_files_27columns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	char *filename = "/tmp/cnrt_test_linefile_print_o";
	FILE *fp = sfopen(filename, "w");
	for (i = 0; i < LINESNUM; ++i) {
		//if (i==3323) printf("%.32f\t%.19f\t", da[0][i], da[0][i]);
		for (j = 0; j < 9; ++j) {
			fprintf(fp, "%d\t", ia[j][i]);
		}
		for (j = 0; j < 9; ++j) {
			fprintf(fp, "%f\t", da[j][i]);
		}
		for (j = 0; j < 9; ++j) {
			fprintf(fp, "%s\t", sa[j][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	struct LineFile *lf = create_LineFile(filename, 
			1, 1, 1, \
			1, 1, 1, \
			1, 1, 1, \
			2, 2, 2, \
			2, 2, 2, \
			2, 2, 2, \
			3, 3, 3, \
			3, 3, 3, \
			3, 3, 3, -1);

	char *fileprint = "/tmp/cnrt_test_linefile_print_f";
	print_LineFile(lf, fileprint);

	unsigned char c1[MD5_DIGEST_LENGTH], c2[MD5_DIGEST_LENGTH];
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];

	FILE *fp1 = sfopen (filename, "rb");
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 1024, fp1)) != 0) {
		MD5_Update (&mdContext, data, bytes);
	}
	MD5_Final (c1,&mdContext);
	fclose(fp1);

	FILE *fp2 = sfopen (fileprint, "rb");
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 1024, fp2)) != 0) {
		MD5_Update (&mdContext, data, bytes);
	}
	MD5_Final (c2,&mdContext);
	fclose(fp2);

	char s1[100], s2[100];
	char *p;
	p = s1;
	for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(p, "%02x", c1[i]);
		p+=2;
	}
	p[0] = '\0';
	p = s2;
	for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(p, "%02x", c2[i]);
		p+=2;
	}
	p[0] = '\0';

	ck_assert_str_eq(s1, s2);

	free_LineFile(lf);
	test_linefile_free_random_data(ia, da, sa);
}
END_TEST

START_TEST (use_md5_verify_two_files_partcolumns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	char *filename = "/tmp/cnrt_test_linefile_print_o";
	FILE *fp = sfopen(filename, "w");
	for (i = 0; i < LINESNUM; ++i) {
		//if (i==3323) printf("%.32f\t%.19f\t", da[0][i], da[0][i]);
		for (j = 0; j < 3; ++j) {
			fprintf(fp, "%d\t", ia[j][i]);
		}
		for (j = 0; j < 5; ++j) {
			fprintf(fp, "%f\t", da[j][i]);
		}
		for (j = 0; j < 6; ++j) {
			fprintf(fp, "%s\t", sa[j][i]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

	struct LineFile *lf = create_LineFile(filename, 
			1, 1, 1, \
			2, 2, 2, \
			2, 2,  \
			3, 3, 3, \
			3, 3, 3, -1);

	char *fileprint = "/tmp/cnrt_test_linefile_print_f";
	print_LineFile(lf, fileprint);

	unsigned char c1[MD5_DIGEST_LENGTH], c2[MD5_DIGEST_LENGTH];
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];

	FILE *fp1 = fopen (filename, "rb");
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 1024, fp1)) != 0) {
		MD5_Update (&mdContext, data, bytes);
	}
	MD5_Final (c1,&mdContext);
	fclose(fp1);

	FILE *fp2 = fopen (fileprint, "rb");
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 1024, fp2)) != 0) {
		MD5_Update (&mdContext, data, bytes);
	}
	MD5_Final (c2,&mdContext);
	fclose(fp2);

	char s1[100], s2[100];
	char *p;
	p = s1;
	for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(p, "%02x", c1[i]);
		p+=2;
	}
	p[0] = '\0';
	p = s2;
	for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(p, "%02x", c2[i]);
		p+=2;
	}
	p[0] = '\0';

	ck_assert_str_eq(s1, s2);

	free_LineFile(lf);
	test_linefile_free_random_data(ia, da, sa);
}
END_TEST

START_TEST (verify_clone_27columns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	struct LineFile *lf = create_LineFile(NULL);
	for (i = 0; i < 9; ++i) {
		*(lf->ilist[i]) = ia[i];
		*(lf->dlist[i]) = da[i];
		*(lf->slist[i]) = sa[i];
	}
	lf->linesNum = lf->memNum = LINESNUM;

	struct LineFile *lf_c = clone_LineFile(lf);

	ck_assert_int_eq(lf_c->linesNum, lf->linesNum);
	ck_assert_int_eq(lf_c->memNum, lf->memNum);
	ck_assert_str_eq(lf_c->filename, "clone_free-valid");

	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,lf_c->i1[i]);
		ck_assert_int_eq(lf->i2[i] ,lf_c->i2[i]);
		ck_assert_int_eq(lf->i3[i] ,lf_c->i3[i]);
		ck_assert_int_eq(lf->i4[i] ,lf_c->i4[i]);
		ck_assert_int_eq(lf->i5[i] ,lf_c->i5[i]);
		ck_assert_int_eq(lf->i6[i] ,lf_c->i6[i]);
		ck_assert_int_eq(lf->i7[i] ,lf_c->i7[i]);
		ck_assert_int_eq(lf->i8[i] ,lf_c->i8[i]);
		ck_assert_int_eq(lf->i9[i] ,lf_c->i9[i]);

		ck_assert(fabs(lf->d1[i] -lf_c->d1[i]) < TE);
		ck_assert(fabs(lf->d2[i] -lf_c->d2[i]) < TE);
		ck_assert(fabs(lf->d3[i] -lf_c->d3[i]) < TE);
		ck_assert(fabs(lf->d4[i] -lf_c->d4[i]) < TE);
		ck_assert(fabs(lf->d5[i] -lf_c->d5[i]) < TE);
		ck_assert(fabs(lf->d6[i] -lf_c->d6[i]) < TE);
		ck_assert(fabs(lf->d7[i] -lf_c->d7[i]) < TE);
		ck_assert(fabs(lf->d8[i] -lf_c->d8[i]) < TE);
		ck_assert(fabs(lf->d9[i] -lf_c->d9[i]) < TE);

		ck_assert_str_eq(lf->s1[i] ,lf_c->s1[i]);
		ck_assert_str_eq(lf->s2[i] ,lf_c->s2[i]);
		ck_assert_str_eq(lf->s3[i] ,lf_c->s3[i]);
		ck_assert_str_eq(lf->s4[i] ,lf_c->s4[i]);
		ck_assert_str_eq(lf->s5[i] ,lf_c->s5[i]);
		ck_assert_str_eq(lf->s6[i] ,lf_c->s6[i]);
		ck_assert_str_eq(lf->s7[i] ,lf_c->s7[i]);
		ck_assert_str_eq(lf->s8[i] ,lf_c->s8[i]);
		ck_assert_str_eq(lf->s9[i] ,lf_c->s9[i]);
	}

	free_LineFile(lf_c);
	free_LineFile(lf);
}
END_TEST

START_TEST (verify_clone_partcolumns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	struct LineFile *lf = create_LineFile(NULL);
	for (i = 0; i < 4; ++i) {
		*(lf->ilist[i]) = ia[i];
	}
	for (i = 0; i < 3; ++i) {
		*(lf->dlist[i]) = da[i];
	}
	for (i = 0; i < 7; ++i) {
		*(lf->slist[i]) = sa[i];
	}
	lf->linesNum = lf->memNum = LINESNUM;

	struct LineFile *lf_c = clone_LineFile(lf);

	ck_assert_int_eq(lf_c->linesNum, lf->linesNum);
	ck_assert_int_eq(lf_c->memNum, lf->memNum);
	ck_assert_str_eq(lf_c->filename, "clone_free-valid");

	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,lf_c->i1[i]);
		ck_assert_int_eq(lf->i2[i] ,lf_c->i2[i]);
		ck_assert_int_eq(lf->i3[i] ,lf_c->i3[i]);
		ck_assert_int_eq(lf->i4[i] ,lf_c->i4[i]);

		ck_assert(fabs(lf->d1[i] -lf_c->d1[i]) < TE);
		ck_assert(fabs(lf->d2[i] -lf_c->d2[i]) < TE);
		ck_assert(fabs(lf->d3[i] -lf_c->d3[i]) < TE);

		ck_assert_str_eq(lf->s1[i] ,lf_c->s1[i]);
		ck_assert_str_eq(lf->s2[i] ,lf_c->s2[i]);
		ck_assert_str_eq(lf->s3[i] ,lf_c->s3[i]);
		ck_assert_str_eq(lf->s4[i] ,lf_c->s4[i]);
		ck_assert_str_eq(lf->s5[i] ,lf_c->s5[i]);
		ck_assert_str_eq(lf->s6[i] ,lf_c->s6[i]);
		ck_assert_str_eq(lf->s7[i] ,lf_c->s7[i]);
	}
	for (i = 4; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf->ilist[i]), NULL);
		ck_assert_ptr_eq(*(lf_c->ilist[i]), NULL);
	}
	for (i = 3; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf->dlist[i]), NULL);
		ck_assert_ptr_eq(*(lf_c->dlist[i]), NULL);
	}
	for (i = 7; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf->slist[i]), NULL);
		ck_assert_ptr_eq(*(lf_c->slist[i]), NULL);
	}

	free_LineFile(lf_c);
	free_LineFile(lf);
}
END_TEST

START_TEST (add_27columns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	struct LineFile *lf = create_LineFile(NULL);
	for (i = 0; i < 9; ++i) {
		*(lf->ilist[i]) = ia[i];
		*(lf->dlist[i]) = da[i];
		*(lf->slist[i]) = sa[i];
	}
	lf->linesNum = lf->memNum = LINESNUM;

	int *ia_2[9];
	double *da_2[9];
	char **sa_2[9];
	test_linefile_generate_random_data(ia_2, da_2, sa_2);
	struct LineFile *lf_2 = create_LineFile(NULL);
	for (i = 0; i < 9; ++i) {
		*(lf_2->ilist[i]) = ia_2[i];
		*(lf_2->dlist[i]) = da_2[i];
		*(lf_2->slist[i]) = sa_2[i];
	}
	lf_2->linesNum = lf_2->memNum = LINESNUM;

	struct LineFile *lf_add = add_LineFile(lf, lf_2);

	ck_assert_int_eq(lf_add->linesNum, lf->linesNum + lf_2->linesNum);
	ck_assert_int_eq(lf_add->memNum, lf_add->linesNum);
	ck_assert_str_eq(lf_add->filename, "free-valid+free-valid");

	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,lf_add->i1[i]);
		ck_assert_int_eq(lf->i2[i] ,lf_add->i2[i]);
		ck_assert_int_eq(lf->i3[i] ,lf_add->i3[i]);
		ck_assert_int_eq(lf->i4[i] ,lf_add->i4[i]);
		ck_assert_int_eq(lf->i5[i] ,lf_add->i5[i]);
		ck_assert_int_eq(lf->i6[i] ,lf_add->i6[i]);
		ck_assert_int_eq(lf->i7[i] ,lf_add->i7[i]);
		ck_assert_int_eq(lf->i8[i] ,lf_add->i8[i]);
		ck_assert_int_eq(lf->i9[i] ,lf_add->i9[i]);

		ck_assert(fabs(lf->d1[i] -lf_add->d1[i]) < TE);
		ck_assert(fabs(lf->d2[i] -lf_add->d2[i]) < TE);
		ck_assert(fabs(lf->d3[i] -lf_add->d3[i]) < TE);
		ck_assert(fabs(lf->d4[i] -lf_add->d4[i]) < TE);
		ck_assert(fabs(lf->d5[i] -lf_add->d5[i]) < TE);
		ck_assert(fabs(lf->d6[i] -lf_add->d6[i]) < TE);
		ck_assert(fabs(lf->d7[i] -lf_add->d7[i]) < TE);
		ck_assert(fabs(lf->d8[i] -lf_add->d8[i]) < TE);
		ck_assert(fabs(lf->d9[i] -lf_add->d9[i]) < TE);

		ck_assert_str_eq(lf->s1[i] ,lf_add->s1[i]);
		ck_assert_str_eq(lf->s2[i] ,lf_add->s2[i]);
		ck_assert_str_eq(lf->s3[i] ,lf_add->s3[i]);
		ck_assert_str_eq(lf->s4[i] ,lf_add->s4[i]);
		ck_assert_str_eq(lf->s5[i] ,lf_add->s5[i]);
		ck_assert_str_eq(lf->s6[i] ,lf_add->s6[i]);
		ck_assert_str_eq(lf->s7[i] ,lf_add->s7[i]);
		ck_assert_str_eq(lf->s8[i] ,lf_add->s8[i]);
		ck_assert_str_eq(lf->s9[i] ,lf_add->s9[i]);
	}

	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf_2->i1[i] ,lf_add->i1[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i2[i] ,lf_add->i2[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i3[i] ,lf_add->i3[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i4[i] ,lf_add->i4[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i5[i] ,lf_add->i5[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i6[i] ,lf_add->i6[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i7[i] ,lf_add->i7[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i8[i] ,lf_add->i8[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i9[i] ,lf_add->i9[i+LINESNUM]);

		ck_assert(fabs(lf_2->d1[i] -lf_add->d1[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d2[i] -lf_add->d2[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d3[i] -lf_add->d3[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d4[i] -lf_add->d4[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d5[i] -lf_add->d5[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d6[i] -lf_add->d6[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d7[i] -lf_add->d7[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d8[i] -lf_add->d8[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d9[i] -lf_add->d9[i+LINESNUM]) < TE);

		ck_assert_str_eq(lf_2->s1[i] ,lf_add->s1[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s2[i] ,lf_add->s2[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s3[i] ,lf_add->s3[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s4[i] ,lf_add->s4[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s5[i] ,lf_add->s5[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s6[i] ,lf_add->s6[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s7[i] ,lf_add->s7[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s8[i] ,lf_add->s8[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s9[i] ,lf_add->s9[i+LINESNUM]);
	}
	free_LineFile(lf_add);
	free_LineFile(lf);
	free_LineFile(lf_2);
}
END_TEST

START_TEST (add_partcolumns)
{
	int *ia[9];
	double *da[9];
	char **sa[9];
	test_linefile_generate_random_data(ia, da, sa);
	int i,j,z;

	struct LineFile *lf = create_LineFile(NULL);
	for (i = 0; i < 4; ++i) {
		*(lf->ilist[i]) = ia[i];
		*(lf->dlist[i]) = da[i];
		*(lf->slist[i]) = sa[i];
	}
	lf->linesNum = lf->memNum = LINESNUM;

	int *ia_2[9];
	double *da_2[9];
	char **sa_2[9];
	test_linefile_generate_random_data(ia_2, da_2, sa_2);
	struct LineFile *lf_2 = create_LineFile(NULL);
	for (i = 0; i < 4; ++i) {
		*(lf_2->ilist[i]) = ia_2[i];
		*(lf_2->dlist[i]) = da_2[i];
		*(lf_2->slist[i]) = sa_2[i];
	}
	lf_2->linesNum = lf_2->memNum = LINESNUM;

	struct LineFile *lf_add = add_LineFile(lf, lf_2);

	ck_assert_int_eq(lf_add->linesNum, lf->linesNum + lf_2->linesNum);
	ck_assert_int_eq(lf_add->memNum, lf_add->linesNum);
	ck_assert_str_eq(lf_add->filename, "free-valid+free-valid");

	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,lf_add->i1[i]);
		ck_assert_int_eq(lf->i2[i] ,lf_add->i2[i]);
		ck_assert_int_eq(lf->i3[i] ,lf_add->i3[i]);
		ck_assert_int_eq(lf->i4[i] ,lf_add->i4[i]);

		ck_assert(fabs(lf->d1[i] -lf_add->d1[i]) < TE);
		ck_assert(fabs(lf->d2[i] -lf_add->d2[i]) < TE);
		ck_assert(fabs(lf->d3[i] -lf_add->d3[i]) < TE);
		ck_assert(fabs(lf->d4[i] -lf_add->d4[i]) < TE);

		ck_assert_str_eq(lf->s1[i] ,lf_add->s1[i]);
		ck_assert_str_eq(lf->s2[i] ,lf_add->s2[i]);
		ck_assert_str_eq(lf->s3[i] ,lf_add->s3[i]);
		ck_assert_str_eq(lf->s4[i] ,lf_add->s4[i]);
	}

	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf_2->i1[i] ,lf_add->i1[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i2[i] ,lf_add->i2[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i3[i] ,lf_add->i3[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i4[i] ,lf_add->i4[i+LINESNUM]);

		ck_assert(fabs(lf_2->d1[i] -lf_add->d1[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d2[i] -lf_add->d2[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d3[i] -lf_add->d3[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d4[i] -lf_add->d4[i+LINESNUM]) < TE);

		ck_assert_str_eq(lf_2->s1[i] ,lf_add->s1[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s2[i] ,lf_add->s2[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s3[i] ,lf_add->s3[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s4[i] ,lf_add->s4[i+LINESNUM]);
	}

	for (i = 4; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf_add->ilist[i]), NULL);
	}
	for (i = 4; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf_add->dlist[i]), NULL);
	}
	for (i = 4; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf_add->slist[i]), NULL);
	}

	free_LineFile(lf_add);
	free_LineFile(lf);
	free_LineFile(lf_2);
}
END_TEST

START_TEST (add_LineFile_with_diffstruct)
{
	int *ia[3];
	double *da[5];
	char **sa[7];
	test_linefile_generate_random_data_Num(ia, da, sa, 3, 5, 7);
	int i,j,z;

	struct LineFile *lf = create_LineFile(NULL);
	for (i = 0; i < 3; ++i) {
		*(lf->ilist[i]) = ia[i];
	}
	for (i = 0; i < 5; ++i) {
		*(lf->dlist[i]) = da[i];
	}
	for (i = 0; i < 7; ++i) {
		*(lf->slist[i]) = sa[i];
	}
	lf->linesNum = lf->memNum = LINESNUM;

	int *ia_2[6];
	double *da_2[4];
	char **sa_2[2];
	test_linefile_generate_random_data_Num(ia_2, da_2, sa_2, 6, 4, 2);
	struct LineFile *lf_2 = create_LineFile(NULL);
	for (i = 0; i < 6; ++i) {
		*(lf_2->ilist[i]) = ia_2[i];
	}
	for (i = 0; i < 4; ++i) {
		*(lf_2->dlist[i]) = da_2[i];
	}
	for (i = 0; i < 2; ++i) {
		*(lf_2->slist[i]) = sa_2[i];
	}
	lf_2->linesNum = lf_2->memNum = LINESNUM;

	struct LineFile *lf_add = add_LineFile(lf, lf_2);

	ck_assert_int_eq(lf_add->linesNum, lf->linesNum + lf_2->linesNum);
	ck_assert_int_eq(lf_add->memNum, lf_add->linesNum);
	ck_assert_str_eq(lf_add->filename, "free-valid+free-valid");

	double TE = 1E-19;
	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf->i1[i] ,lf_add->i1[i]);
		ck_assert_int_eq(lf->i2[i] ,lf_add->i2[i]);
		ck_assert_int_eq(lf->i3[i] ,lf_add->i3[i]);

		ck_assert(fabs(lf->d1[i] -lf_add->d1[i]) < TE);
		ck_assert(fabs(lf->d2[i] -lf_add->d2[i]) < TE);
		ck_assert(fabs(lf->d3[i] -lf_add->d3[i]) < TE);
		ck_assert(fabs(lf->d4[i] -lf_add->d4[i]) < TE);

		ck_assert_str_eq(lf->s1[i] ,lf_add->s1[i]);
		ck_assert_str_eq(lf->s2[i] ,lf_add->s2[i]);
	}

	for (i = 0; i < LINESNUM; ++i) {
		ck_assert_int_eq(lf_2->i1[i] ,lf_add->i1[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i2[i] ,lf_add->i2[i+LINESNUM]);
		ck_assert_int_eq(lf_2->i3[i] ,lf_add->i3[i+LINESNUM]);

		ck_assert(fabs(lf_2->d1[i] -lf_add->d1[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d2[i] -lf_add->d2[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d3[i] -lf_add->d3[i+LINESNUM]) < TE);
		ck_assert(fabs(lf_2->d4[i] -lf_add->d4[i+LINESNUM]) < TE);

		ck_assert_str_eq(lf_2->s1[i] ,lf_add->s1[i+LINESNUM]);
		ck_assert_str_eq(lf_2->s2[i] ,lf_add->s2[i+LINESNUM]);
	}

	for (i = 3; i < lf->iNum; ++i) {
		ck_assert_ptr_eq(*(lf_add->ilist[i]), NULL);
	}
	for (i = 4; i < lf->dNum; ++i) {
		ck_assert_ptr_eq(*(lf_add->dlist[i]), NULL);
	}
	for (i = 2; i < lf->sNum; ++i) {
		ck_assert_ptr_eq(*(lf_add->slist[i]), NULL);
	}

	free_LineFile(lf_add);
	free_LineFile(lf);
	free_LineFile(lf_2);
}
END_TEST

Suite *linefile_suite(void) {
	Suite *s = suite_create("linefile.c");

	TCase *tc_create = tcase_create("create_LineFile");
	tcase_set_timeout(tc_create, 0);
	tcase_add_test(tc_create, int_double_cstring_verify_27columns);
	tcase_add_test(tc_create, first_argument_is_NULL);
	tcase_add_test(tc_create, wrong_arguments);
	tcase_add_test(tc_create, only_read_part_columns);
	tcase_add_test(tc_create, read_more_columns_then_valid);
	suite_add_tcase(s, tc_create);

	TCase *tc_print= tcase_create("print_LineFile");
	tcase_set_timeout(tc_print, 0);
	tcase_add_test(tc_print, use_md5_verify_two_files_partcolumns);
	tcase_add_test(tc_print, use_md5_verify_two_files_27columns);
	suite_add_tcase(s, tc_print);

	TCase *tc_clone= tcase_create("clone_LineFile");
	tcase_set_timeout(tc_clone, 0);
	tcase_add_test(tc_clone, verify_clone_27columns);
	tcase_add_test(tc_clone, verify_clone_partcolumns);
	suite_add_tcase(s, tc_clone);

	TCase *tc_add= tcase_create("add_LineFile");
	tcase_set_timeout(tc_add, 0);
	tcase_add_test(tc_add, add_27columns);
	tcase_add_test(tc_add, add_partcolumns);
	tcase_add_test(tc_add, add_LineFile_with_diffstruct);
	suite_add_tcase(s, tc_add);
	return s;
}
