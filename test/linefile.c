#include <check.h>
#include "linefile.h"
#include "mtprand.h"
#include "base.h"
#include "stdlib.h"

START_TEST (test_linefile_create)
{
	int linesNum = 100000;
	int *ia[9];
	double *da[9];
	char **sa[9];
	int i,j,z;
	for (i = 0; i < 9; ++i) {
		ia[i] = smalloc(linesNum * sizeof(int));
		da[i] = smalloc(linesNum * sizeof(double));
		sa[i] = smalloc(linesNum * sizeof(char *));
	}
	set_timeseed_MTPR();
	for (j = 0; j < 9; ++j) {
		for (i = 0; i < linesNum; ++i) {
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

	char *filename = "/tmp/cnrt_test_linefile_create";
	FILE *fp = fopen(filename, "w");
	fileError(fp, "xx");
	for (i = 0; i < linesNum; ++i) {
		for (j = 0; j < 9; ++j) {
			fprintf(fp, "%d\t", ia[j][i]);
			fprintf(fp, "%f\t", da[j][i]);
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
	
	double TE = 0.0000001;
	for (i = 0; i < linesNum; ++i) {
		ck_assert_int_eq(lf->i1[i] ,ia[0][i]);
		ck_assert_int_eq(lf->i2[i] ,ia[1][i]);
		ck_assert_int_eq(lf->i3[i] ,ia[2][i]);
		ck_assert_int_eq(lf->i4[i] ,ia[3][i]);
		ck_assert_int_eq(lf->i5[i] ,ia[4][i]);
		ck_assert_int_eq(lf->i6[i] ,ia[5][i]);
		ck_assert_int_eq(lf->i7[i] ,ia[6][i]);
		ck_assert_int_eq(lf->i8[i] ,ia[7][i]);
		ck_assert_int_eq(lf->i9[i] ,ia[8][i]);

		ck_assert(abs(lf->d1[i] -da[0][i]) < TE);
		ck_assert(abs(lf->d2[i] -da[1][i]) < TE);
		ck_assert(abs(lf->d3[i] -da[2][i]) < TE);
		ck_assert(abs(lf->d4[i] -da[3][i]) < TE);
		ck_assert(abs(lf->d5[i] -da[4][i]) < TE);
		ck_assert(abs(lf->d6[i] -da[5][i]) < TE);
		ck_assert(abs(lf->d7[i] -da[6][i]) < TE);
		ck_assert(abs(lf->d8[i] -da[7][i]) < TE);
		ck_assert(abs(lf->d9[i] -da[8][i]) < TE);

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
	for (i = 0; i < 9; ++i) {
		free(ia[i]);
		free(da[i]);
		for (j = 0; j < linesNum; ++j) {
			free(sa[i][j]);	
		}
		free(sa[i]);
	}
}
END_TEST

START_TEST (test_linefile_clone)
{
}
END_TEST

Suite *linefile_suite(void) {
	Suite *s = suite_create("linefile");

	TCase *tc_create = tcase_create("create");
	tcase_set_timeout(tc_create, 0);
	tcase_add_test(tc_create, test_linefile_create);
	suite_add_tcase(s, tc_create);

	TCase *tc_clone= tcase_create("clone");
	tcase_add_test(tc_clone, test_linefile_clone);
	suite_add_tcase(s, tc_clone);

	return s;
}
