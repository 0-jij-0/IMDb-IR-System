#pragma once

#include "Indexing.h"

vector<int> listIntersection2P(const vector<int>& A, const vector<int>& B) {
	vector<int> res; int n = (int)A.size(), m = (int)B.size();
	int i = 0, j = 0; while (i != n && j != m) {
		if (A[i] < B[j]) { i++; continue; }
		if (A[i] > B[j]) { j++; continue; }
		res.push_back(A[i]); i++; j++;
	} return move(res);
}

vector<int> listIntersectionBS(const vector<int>& A, const vector<int>& B) {
	vector<int> res; int n = (int)A.size(), m = (int)B.size();
	int i = 0, j = 0; while (i != n) {
		auto it = lower_bound(B.begin() + j, B.end(), A[i]);
		if (it == B.end()) { break; }
		if (*it == A[i]) { res.push_back(A[i]); }
		i++; j = it - B.begin();
	} return move(res);
}

vector<int> listIntersection(const vector<int> &A, const vector<int> &B) {
	const vector<int>& a = (A.size() > B.size()) ? B : A;
	const vector<int>& b = (A.size() > B.size()) ? A : B;
	ld ratio = (b.size() + 0.0) / (a.size() + 0.0);
	return ratio > 9.0 ? listIntersectionBS(a, b) : listIntersection2P(a, b);
}