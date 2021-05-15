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

vector<int> ANDQuery(vector<string> &query, map<string, vector<int>>& index) {
	auto comp = [&](const string& a, const string& b) {
		return index[a].size() < index[b].size();
	}; sort(query.begin(), query.end(), comp);

	vector<int> res = index[query[0]]; 
	for(auto &word : query) if(word != query[0]) {
		res = listIntersection(res, index[word]);
	} return move(res);
}

vector<int> listsUnion(int L, int R, map<string, vector<int>>& index) {
	vector<pair<int, int>> pq; vector<int> res;
	for (int i = L; i <= R; i++) if(index.count(to_string(i))) pq.push_back({ i, 0 });
	auto comp = [&](const pair<int, int>& a, const pair<int, int>& b) {
		return index[to_string(a.first)][a.second] > index[to_string(b.first)][b.second];
	}; make_heap(pq.begin(), pq.end(), comp);

	while (!pq.empty()) {
		pair<int, int> cur = pq.front(); 
		pop_heap(pq.begin(), pq.end()); pq.pop_back();
		string id = to_string(cur.first); int i = cur.second;
		if (i + 1 != (int)index[id].size()) {
			pq.emplace_back(cur.first, i + 1);
			push_heap(pq.begin(), pq.end(), comp);
		} if (res.empty() || index[id][i] != res.back())
			res.push_back(index[id][i]);
	} return move(res);
}