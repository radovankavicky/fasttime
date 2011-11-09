#define USE_RINTERNALS 1

#include <Rinternals.h>
#include <stdlib.h>

#define DIGIT(X) ((X) >= '0' && (X) <= '9')

/* start of each month in seconds */
static const int cml[] = { 0, 0, 2678400, 5097600, 7776000, 10368000, 13046400, 15638400,
			   18316800, 20995200, 23587200, 26265600, 28857600, 31536000 };

SEXP parse_ts(SEXP str, SEXP sRequiredComp) {
    SEXP res;
    double *tsv;
    int required_components = Rf_asInteger(sRequiredComp);
    int n, i;
    if (TYPEOF(str) != STRSXP) Rf_error("invalid timestamp vector");
    n = LENGTH(str);
    res = Rf_allocVector(REALSXP, n);
    tsv = REAL(res);
    for (i = 0; i < n; i++) {
	const char *c = CHAR(STRING_ELT(str, i));
	int comp = 0;
	if (DIGIT(*c)) {
	    double ts = 0.0;
	    int y = 0, m = 0, d = 0, h = 0, mi = 0;
	    while (DIGIT(*c)) { y = y * 10 + (*c - '0'); c++; }
	    if (y < 100) y += 2000;
	    y -= 1970;
	    if (y >= 0) {
		ts += ((int)((y + 1) / 4)) * 86400;
		ts += y * 31536000;
		comp++;
		while (*c && !DIGIT(*c)) c++;
		if (*c) {
		    while (DIGIT(*c)) { m = m * 10 + (*c - '0'); c++; }
		    if (m > 0 && m < 13) {
			ts += cml[m];
			if (m > 2 && (y & 3) == 2) ts += 86400;
			comp++;
			while (*c && !DIGIT(*c)) c++;
			if (*c) {
			    while (DIGIT(*c)) { d = d * 10 + (*c - '0'); c++; }
			    if (d > 1) ts += (d - 1) * 86400;
			    comp++;
			    while (*c && !DIGIT(*c)) c++;
			    if (*c) {
				while (DIGIT(*c)) { h = h * 10 + (*c - '0'); c++; }
				ts += h * 3600;
				comp++;
				while (*c && !DIGIT(*c)) c++;
				if (*c) {
				    while (DIGIT(*c)) { mi = mi * 10 + (*c - '0'); c++; }
				    ts += mi * 60;
				    comp++;
				    while (*c && !(DIGIT(*c) || *c == '.')) c++;
				    if (*c) {
					ts += atof(c);
					comp++;
				    }
				}
			    }
			}
		    }
		}
	    }
	    if (comp >= required_components)
		tsv[i] = ts;
	}
    }
    return res;
}